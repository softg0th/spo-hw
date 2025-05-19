#include <stdio.h>
#include <stdlib.h>

void handleError(int errorCode, char* token) {
    switch (errorCode) {
        case 1:
            printf("An error occured! IR owerflow");
        case 2:
            printf("An error occured! Malformed operation: %s with missing operand(s)\n", token);
        case 3:
            printf("Failed to allocate IRInstruction");
        default:
            printf("An error occured! Unknown error");
        exit(0);
    }
}