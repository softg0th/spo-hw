#include <antlr3.h>
#include "graphStructures.h"

struct programGraph processTree(pANTLR3_BASE_TREE tree);
static void collectNodes(struct cfgNode *node, struct cfgNode **list, bool *used, int *count);