# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /home/dominic/Downloads/clion-2018.1.3/bin/cmake/bin/cmake

# The command to remove a file.
RM = /home/dominic/Downloads/clion-2018.1.3/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dominic/Tests

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dominic/Tests/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Tests.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Tests.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Tests.dir/flags.make

CMakeFiles/Tests.dir/main.cpp.o: CMakeFiles/Tests.dir/flags.make
CMakeFiles/Tests.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dominic/Tests/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Tests.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Tests.dir/main.cpp.o -c /home/dominic/Tests/main.cpp

CMakeFiles/Tests.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Tests.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dominic/Tests/main.cpp > CMakeFiles/Tests.dir/main.cpp.i

CMakeFiles/Tests.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Tests.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dominic/Tests/main.cpp -o CMakeFiles/Tests.dir/main.cpp.s

CMakeFiles/Tests.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/Tests.dir/main.cpp.o.requires

CMakeFiles/Tests.dir/main.cpp.o.provides: CMakeFiles/Tests.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/Tests.dir/build.make CMakeFiles/Tests.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/Tests.dir/main.cpp.o.provides

CMakeFiles/Tests.dir/main.cpp.o.provides.build: CMakeFiles/Tests.dir/main.cpp.o


# Object files for target Tests
Tests_OBJECTS = \
"CMakeFiles/Tests.dir/main.cpp.o"

# External object files for target Tests
Tests_EXTERNAL_OBJECTS =

Tests: CMakeFiles/Tests.dir/main.cpp.o
Tests: CMakeFiles/Tests.dir/build.make
Tests: CMakeFiles/Tests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dominic/Tests/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Tests"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Tests.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Tests.dir/build: Tests

.PHONY : CMakeFiles/Tests.dir/build

CMakeFiles/Tests.dir/requires: CMakeFiles/Tests.dir/main.cpp.o.requires

.PHONY : CMakeFiles/Tests.dir/requires

CMakeFiles/Tests.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Tests.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Tests.dir/clean

CMakeFiles/Tests.dir/depend:
	cd /home/dominic/Tests/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dominic/Tests /home/dominic/Tests /home/dominic/Tests/cmake-build-debug /home/dominic/Tests/cmake-build-debug /home/dominic/Tests/cmake-build-debug/CMakeFiles/Tests.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Tests.dir/depend

