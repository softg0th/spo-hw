cmake_minimum_required(VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
endif()

project("Lab1" C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)

set(ANTLR_RUNTIME_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/runtime/C)

include_directories(${ANTLR_RUNTIME_PATH}/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/lang)  

if (MSVC)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<CONFIG:Debug>,EditAndContinue,ProgramDatabase>")
endif()

set(GENERATED_FILES
    ${CMAKE_SOURCE_DIR}/lib/lang/AwesomeLangLexer.c
    ${CMAKE_SOURCE_DIR}/lib/lang/AwesomeLangLexer.h
    ${CMAKE_SOURCE_DIR}/lib/lang/AwesomeLangParser.c
    ${CMAKE_SOURCE_DIR}/lib/lang/AwesomeLangParser.h
)

add_custom_command(
    OUTPUT ${GENERATED_FILES}
    COMMAND java -jar ${CMAKE_CURRENT_SOURCE_DIR}/antlr/antlr-3.5.3-complete.jar -language C -o ${CMAKE_SOURCE_DIR}/lib/lang
    COMMENT "Running ANTLR to generate lexer and parser"
)

add_library(antlr_runtime STATIC IMPORTED)
set_target_properties(antlr_runtime PROPERTIES
    IMPORTED_LOCATION ${ANTLR_RUNTIME_PATH}/lib/libantlr3c.a
)

set(SOURCE_FILES
    ./Lab1.c
    ${GENERATED_FILES}
)

add_executable (Lab1 ${SOURCE_FILES})

target_link_libraries(Lab1 PRIVATE antlr_runtime)
