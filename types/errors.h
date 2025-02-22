struct typeError {
    char* value;
    char* expectedType;
    struct typeError *next;
};

void addTypeError(struct typeError **elist, char* value, char* expectedType);
void printErrors(struct typeError *elist);