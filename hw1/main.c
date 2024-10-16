#include <string.h>

#include "MyLangLexer.h"
#include "MyLangParser.h"
#include <antlr3treeparser.h>
#include <antlr3.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>



void printTree(pANTLR3_BASE_TREE tree, int depth) {
    if (tree == NULL) {
        return;
    }

    for (int i = 0; i < depth; ++i) {
        printf("   ");
    }

    printf("%s\n", (char*)tree->toString(tree)->chars);

    unsigned int childCount = tree->getChildCount(tree);
    for (unsigned int i = 0; i < childCount; ++i) {
        printTree((pANTLR3_BASE_TREE)tree->getChild(tree, i), depth + 1);
    }
}

int main(void)
{
    const char* filename = "input.txt";
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Ошибка при открытии файла");
        return 1;
    }
    size_t buffer_size = 1024;
    char *content = malloc(buffer_size);
    if (content == NULL) {
        perror("Ошибка при выделении памяти");
        fclose(file);
        return 1;
    }
    content[0] = '\0';

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strlen(content) + strlen(line) + 1 > buffer_size) {
            buffer_size *= 2;
            char *new_content = realloc(content, buffer_size);
            if (new_content == NULL) {
                perror("Ошибка при перераспределении памяти");
                free(content);
                fclose(file);
                return 1;
            }
            content = new_content;
        }
        strcat(content, line);
    }

    const pANTLR3_UINT8 input_string = (pANTLR3_UINT8) content;

    const pANTLR3_INPUT_STREAM input = antlr3StringStreamNew(input_string, ANTLR3_ENC_8BIT, strlen(input_string),(pANTLR3_UINT8)"SPOparser");
    const pMyLangLexer lex = MyLangLexerNew(input);
    const pANTLR3_COMMON_TOKEN_STREAM tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
    const pMyLangParser parser = MyLangParserNew(tokens);
    const MyLangParser_source_return r = parser->source(parser);
    if (parser->pParser->rec->state->errorCount > 0) {
        parser->free(parser);
        tokens->free(tokens);
        lex->free(lex);
        input->close(input);
        fclose(file);
        return 1;
    }
    pANTLR3_BASE_TREE ast = r.tree;

    printTree(ast, 0);

    parser->free(parser);
    tokens->free(tokens);
    lex->free(lex);
    input->close(input);
    fclose(file);
    return 0;
}