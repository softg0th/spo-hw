#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

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

bool isCond(const char* node) {
    if (!node) return false;

    while (isspace(*node)) node++;

    if (node[0] != 'i' || node[1] != 'f') return false;
    node += 2;

    while (isspace(*node)) node++;

    return *node == '(';
}

char* extractCond(const char* src) {
    const char* start = src;
    const char* end = src + strlen(src) - 1;

    // Удаляем "if" и пробелы
    while (*start && !(*start == '(')) start++;
    if (*start != '(') return NULL;

    // Сдвиг на первую (
    start++;

    // Сдвиг к последней )
    while (end > start && *end != ')') end--;
    if (end <= start) return NULL;

    // Пропускаем внешние скобки ещё раз (если их много)
    while (*start == '(' && *(end - 1) == ')') {
        start++;
        end--;
    }

    size_t len = end - start;
    char* result = malloc(len + 1);
    strncpy(result, start, len);
    result[len] = '\0';

    return result;
}


void trim(char* str) {
    if (!str) return;
    char* end;

    while (isspace(*str)) str++;
    if (*str == 0) return;

    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;

    *(end + 1) = '\0';
}

void splitCondExpr(const char* expr, char* lhs, char* op, char* rhs) {
    const char* p = expr;
    char lhs_buf[64] = {0}, rhs_buf[64] = {0};

    char* lhs_ptr = lhs_buf;
    while (*p && !strchr("=!<>", *p)) *lhs_ptr++ = *p++;
    *lhs_ptr = '\0';

    if (*p == '=' && *(p+1) == '=') {
        strcpy(op, "==");
        p += 2;
    } else if (*p == '!' && *(p+1) == '=') {
        strcpy(op, "!=");
        p += 2;
    } else if (*p == '>') {
        strcpy(op, ">");
        p++;
    } else if (*p == '<') {
        strcpy(op, "<");
        p++;
    } else {
        strcpy(op, "?");
    }

    strcpy(rhs_buf, p);

    trim(lhs_buf);
    trim(rhs_buf);
    strcpy(lhs, lhs_buf);
    strcpy(rhs, rhs_buf);
}


