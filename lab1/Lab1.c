#include <stdio.h>
#include "AwesomeLangLexer.h"
#include "AwesomeLangParser.h"

int main(int argc, char* argv[]) {
    pANTLR3_INPUT_STREAM input;
    pAwesomeLangLexer lexer;
    pANTLR3_COMMON_TOKEN_STREAM tokens;
    pAwesomeLangParser parser;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    input = antlr3FileStreamNew((pANTLR3_UINT8)argv[1], ANTLR3_ENC_8BIT);
    if (input == NULL) {
        fprintf(stderr, "Failed to open file %s\n", argv[1]);
        return 1;
    }

    lexer = AwesomeLangLexerNew(input);
    if (lexer == NULL) {
        fprintf(stderr, "Unable to create lexer\n");
        input->close(input);
        return 1;
    }

    tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));
    if (tokens == NULL) {
        fprintf(stderr, "Out of memory trying to allocate token stream\n");
        lexer->free(lexer);
        input->close(input);
        return 1;
    }

    parser = AwesomeLangParserNew(tokens);
    if (parser == NULL) {
        fprintf(stderr, "Unable to create parser\n");
        tokens->free(tokens);
        lexer->free(lexer);
        input->close(input);
        return 1;
    }

    parser->free(parser);
    tokens->free(tokens);
    lexer->free(lexer);
    input->close(input);

    return 0;
}
