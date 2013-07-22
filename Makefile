# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jmccrae/oneta

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jmccrae/oneta

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/bin/ccmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/jmccrae/oneta/CMakeFiles /home/jmccrae/oneta/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/jmccrae/oneta/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named bin/lda

# Build rule for target.
bin/lda: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 bin/lda
.PHONY : bin/lda

# fast build rule for target.
bin/lda/fast:
	$(MAKE) -f CMakeFiles/bin/lda.dir/build.make CMakeFiles/bin/lda.dir/build
.PHONY : bin/lda/fast

#=============================================================================
# Target rules for targets named bin/lsi

# Build rule for target.
bin/lsi: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 bin/lsi
.PHONY : bin/lsi

# fast build rule for target.
bin/lsi/fast:
	$(MAKE) -f CMakeFiles/bin/lsi.dir/build.make CMakeFiles/bin/lsi.dir/build
.PHONY : bin/lsi/fast

#=============================================================================
# Target rules for targets named bin/mate-finding

# Build rule for target.
bin/mate-finding: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 bin/mate-finding
.PHONY : bin/mate-finding

# fast build rule for target.
bin/mate-finding/fast:
	$(MAKE) -f CMakeFiles/bin/mate-finding.dir/build.make CMakeFiles/bin/mate-finding.dir/build
.PHONY : bin/mate-finding/fast

#=============================================================================
# Target rules for targets named bin/oneta

# Build rule for target.
bin/oneta: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 bin/oneta
.PHONY : bin/oneta

# fast build rule for target.
bin/oneta/fast:
	$(MAKE) -f CMakeFiles/bin/oneta.dir/build.make CMakeFiles/bin/oneta.dir/build
.PHONY : bin/oneta/fast

#=============================================================================
# Target rules for targets named bin/sim-csv

# Build rule for target.
bin/sim-csv: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 bin/sim-csv
.PHONY : bin/sim-csv

# fast build rule for target.
bin/sim-csv/fast:
	$(MAKE) -f CMakeFiles/bin/sim-csv.dir/build.make CMakeFiles/bin/sim-csv.dir/build
.PHONY : bin/sim-csv/fast

src/lda.o: src/lda.cpp.o
.PHONY : src/lda.o

# target to build an object file
src/lda.cpp.o:
	$(MAKE) -f CMakeFiles/bin/lda.dir/build.make CMakeFiles/bin/lda.dir/src/lda.cpp.o
.PHONY : src/lda.cpp.o

src/lda.i: src/lda.cpp.i
.PHONY : src/lda.i

# target to preprocess a source file
src/lda.cpp.i:
	$(MAKE) -f CMakeFiles/bin/lda.dir/build.make CMakeFiles/bin/lda.dir/src/lda.cpp.i
.PHONY : src/lda.cpp.i

src/lda.s: src/lda.cpp.s
.PHONY : src/lda.s

# target to generate assembly for a file
src/lda.cpp.s:
	$(MAKE) -f CMakeFiles/bin/lda.dir/build.make CMakeFiles/bin/lda.dir/src/lda.cpp.s
.PHONY : src/lda.cpp.s

src/lsi.o: src/lsi.cpp.o
.PHONY : src/lsi.o

# target to build an object file
src/lsi.cpp.o:
	$(MAKE) -f CMakeFiles/bin/lsi.dir/build.make CMakeFiles/bin/lsi.dir/src/lsi.cpp.o
.PHONY : src/lsi.cpp.o

src/lsi.i: src/lsi.cpp.i
.PHONY : src/lsi.i

# target to preprocess a source file
src/lsi.cpp.i:
	$(MAKE) -f CMakeFiles/bin/lsi.dir/build.make CMakeFiles/bin/lsi.dir/src/lsi.cpp.i
.PHONY : src/lsi.cpp.i

src/lsi.s: src/lsi.cpp.s
.PHONY : src/lsi.s

# target to generate assembly for a file
src/lsi.cpp.s:
	$(MAKE) -f CMakeFiles/bin/lsi.dir/build.make CMakeFiles/bin/lsi.dir/src/lsi.cpp.s
.PHONY : src/lsi.cpp.s

src/mate-finding.o: src/mate-finding.cpp.o
.PHONY : src/mate-finding.o

# target to build an object file
src/mate-finding.cpp.o:
	$(MAKE) -f CMakeFiles/bin/mate-finding.dir/build.make CMakeFiles/bin/mate-finding.dir/src/mate-finding.cpp.o
.PHONY : src/mate-finding.cpp.o

src/mate-finding.i: src/mate-finding.cpp.i
.PHONY : src/mate-finding.i

# target to preprocess a source file
src/mate-finding.cpp.i:
	$(MAKE) -f CMakeFiles/bin/mate-finding.dir/build.make CMakeFiles/bin/mate-finding.dir/src/mate-finding.cpp.i
.PHONY : src/mate-finding.cpp.i

src/mate-finding.s: src/mate-finding.cpp.s
.PHONY : src/mate-finding.s

# target to generate assembly for a file
src/mate-finding.cpp.s:
	$(MAKE) -f CMakeFiles/bin/mate-finding.dir/build.make CMakeFiles/bin/mate-finding.dir/src/mate-finding.cpp.s
.PHONY : src/mate-finding.cpp.s

src/oneta-gsl.o: src/oneta-gsl.cpp.o
.PHONY : src/oneta-gsl.o

# target to build an object file
src/oneta-gsl.cpp.o:
	$(MAKE) -f CMakeFiles/bin/oneta.dir/build.make CMakeFiles/bin/oneta.dir/src/oneta-gsl.cpp.o
.PHONY : src/oneta-gsl.cpp.o

src/oneta-gsl.i: src/oneta-gsl.cpp.i
.PHONY : src/oneta-gsl.i

# target to preprocess a source file
src/oneta-gsl.cpp.i:
	$(MAKE) -f CMakeFiles/bin/oneta.dir/build.make CMakeFiles/bin/oneta.dir/src/oneta-gsl.cpp.i
.PHONY : src/oneta-gsl.cpp.i

src/oneta-gsl.s: src/oneta-gsl.cpp.s
.PHONY : src/oneta-gsl.s

# target to generate assembly for a file
src/oneta-gsl.cpp.s:
	$(MAKE) -f CMakeFiles/bin/oneta.dir/build.make CMakeFiles/bin/oneta.dir/src/oneta-gsl.cpp.s
.PHONY : src/oneta-gsl.cpp.s

src/sim-csv.o: src/sim-csv.cpp.o
.PHONY : src/sim-csv.o

# target to build an object file
src/sim-csv.cpp.o:
	$(MAKE) -f CMakeFiles/bin/sim-csv.dir/build.make CMakeFiles/bin/sim-csv.dir/src/sim-csv.cpp.o
.PHONY : src/sim-csv.cpp.o

src/sim-csv.i: src/sim-csv.cpp.i
.PHONY : src/sim-csv.i

# target to preprocess a source file
src/sim-csv.cpp.i:
	$(MAKE) -f CMakeFiles/bin/sim-csv.dir/build.make CMakeFiles/bin/sim-csv.dir/src/sim-csv.cpp.i
.PHONY : src/sim-csv.cpp.i

src/sim-csv.s: src/sim-csv.cpp.s
.PHONY : src/sim-csv.s

# target to generate assembly for a file
src/sim-csv.cpp.s:
	$(MAKE) -f CMakeFiles/bin/sim-csv.dir/build.make CMakeFiles/bin/sim-csv.dir/src/sim-csv.cpp.s
.PHONY : src/sim-csv.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... bin/lda"
	@echo "... bin/lsi"
	@echo "... bin/mate-finding"
	@echo "... bin/oneta"
	@echo "... bin/sim-csv"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... src/lda.o"
	@echo "... src/lda.i"
	@echo "... src/lda.s"
	@echo "... src/lsi.o"
	@echo "... src/lsi.i"
	@echo "... src/lsi.s"
	@echo "... src/mate-finding.o"
	@echo "... src/mate-finding.i"
	@echo "... src/mate-finding.s"
	@echo "... src/oneta-gsl.o"
	@echo "... src/oneta-gsl.i"
	@echo "... src/oneta-gsl.s"
	@echo "... src/sim-csv.o"
	@echo "... src/sim-csv.i"
	@echo "... src/sim-csv.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

