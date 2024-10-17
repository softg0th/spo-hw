cmake_minimum_required(VERSION 3.16)
project(spo C)

set(CMAKE_C_STANDARD 17)
# ффф
#add_executable(spo main.c Variant3Lexer.c Variant3Parser.c MyLangLexer.c MyLangParser.c)

add_executable(spo main.c MyLangLexer.c MyLangParser.c)
target_link_libraries(spo antlr3c)