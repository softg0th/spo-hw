# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/antlr

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/antlr/cmake-build-debug-wsl

# Include any dependencies generated for this target.
include CMakeFiles/spo.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/spo.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/spo.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/spo.dir/flags.make

CMakeFiles/spo.dir/main.c.o: CMakeFiles/spo.dir/flags.make
CMakeFiles/spo.dir/main.c.o: ../main.c
CMakeFiles/spo.dir/main.c.o: CMakeFiles/spo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/antlr/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/spo.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/spo.dir/main.c.o -MF CMakeFiles/spo.dir/main.c.o.d -o CMakeFiles/spo.dir/main.c.o -c /home/antlr/main.c

CMakeFiles/spo.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/spo.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/antlr/main.c > CMakeFiles/spo.dir/main.c.i

CMakeFiles/spo.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/spo.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/antlr/main.c -o CMakeFiles/spo.dir/main.c.s

CMakeFiles/spo.dir/lang/MyLangLexer.c.o: CMakeFiles/spo.dir/flags.make
CMakeFiles/spo.dir/lang/MyLangLexer.c.o: ../lang/MyLangLexer.c
CMakeFiles/spo.dir/lang/MyLangLexer.c.o: CMakeFiles/spo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/antlr/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/spo.dir/lang/MyLangLexer.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/spo.dir/lang/MyLangLexer.c.o -MF CMakeFiles/spo.dir/lang/MyLangLexer.c.o.d -o CMakeFiles/spo.dir/lang/MyLangLexer.c.o -c /home/antlr/lang/MyLangLexer.c

CMakeFiles/spo.dir/lang/MyLangLexer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/spo.dir/lang/MyLangLexer.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/antlr/lang/MyLangLexer.c > CMakeFiles/spo.dir/lang/MyLangLexer.c.i

CMakeFiles/spo.dir/lang/MyLangLexer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/spo.dir/lang/MyLangLexer.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/antlr/lang/MyLangLexer.c -o CMakeFiles/spo.dir/lang/MyLangLexer.c.s

CMakeFiles/spo.dir/lang/MyLangParser.c.o: CMakeFiles/spo.dir/flags.make
CMakeFiles/spo.dir/lang/MyLangParser.c.o: ../lang/MyLangParser.c
CMakeFiles/spo.dir/lang/MyLangParser.c.o: CMakeFiles/spo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/antlr/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/spo.dir/lang/MyLangParser.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/spo.dir/lang/MyLangParser.c.o -MF CMakeFiles/spo.dir/lang/MyLangParser.c.o.d -o CMakeFiles/spo.dir/lang/MyLangParser.c.o -c /home/antlr/lang/MyLangParser.c

CMakeFiles/spo.dir/lang/MyLangParser.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/spo.dir/lang/MyLangParser.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/antlr/lang/MyLangParser.c > CMakeFiles/spo.dir/lang/MyLangParser.c.i

CMakeFiles/spo.dir/lang/MyLangParser.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/spo.dir/lang/MyLangParser.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/antlr/lang/MyLangParser.c -o CMakeFiles/spo.dir/lang/MyLangParser.c.s

CMakeFiles/spo.dir/ast/ast.c.o: CMakeFiles/spo.dir/flags.make
CMakeFiles/spo.dir/ast/ast.c.o: ../ast/ast.c
CMakeFiles/spo.dir/ast/ast.c.o: CMakeFiles/spo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/antlr/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/spo.dir/ast/ast.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/spo.dir/ast/ast.c.o -MF CMakeFiles/spo.dir/ast/ast.c.o.d -o CMakeFiles/spo.dir/ast/ast.c.o -c /home/antlr/ast/ast.c

CMakeFiles/spo.dir/ast/ast.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/spo.dir/ast/ast.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/antlr/ast/ast.c > CMakeFiles/spo.dir/ast/ast.c.i

CMakeFiles/spo.dir/ast/ast.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/spo.dir/ast/ast.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/antlr/ast/ast.c -o CMakeFiles/spo.dir/ast/ast.c.s

CMakeFiles/spo.dir/graph/graph.c.o: CMakeFiles/spo.dir/flags.make
CMakeFiles/spo.dir/graph/graph.c.o: ../graph/graph.c
CMakeFiles/spo.dir/graph/graph.c.o: CMakeFiles/spo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/antlr/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/spo.dir/graph/graph.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/spo.dir/graph/graph.c.o -MF CMakeFiles/spo.dir/graph/graph.c.o.d -o CMakeFiles/spo.dir/graph/graph.c.o -c /home/antlr/graph/graph.c

CMakeFiles/spo.dir/graph/graph.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/spo.dir/graph/graph.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/antlr/graph/graph.c > CMakeFiles/spo.dir/graph/graph.c.i

CMakeFiles/spo.dir/graph/graph.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/spo.dir/graph/graph.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/antlr/graph/graph.c -o CMakeFiles/spo.dir/graph/graph.c.s

CMakeFiles/spo.dir/types/typization.c.o: CMakeFiles/spo.dir/flags.make
CMakeFiles/spo.dir/types/typization.c.o: ../types/typization.c
CMakeFiles/spo.dir/types/typization.c.o: CMakeFiles/spo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/antlr/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/spo.dir/types/typization.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/spo.dir/types/typization.c.o -MF CMakeFiles/spo.dir/types/typization.c.o.d -o CMakeFiles/spo.dir/types/typization.c.o -c /home/antlr/types/typization.c

CMakeFiles/spo.dir/types/typization.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/spo.dir/types/typization.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/antlr/types/typization.c > CMakeFiles/spo.dir/types/typization.c.i

CMakeFiles/spo.dir/types/typization.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/spo.dir/types/typization.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/antlr/types/typization.c -o CMakeFiles/spo.dir/types/typization.c.s

CMakeFiles/spo.dir/types/utils.c.o: CMakeFiles/spo.dir/flags.make
CMakeFiles/spo.dir/types/utils.c.o: ../types/utils.c
CMakeFiles/spo.dir/types/utils.c.o: CMakeFiles/spo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/antlr/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/spo.dir/types/utils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/spo.dir/types/utils.c.o -MF CMakeFiles/spo.dir/types/utils.c.o.d -o CMakeFiles/spo.dir/types/utils.c.o -c /home/antlr/types/utils.c

CMakeFiles/spo.dir/types/utils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/spo.dir/types/utils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/antlr/types/utils.c > CMakeFiles/spo.dir/types/utils.c.i

CMakeFiles/spo.dir/types/utils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/spo.dir/types/utils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/antlr/types/utils.c -o CMakeFiles/spo.dir/types/utils.c.s

# Object files for target spo
spo_OBJECTS = \
"CMakeFiles/spo.dir/main.c.o" \
"CMakeFiles/spo.dir/lang/MyLangLexer.c.o" \
"CMakeFiles/spo.dir/lang/MyLangParser.c.o" \
"CMakeFiles/spo.dir/ast/ast.c.o" \
"CMakeFiles/spo.dir/graph/graph.c.o" \
"CMakeFiles/spo.dir/types/typization.c.o" \
"CMakeFiles/spo.dir/types/utils.c.o"

# External object files for target spo
spo_EXTERNAL_OBJECTS =

spo: CMakeFiles/spo.dir/main.c.o
spo: CMakeFiles/spo.dir/lang/MyLangLexer.c.o
spo: CMakeFiles/spo.dir/lang/MyLangParser.c.o
spo: CMakeFiles/spo.dir/ast/ast.c.o
spo: CMakeFiles/spo.dir/graph/graph.c.o
spo: CMakeFiles/spo.dir/types/typization.c.o
spo: CMakeFiles/spo.dir/types/utils.c.o
spo: CMakeFiles/spo.dir/build.make
spo: CMakeFiles/spo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/antlr/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking C executable spo"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/spo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/spo.dir/build: spo
.PHONY : CMakeFiles/spo.dir/build

CMakeFiles/spo.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/spo.dir/cmake_clean.cmake
.PHONY : CMakeFiles/spo.dir/clean

CMakeFiles/spo.dir/depend:
	cd /home/antlr/cmake-build-debug-wsl && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/antlr /home/antlr /home/antlr/cmake-build-debug-wsl /home/antlr/cmake-build-debug-wsl /home/antlr/cmake-build-debug-wsl/CMakeFiles/spo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/spo.dir/depend

