#include <string.h>
#include <stdbool.h>

bool isInterestingNode(char* nodeName) {
    if (strcmp(nodeName, "FuncSignatureToken") == 0) {
        return false;
    }
    if (strcmp(nodeName, "Identifier") == 0) {
        return false;
    }
    if (strcmp(nodeName, "ArgListToken") == 0) {
        return false;
    }
    if (strcmp(nodeName, "Body") == 0) {
        return false;
    }
    return true;
}