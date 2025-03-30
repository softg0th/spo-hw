#include <stdio.h>
#include <stdlib.h>

void handleError(int errorCode) {
    switch (errorCode) {
        case 1:
            printf("An error occured! IR owerflow");
        default:
            printf("An error occured! Unknown error");
        exit(0);
    }
}