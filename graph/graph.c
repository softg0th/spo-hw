#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <antlr3.h>

#include "graphStructures.h"

int nodeId = 0;

static struct cfgNode **allNodes = NULL;
static int allNodesCount = 0;
static int binCounter = 0;
static struct parseTree* currentParseTree = NULL;

static int parseTreeIdCounter = 100000;

struct breakStack {
    struct cfgNode *breakTarget;
    struct breakStack *next;
};

static char* safeStrdup(const char *s) {
    return s ? strdup(s) : NULL;
}

static void addError(struct errorList **elist, const char *msg)
{
    if (!elist || !msg) return;

    struct errorList *node = malloc(sizeof(struct errorList));
    if (!node) return;

    node->message = strdup(msg);
    node->next = NULL;

    if (*elist == NULL) {
        *elist = node;
    } else {
        struct errorList *cur = *elist;
        while (cur->next) {
            cur = cur->next;
        }
        cur->next = node;
    }
}


static void printErrors(const struct errorList *elist)
{
    if (!elist) return;
    fprintf(stdout, "\n==== Detected Errors ====\n");
    int idx = 1;
    for (const struct errorList *cur = elist; cur; cur = cur->next) {
        fprintf(stdout, "%d) %s\n", idx++, cur->message);
    }
    fprintf(stdout, "=========================\n");
}

static void freeErrors(struct errorList **elist)
{
    if (!elist || !*elist) return;

    struct errorList *cur = *elist;
    while (cur) {
        struct errorList *tmp = cur;
        cur = cur->next;
        free(tmp->message);
        free(tmp);
    }
    *elist = NULL;
}


static void addNodeToGlobalList(struct cfgNode *node) {
    struct cfgNode **tmp = realloc(allNodes, sizeof(struct cfgNode*)*(allNodesCount+1));
    if (!tmp) {
        fprintf(stderr,"Error: realloc for allNodes failed\n");
        exit(1);
    }
    allNodes = tmp;
    allNodes[allNodesCount++] = node;
}

static bool isNodeInList(struct cfgNode *candidate) {
    if (!candidate) return false;
    for (int i=0; i<allNodesCount; i++) {
        if (allNodes[i] == candidate) {
            return true;
        }
    }
    return false;
}

char* generateBinopName(pANTLR3_BASE_TREE tree) {
    if (tree == NULL) return NULL;

    char* nodeName = (char*)tree->toString(tree)->chars;
    if (!nodeName) return NULL;

    const char* binaryOps[] = {
        "+", "-", "*", "/", "%", "<<", ">>", "&", "^", "|",
        "=", "==", "!=", ">", "<"
    };

    const char* binopNames[] = {
        "OP_ADD", "OP_SUB", "OP_MUL", "OP_DIV", "OP_MOD",
        "OP_LSHIFT", "BINOP_RSHIFT", "BINOP_AND", "OP_XOR", "OP_OR",
        "OP_ASSIGN", "OP_EQ", "OP_NEQ", "OP_GT", "OP_LT"
    };

    size_t opsCount = sizeof(binaryOps) / sizeof(binaryOps[0]);

    for (size_t i = 0; i < opsCount; ++i) {
        if (strcmp(nodeName, binaryOps[i]) == 0) {
            return safeStrdup(binopNames[i]);
        }
    }

    return NULL;
}

bool isOperation(pANTLR3_BASE_TREE tree) {
    if (tree == NULL) return false;

    char* nodeName = (char*)tree->toString(tree)->chars;
    if (nodeName == NULL) return false;

    const char* binaryOps[] = {
        "+", "-", "*", "/", "%", "<<", ">>", "&", "^", "|",
        "=", "==", "!=", ">", "<"
    };

    for (size_t i = 0; i < sizeof(binaryOps) / sizeof(binaryOps[0]); ++i) {
        if (strcmp(nodeName, binaryOps[i]) == 0) {
            return true;
        }
    }
    return false;
}

struct parseTree *buildParseTreeForExpression(pANTLR3_BASE_TREE expr)
{
    if (!expr) return NULL;

    const char* name = expr->toString(expr)->chars;

    if (strcmp(name, "ExpressionToken") == 0 || strcmp(name, "Body") == 0) {
        if (expr->getChildCount(expr) == 1) {
            return buildParseTreeForExpression(expr->getChild(expr, 0));
        }
    }

    if (isOperation(expr)) {
        struct parseTree *opNode = calloc(1, sizeof(struct parseTree));
        char *opName = generateBinopName(expr);
        opNode->name = opName ? opName : safeStrdup("op");

        if (expr->getChildCount(expr) == 2) {
            opNode->left  = buildParseTreeForExpression(expr->getChild(expr,0));
            opNode->right = buildParseTreeForExpression(expr->getChild(expr,1));
        }
        return opNode;
    }

    struct parseTree *leaf = calloc(1, sizeof(struct parseTree));
    pANTLR3_STRING s = expr->toString(expr);
    const char *txt = (s && s->chars) ? s->chars : "";
    leaf->name = safeStrdup(txt);
    return leaf;
}


 struct cfgNode *tempParentNode;

struct cfgNode* createCfgNode(pANTLR3_BASE_TREE tree) {
    struct cfgNode *node = calloc(1, sizeof(struct cfgNode));
    if (!node) {
        fprintf(stderr,"Error: Allocation for cfgNode failed\n");
        return NULL;
    }
    node->id = nodeId++;
    node->ast = (struct astNode*)tree;
    node->isProcessed = false;
    node->isParsed = false;
    node->parseTree = NULL;

    if (isOperation(tree)) {
        node->nodeOp = generateBinopName(tree);
        tempParentNode->isParseTreeRoot = true;
        node->isParseTreeRoot=true;
    } else {
        node->isParseTreeRoot = false;
        node->nodeOp = NULL;
    }
    tempParentNode = node;

    addNodeToGlobalList(node);
    return node;
}


struct funcNode* processFunction(pANTLR3_BASE_TREE funcTree, char* name) {
    struct funcNode *f = calloc(1, sizeof(struct funcNode));
    f->identifier = safeStrdup(name? name : "unknown_func");

    f->cfgEntry = createCfgNode(funcTree);
    f->cfgEntry->name = safeStrdup(f->identifier);

    f->cfgExit = createCfgNode(NULL);
    f->cfgExit->name = safeStrdup("func_exit");

    return f;
}

static void pushBreak(struct context *ctx, struct cfgNode *breakTarget) {
    struct breakStack *bs = malloc(sizeof(struct breakStack));
    bs->breakTarget = breakTarget;
    bs->next = ctx->breakStack;
    ctx->breakStack = bs;
}
static struct cfgNode* topBreak(struct context *ctx) {
    return ctx->breakStack? ctx->breakStack->breakTarget : NULL;
}
static void popBreak(struct context *ctx) {
    if (ctx->breakStack) {
        struct breakStack* tmp = ctx->breakStack;
        ctx->breakStack = tmp->next;
        free(tmp);
    }
}

static bool isControlNode(const char* n) {
    if (!n) return true;
    if (!strcmp(n,"CondToken"))  return true;
    if (!strcmp(n,"LoopToken"))  return true;
    if (!strcmp(n,"RepeatToken")) return true;
    if (!strcmp(n,"BreakToken")) return true;
    if (!strcmp(n,"VarDeclToken")) return true;
    return false;
}

static void getExpressionString(pANTLR3_BASE_TREE tree, char* buf, size_t sz) {
    if (!tree || sz<2) return;
    pANTLR3_STRING s = tree->toString(tree);
    const char* nm = (s&&s->chars)? s->chars:"";
    unsigned cc = tree->getChildCount(tree);

    if (isControlNode(nm)) {
        for (unsigned i=0; i<cc; i++) {
            getExpressionString(tree->getChild(tree,i), buf, sz);
        }
        return;
    }

    if (cc==0) {
        if (strlen(buf)+strlen(nm)+2 < sz) {
            strcat(buf,nm);
        }
    }
    else if (cc==2 && (
             !strcmp(nm,"==")||!strcmp(nm,"!=")||
             !strcmp(nm,"+") ||!strcmp(nm,"-")  ||
             !strcmp(nm,"*") ||!strcmp(nm,"/")  ||
             !strcmp(nm,"%")))
    {
        strcat(buf,"(");
        getExpressionString(tree->getChild(tree,0), buf, sz);
        strcat(buf,nm);
        getExpressionString(tree->getChild(tree,1), buf, sz);
        strcat(buf,")");
    } else {
        for (unsigned i=0; i<cc; i++) {
            getExpressionString(tree->getChild(tree,i), buf, sz);
        }
    }
}

static void processTreeNode(pANTLR3_BASE_TREE tree, struct context *ctx);

static void processConditional(pANTLR3_BASE_TREE ifTree, struct context *ctx) {
    struct cfgNode *ifNode = createCfgNode(ifTree);

    char expr[256];
    expr[0] = '\0';
    unsigned cc = ifTree->getChildCount(ifTree);
    pANTLR3_BASE_TREE exprChild = NULL;
    pANTLR3_BASE_TREE thenBody  = NULL;
    pANTLR3_BASE_TREE elseBody  = NULL;
    if (cc > 0) exprChild = ifTree->getChild(ifTree, 0);
    if (cc > 1) thenBody  = ifTree->getChild(ifTree, 1);
    if (cc > 2) elseBody  = ifTree->getChild(ifTree, 2);
    if (exprChild) {
        getExpressionString(exprChild, expr, sizeof(expr));
    }
    if (expr[0]) {
        char tmp[300];
        snprintf(tmp, sizeof(tmp), "if ((%s))", expr);
        ifNode->name = safeStrdup(tmp);
    } else {
        ifNode->name = safeStrdup("if (?)");
    }
    if (ctx->curr) {
        ctx->curr->defaultBranch = ifNode;
    }
    ctx->curr = ifNode;

    struct cfgNode* endIf = createCfgNode(NULL);
    endIf->name = safeStrdup("endif");

    if (thenBody) {
        struct cfgNode* thenStart = createCfgNode(thenBody);
        thenStart->name = safeStrdup("thenBlock");
        ifNode->conditionalBranch = thenStart;
        ctx->curr = thenStart;
        processTreeNode(thenBody, ctx);
        if (ctx->curr && ctx->curr->defaultBranch == NULL) {
            ctx->curr->defaultBranch = endIf;
        }
    } else {
        ifNode->conditionalBranch = endIf;
    }

    if (elseBody) {
        struct cfgNode* elseStart = createCfgNode(elseBody);
        elseStart->name = safeStrdup("elseBlock");
        ifNode->defaultBranch = elseStart;
        ctx->curr = elseStart;
        processTreeNode(elseBody, ctx);
        if (ctx->curr && ctx->curr->defaultBranch==NULL) {
            ctx->curr->defaultBranch = endIf;
        }
    } else {
        if (!ifNode->defaultBranch) {
            ifNode->defaultBranch = endIf;
        }
    }
    ctx->curr = endIf;
}

struct cfgNode* findCfgNodeByAST(pANTLR3_BASE_TREE tree) {
    for (int i = 0; i < allNodesCount; i++) {
        if (allNodes[i]->ast == (struct astNode*)tree) {
            return allNodes[i];
        }
    }
    return NULL;
}

static void processLoop(pANTLR3_BASE_TREE loopTree, struct context *ctx) {
    struct cfgNode *loopNode = createCfgNode(loopTree);

    char exprBuf[256];
    exprBuf[0] = '\0';

    unsigned cc = loopTree->getChildCount(loopTree);
    pANTLR3_BASE_TREE exprChild=NULL, bodyChild=NULL;
    if (cc>0) exprChild = loopTree->getChild(loopTree,0);
    if (cc>1) bodyChild = loopTree->getChild(loopTree,1);

    if (exprChild) {
        getExpressionString(exprChild, exprBuf, sizeof(exprBuf));
        loopNode->parseTree = buildParseTreeForExpression(exprChild);
    }
    if (exprBuf[0]) {
        char tmp[300];
        snprintf(tmp,sizeof(tmp),"while ((%s))",exprBuf);
        loopNode->name = safeStrdup(tmp);
    } else {
        loopNode->name = safeStrdup("while (?)");
    }

    if (ctx->curr) {
        ctx->curr->defaultBranch = loopNode;
    }
    ctx->curr = loopNode;

    struct cfgNode *loopExit = createCfgNode(NULL);
    loopExit->name = safeStrdup("loop_exit");
    pushBreak(ctx, loopExit);

    if (bodyChild) {
        loopNode->conditionalBranch = NULL;
        struct cfgNode *bodyStart = createCfgNode(bodyChild);
        bodyStart->name = safeStrdup("while_body");
        loopNode->conditionalBranch = bodyStart;
        ctx->curr = bodyStart;
        processTreeNode(bodyChild, ctx);
        if (ctx->curr && ctx->curr->defaultBranch==NULL) {
            ctx->curr->defaultBranch = loopNode;
        }
    }
    loopNode->defaultBranch = loopExit;
    ctx->curr = loopExit;

    popBreak(ctx);
}

static void processRepeat(pANTLR3_BASE_TREE repeatTree, struct context *ctx)
{
    struct cfgNode *repeatNode = createCfgNode(repeatTree);

    char exprBuf[256];
    exprBuf[0] = '\0';
    unsigned cc = repeatTree->getChildCount(repeatTree);
    pANTLR3_BASE_TREE bodyChild = NULL;
    pANTLR3_BASE_TREE exprChild = NULL;

    if (cc > 0) {
        bodyChild = repeatTree->getChild(repeatTree, 0);
    }
    if (cc > 1) {
        exprChild = repeatTree->getChild(repeatTree, 1);
    }
    if (exprChild) {
        getExpressionString(exprChild, exprBuf, sizeof(exprBuf));
    }
    if (exprBuf[0]) {
        char tmp[300];
        snprintf(tmp, sizeof(tmp), "repeat { ... } until ((%s))", exprBuf);
        repeatNode->name = safeStrdup(tmp);
    } else {
        repeatNode->name = safeStrdup("repeat { ... } until (?)");
    }
    if (ctx->curr) {
        ctx->curr->defaultBranch = repeatNode;
    }
    ctx->curr = repeatNode;
    struct cfgNode *loopExit = createCfgNode(NULL);
    loopExit->name = safeStrdup("repeat_until_exit");
    pushBreak(ctx, loopExit);
    if (bodyChild) {
        repeatNode->conditionalBranch = NULL;
        struct cfgNode *bodyStart = createCfgNode(bodyChild);
        bodyStart->name = safeStrdup("repeat_body");
        repeatNode->defaultBranch = bodyStart;
        ctx->curr = bodyStart;
        processTreeNode(bodyChild, ctx);
        if (ctx->curr && ctx->curr->defaultBranch == NULL) {
            ctx->curr->defaultBranch = repeatNode;
        }
    }
    repeatNode->conditionalBranch = loopExit;
    ctx->curr = loopExit;
    popBreak(ctx);
}


static void processBreak(pANTLR3_BASE_TREE bkTree, struct context *ctx) {
    struct cfgNode *bkNode = createCfgNode(bkTree);
    bkNode->name = safeStrdup("break");

    struct cfgNode* exitNode = topBreak(ctx);
    if (exitNode) {
        bkNode->defaultBranch = exitNode;
    } else {
        addError(&ctx->errors, "Error: break outside of any loop");
    }
    ctx->curr = bkNode;
}


static void processReturn(pANTLR3_BASE_TREE retTree, struct context *ctx) {
    struct cfgNode* r = createCfgNode(retTree);
    char expr[256]; expr[0]='\0';

    if (retTree->getChildCount(retTree)>0) {
        getExpressionString(retTree->getChild(retTree,0), expr,sizeof(expr));
    }
    if (expr[0]) {
        char tmp[300];
        snprintf(tmp,sizeof(tmp),"return (%s)",expr);
        r->name = safeStrdup(tmp);
    } else {
        r->name = safeStrdup("return");
    }
    if (ctx->curr) {
        ctx->curr->defaultBranch = r;
    }
    ctx->curr = r;
}

/*
static bool isTrivialNode(const char* n) {
    if (!n) return true;
    if (!strcmp(n,"Identifier"))    return true;
    if (!strcmp(n,"Literal"))       return true;
    if (!strcmp(n,"Builtin"))       return true;
    if (!strcmp(n,"TypeRefToken"))  return true;
    if (!strcmp(n,"ArrayToken"))    return true;
    if (!strcmp(n,"ArrayTokenSuffix")) return true;
    if (!strcmp(n,"FuncSignatureToken")) return true;
    if (!strcmp(n,"ArgListToken"))  return true;
    if (!strcmp(n,"Body"))         return true;
    return false;
}
*/


struct parseTree *buildAssignParseTree(pANTLR3_BASE_TREE varDecl)
{
    if (!varDecl) return NULL;
    unsigned cc = varDecl->getChildCount(varDecl);
    if (cc < 2) return NULL;

    pANTLR3_BASE_TREE varChild = varDecl->getChild(varDecl, 0);
    pANTLR3_BASE_TREE exprChild = varDecl->getChild(varDecl, 1);

    struct parseTree *root = calloc(1, sizeof(struct parseTree));
    root->name = safeStrdup("OP_ASSIGN");

    struct parseTree *opPlace = calloc(1, sizeof(struct parseTree));
    opPlace->name = safeStrdup("OP_PLACE");
    root->left = opPlace;

    struct parseTree *varNode = calloc(1, sizeof(struct parseTree));
    pANTLR3_STRING vs = varChild->toString(varChild);
    const char *varName = (vs && vs->chars) ? vs->chars : "unknownVar";
    varNode->name = safeStrdup(varName);
    opPlace->left = varNode;

    root->right = buildParseTreeForExpression(exprChild);

    return root;
}


static void processGenericNode(pANTLR3_BASE_TREE tree, struct context *ctx)
{
    if (!tree) return;

    pANTLR3_STRING s = tree->toString(tree);
    const char *nm = (s && s->chars) ? s->chars : "";

    if (!strcmp(nm, "VarDeclToken"))
    {
        unsigned cc = tree->getChildCount(tree);
        if (cc < 2)
        {
            return;
        }

        struct cfgNode *assignNode = calloc(1, sizeof(struct cfgNode));
        assignNode->id = nodeId++;
        assignNode->name = safeStrdup("OP_ASSIGN");
        assignNode->nodeOp = safeStrdup("OP_ASSIGN");
        assignNode->isParseTreeRoot = true;
        assignNode->parseTree = buildAssignParseTree(tree);
        addNodeToGlobalList(assignNode);

        if (ctx->curr) {
            ctx->curr->defaultBranch = assignNode;
        }
        ctx->curr = assignNode;
        return;
    }

    struct cfgNode *g = createCfgNode(tree);
    if (!g->name) {
        g->name = safeStrdup(nm);
    }

    if (ctx->curr) {
        ctx->curr->defaultBranch = g;
    }
    ctx->curr = g;

    unsigned childCount = tree->getChildCount(tree);
    for (unsigned i = 0; i < childCount; i++) {
        pANTLR3_BASE_TREE ch = tree->getChild(tree,i);
        processTreeNode(ch, ctx);
    }
}

static void processTreeNode(pANTLR3_BASE_TREE tree, struct context *ctx) {
    if (!tree) return;

    pANTLR3_STRING s = tree->toString(tree);
    const char* nm = (s && s->chars) ? s->chars : "";

    if (!strcmp(nm, "CondToken")) {
        processConditional(tree, ctx);
    }
    else if (!strcmp(nm, "LoopToken")) {
        processLoop(tree, ctx);
    }
    else if (!strcmp(nm, "RepeatToken")) {
        processRepeat(tree, ctx);
    }
    else if (!strcmp(nm, "BreakToken")) {
        processBreak(tree, ctx);
    }
    else if (!strcmp(nm, "ReturnToken")) {
        processReturn(tree, ctx);
    }
    else {
        processGenericNode(tree, ctx);
    }
}

static void resetTraversal(struct cfgNode *node) {
    if (!node) return;
    if (node->isTraversed) return;
    node->isTraversed=true;

    if (node->conditionalBranch && !isNodeInList(node->conditionalBranch)) {
        node->conditionalBranch=NULL;
    }
    if (node->defaultBranch && !isNodeInList(node->defaultBranch)) {
        node->defaultBranch=NULL;
    }
    if (node->conditionalBranch) resetTraversal(node->conditionalBranch);
    if (node->defaultBranch) resetTraversal(node->defaultBranch);

    node->isTraversed=false;
}

static void printParseSubtree(FILE *f, int parseParentId, struct parseTree *pt) {
    if (!pt) return;
    int myId = parseTreeIdCounter++;
    fprintf(f, "    PT%d [label=\"%s\", shape=ellipse];\n", myId, pt->name);
    fprintf(f, "    PT%d -> PT%d;\n", parseParentId, myId);

    if (pt->left)  printParseSubtree(f, myId, pt->left);
    if (pt->right) printParseSubtree(f, myId, pt->right);
}


static void printOperationSubtree(FILE *f, struct cfgNode *node) {
    if (!node->parseTree) return;

    int parseRootId = parseTreeIdCounter++;
    fprintf(f, "    PT%d [label=\"%s\", shape=ellipse, style=dashed];\n",
            parseRootId, node->parseTree->name ? node->parseTree->name : "op_root");
    fprintf(f, "    Node%d -> PT%d [label=\"ops\"];\n", node->id, parseRootId);

    if (node->parseTree->left) {
        printParseSubtree(f, parseRootId, node->parseTree->left);
    }
    if (node->parseTree->right) {
        printParseSubtree(f, parseRootId, node->parseTree->right);
    }
}


void sanitizeString(char *str, char value) {
    int i = 0, j = 0;

    while (str[i] != '\0') {
        if (str[i] != value) {
            str[j++] = str[i];
        }
        i++;
    }

    str[j] = '\0';
}

static void printCFGNode(FILE *f, struct cfgNode *node) {
    if (!node || node->isTraversed) return;
    node->isTraversed=true;
    sanitizeString(node->name, '\"');
    fprintf(f,"    Node%d [label=\"%s\"];\n", node->id, node->name?node->name:"");
    printOperationSubtree(f, node);

    if (node->conditionalBranch && !isNodeInList(node->conditionalBranch)) {
        node->conditionalBranch=NULL;
    }
    if (node->defaultBranch && !isNodeInList(node->defaultBranch)) {
        node->defaultBranch=NULL;
    }

    bool isIf=false,isWhile=false;
    if (node->name) {
        if (!strncmp(node->name,"if ((",4))     isIf=true;
        else if (!strncmp(node->name,"while ((",6)) isWhile=true;
    }

    if (node->conditionalBranch) {
        const char* lab = isIf? "then": (isWhile? "true":"");
        fprintf(f,"    Node%d -> Node%d [label=\"%s\"];\n",
                node->id,node->conditionalBranch->id,lab);
        printCFGNode(f,node->conditionalBranch);
    }

    if (node->defaultBranch) {
        const char* lab = isIf? "else": (isWhile? "false":"");
        fprintf(f,"    Node%d -> Node%d [label=\"%s\"];\n",
                node->id,node->defaultBranch->id,lab);
        printCFGNode(f,node->defaultBranch);
    }
}

static void drawCFG(struct programGraph *graph) {
    if (!graph) return;

    for (int i=0; i<graph->funcCount; i++) {
        struct funcNode *fn = graph->functions[i];
        if (!fn || !fn->cfgEntry) continue;
        char fname[256];
        snprintf(fname,sizeof(fname),"%s.dot", fn->identifier);
        FILE *fp = fopen(fname,"w");
        if (!fp) {
            fprintf(stderr,"Error: cannot open %s\n",fname);
            continue;
        }
        resetTraversal(fn->cfgEntry);

        fprintf(fp,"digraph CFG {\n");
        fprintf(fp,"    node [shape=box];\n");
        printCFGNode(fp, fn->cfgEntry);

        fprintf(fp,"}\n");
        fclose(fp);
        char cmd[512];
        snprintf(cmd,sizeof(cmd),"dot -Tpng %s -o %s.png", fname, fn->identifier);
        system(cmd);
    }
}

void collectNodes(struct cfgNode *node, struct cfgNode **list, bool *used, int *count) {
    if (!node) return;
    if (used[node->id]) return;
    used[node->id] = true;
    list[*count] = node;
    (*count)++;
    collectNodes(node->conditionalBranch, list, used, count);
    collectNodes(node->defaultBranch, list, used, count);
}

static struct parseTree* buildExpression(struct cfgNode **arr, int *pos, int cnt)
{
    if (*pos >= cnt) {
        return NULL;
    }

    struct cfgNode *nd = arr[*pos];
    (*pos)++;

    if (nd->isParseTreeRoot)
    {
        if (!nd->parseTree) {
            nd->parseTree = calloc(1, sizeof(struct parseTree));
            nd->parseTree->name = nd->nodeOp ? safeStrdup(nd->nodeOp) : safeStrdup("op");
        }

        nd->parseTree->left  = buildExpression(arr, pos, cnt);
        nd->parseTree->right = buildExpression(arr, pos, cnt);
        return nd->parseTree;
    }
    else
    {
        struct parseTree* leaf = calloc(1, sizeof(struct parseTree));
        leaf->name = nd->name ? safeStrdup(nd->name) : safeStrdup("operand");
        return leaf;
    }
}

static void buildOpTreesForFunc(struct funcNode *fn)
{
    if (!fn || !fn->cfgEntry)
        return;

    struct cfgNode *arr[4096];
    bool used[65536];
    memset(used, 0, sizeof(used));
    int cnt = 0;
    collectNodes(fn->cfgEntry, arr, used, &cnt);
    bool foundOp = false;
    for (int j = 0; j < cnt; j++) {
        if (arr[j]->isParseTreeRoot) {
            foundOp = true;
            break;
        }
    }
    if (!foundOp && cnt > 0) {
        arr[0]->isParseTreeRoot = true;
        if (!arr[0]->nodeOp) {
            arr[0]->nodeOp = safeStrdup("COMPLEX_EXPRESSION");
            free(arr[0]->name);
            arr[0]->name   = safeStrdup("COMPLEX_EXPRESSION");
        }
    }

    int i = 0;
    while (i < cnt)
    {
        struct cfgNode *nd = arr[i];
        if (nd->isParseTreeRoot && nd->parseTree == NULL)
        {
            buildExpression(arr, &i, cnt);
        }
        else
        {
            i++;
        }
    }
}


void buildOperationTrees(struct programGraph *graph) {
    if (!graph) return;
    for (int i = 0; i < graph->funcCount; i++) {
        buildOpTreesForFunc(graph->functions[i]);
    }
}

struct cfgNode *currentParseNode;
int pulse = 0;

void traversePartOfGraph() {
    struct cfgNode *tempNode;
    tempNode = currentParseNode->defaultBranch;

    if(isOperation(currentParseNode->defaultBranch->ast)) {
        tempNode = tempNode->defaultBranch;
    }
    currentParseNode->defaultBranch = tempNode;
}


void removeExtraTokens(struct funcNode *fn) {
    if (!fn || !fn->cfgEntry)
        return;

    struct cfgNode *arr[4096];
    bool used[65536];
    memset(used, 0, sizeof(used));
    int cnt = 0;
    collectNodes(fn->cfgEntry, arr, used, &cnt);
    int i = 0;
    while (i < cnt)
    {
        struct cfgNode *nd = arr[i];

        if(!isOperation(nd->ast)) {
            currentParseNode = nd;
        }
        if (strcmp(nd->defaultBranch->name, "func_exit") == 0) {
            return;
        }
            traversePartOfGraph();
            i++;
    }
}

void clearGraph(struct programGraph *graph) {
    if (!graph) return;
    for (int i = 0; i < graph->funcCount; i++) {
        removeExtraTokens(graph->functions[i]);
    }
}

static struct parseTree* beautifyTreeFromRoot(struct parseTree *node, char* variableName)
{
    if (!node) return NULL;

    if (node->left != NULL || node->right != NULL) {
        return node;
    }
    struct parseTree* op = calloc(1, sizeof(struct parseTree));
    struct parseTree* vn = calloc(1, sizeof(struct parseTree));

    op->name = safeStrdup("OP_PLACE");
    op->left = vn;
    vn->name = safeStrdup(variableName);
    node->left = op;
    return node;
}

void postProcessGraphTreesFunc(struct funcNode *fn) {
    if (!fn || !fn->cfgEntry)
        return;

    struct cfgNode *arr[4096];
    bool used[65536];
    memset(used, 0, sizeof(used));
    int cnt = 0;
    collectNodes(fn->cfgEntry, arr, used, &cnt);
    int i = 0;
    while (i < cnt)
    {
        struct cfgNode *nd = arr[i];

        if (nd->isParseTreeRoot && nd->parseTree) {
            if (strcmp(nd->parseTree->name, "OP_ASSIGN") != 0) {
                nd->parseTree = beautifyTreeFromRoot(nd->parseTree, nd->name);
            }
            nd->name = safeStrdup("OP_ASSIGN");
        }

        i++;
    }
}

char* getLastParseTreeElement(struct parseTree *pt)
{
    if (!pt) return NULL;
    while (pt->right) {
        pt = pt->right;
    }
    while (pt->left) {
        pt = pt->left;
        pulse++;
    }
    return pt->name;
}


void deleteExtraNodes(struct funcNode *fn)
{
    if (!fn || !fn->cfgEntry)
        return;

    struct cfgNode *arr[4096];
    bool used[65536];
    memset(used, 0, sizeof(used));
    int cnt = 0;
    collectNodes(fn->cfgEntry, arr, used, &cnt);
    currentParseNode = NULL;
    char* lastEle = NULL;

    for (int i = 0; i < cnt; i++)
    {
        struct cfgNode *nd = arr[i];
        if (pulse != 0 ) {
            pulse--;
        }
        if (!nd)
            continue;
        if (nd->isParseTreeRoot) {
            lastEle = getLastParseTreeElement(nd->parseTree);
            currentParseNode = nd;
        } else if (lastEle != NULL && strcmp(nd->name, lastEle) == 0 && pulse == 0) {
            currentParseNode -> defaultBranch = nd->defaultBranch;
            currentParseNode = NULL;
            lastEle = NULL;
        }

    }
}


void postProcessGraphTrees(struct programGraph *graph) {
    if (!graph) return;
    for (int i = 0; i < graph->funcCount; i++) {
        postProcessGraphTreesFunc(graph->functions[i]);
    }
    for (int i = 0; i < graph->funcCount; i++) {
        deleteExtraNodes(graph->functions[i]);
    }
}

struct programGraph* processTree(pANTLR3_BASE_TREE tree) {
    struct programGraph *graph = calloc(1, sizeof(struct programGraph));
    unsigned topCount = tree->getChildCount(tree);

    static unsigned funcCounter = 0;

    for (unsigned i = 0; i < topCount; i++) {
        pANTLR3_BASE_TREE child = tree->getChild(tree, i);
        pANTLR3_STRING s = child->toString(child);
        const char *nm = (s && s->chars) ? s->chars : "";
        if (!strcmp(nm, "FuncDefToken")) {
            char funcName[32];
            snprintf(funcName, sizeof(funcName), "func_%u", funcCounter++);

            struct funcNode *func = processFunction(child, funcName);
            graph->functions = realloc(graph->functions,
                sizeof(struct funcNode *) * (graph->funcCount + 1));
            graph->functions[graph->funcCount++] = func;

            struct context ctx;
            memset(&ctx, 0, sizeof(ctx));
            ctx.curr = func->cfgEntry;
            ctx.entryNode = func->cfgEntry;
            ctx.exitNode = func->cfgExit;
            ctx.loopDepth = 0;
            ctx.breakStack = NULL;
            ctx.function = func;

            unsigned c2 = child->getChildCount(child);
            for (unsigned j = 0; j < c2; j++) {
                pANTLR3_BASE_TREE st = child->getChild(child, j);
                processTreeNode(st, &ctx);
            }
            printErrors(ctx.errors);
            freeErrors(&ctx.errors);
            if (ctx.curr && ctx.curr != func->cfgExit && ctx.curr->defaultBranch == NULL) {
                ctx.curr->defaultBranch = func->cfgExit;
            }
        }
    }

    buildOperationTrees(graph);
    clearGraph(graph);
    postProcessGraphTrees(graph);
    drawCFG(graph);
    return graph;
}