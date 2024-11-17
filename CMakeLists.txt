cmake_minimum_required(VERSION 3.15)
project(spo C)

set(CMAKE_C_STANDARD 11)
# add_executable(spo main.c Variant3Lexer.c Variant3Parser.c MyLangLexer.c MyLangParser.c)

add_executable(spo main.c lang/MyLangLexer.c lang/MyLangParser.c
        ast/ast.c
        graph/graph.c
        graph/graphStructures.h)
target_link_libraries(spo antlr3c)