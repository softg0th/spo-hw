#include <stdbool.h>

bool isInterestingNode(char* nodeName);
bool isCond(char* node);
char* extractToken(const char* src);
void splitCondExpr(const char* expr, char* lhs, char* op, char* rhs);
bool isWhile(const char* node);