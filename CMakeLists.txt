cmake_minimum_required(VERSION 3.15)
project(spo C)
include_directories(/opt/homebrew/Cellar/libantlr3c/3.5.3/include/)
link_directories(/opt/homebrew/Cellar/libantlr3c/3.5.3/lib)

set(CMAKE_C_STANDARD 11)

add_executable(spo main.c lang/MyLangLexer.c lang/MyLangParser.c
        ast/ast.c
        graph/graph.c
        graph/graphStructures.h
        types/typization.c
        types/typization.h
        types/utils.c
        graph/graphForExternal.h
        types/utils.h
        types/errors.c
        compiler/compiler.c
        compiler/compiler.h)
target_link_libraries(spo antlr3c)