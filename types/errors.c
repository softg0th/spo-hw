#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"

void addTypeError(struct typeError **elist, char* value, char* expectedType) {
    if (!elist) return;
    struct typeError *node = malloc(sizeof(struct typeError));
    if (!node) {
        return;
    }

    node->value = strdup(value);
    node->expectedType = strdup(expectedType);
    node->next = NULL;

    if (*elist == NULL) {
        *elist = node;
    } else {
        struct typeError *cur = *elist;
        while (cur->next) {
            cur = cur->next;
        }
        cur->next = node;
    }
}

void freeErrors(struct typeError *elist) {
    while (elist) {
        struct typeError *temp = elist;
        elist = elist->next;
        free(temp->value);
        free(temp->expectedType);
        free(temp);
    }
}


void printErrors(struct typeError *elist) {
    while (elist) {
        printf("An error occured! %s is not an %s\n", elist->value, elist->expectedType);
        elist = elist->next;
    }
    freeErrors(elist);
}

