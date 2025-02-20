#include <stdbool.h>
#include <ctype.h>

bool isInteger(char *str) {
    if (*str == '\0') return false;

    while (*str) {
        if (!isdigit(*str)) return false;
        str++;
    }
    return true;
}

char* detectType(char *value) {
    if (isInteger(value)) {
        return "integer";
    }
    return "unknown";
}
