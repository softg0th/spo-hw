#include <string.h>

#include "../lang/MyLangLexer.h"
#include "../lang/MyLangParser.h"
#include <antlr3treeparser.h>
#include <antlr3.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

void makeTree(char *content, char *filename);