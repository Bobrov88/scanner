# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/project/scanner

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/project/scanner/build_centos_x86

# Include any dependencies generated for this target.
include src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/compiler_depend.make

# Include the progress variables for this target.
include src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/progress.make

# Include the compile flags for this target's objects.
include src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/flags.make

src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/hid.c.o: src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/flags.make
src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/hid.c.o: ../src/hidapi/linux/hid.c
src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/hid.c.o: src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/project/scanner/build_centos_x86/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/hid.c.o"
	cd /home/project/scanner/build_centos_x86/src/hidapi/src/linux && /usr/local/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/hid.c.o -MF CMakeFiles/hidapi_hidraw.dir/hid.c.o.d -o CMakeFiles/hidapi_hidraw.dir/hid.c.o -c /home/project/scanner/src/hidapi/linux/hid.c

src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/hid.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/hidapi_hidraw.dir/hid.c.i"
	cd /home/project/scanner/build_centos_x86/src/hidapi/src/linux && /usr/local/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/project/scanner/src/hidapi/linux/hid.c > CMakeFiles/hidapi_hidraw.dir/hid.c.i

src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/hid.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/hidapi_hidraw.dir/hid.c.s"
	cd /home/project/scanner/build_centos_x86/src/hidapi/src/linux && /usr/local/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/project/scanner/src/hidapi/linux/hid.c -o CMakeFiles/hidapi_hidraw.dir/hid.c.s

# Object files for target hidapi_hidraw
hidapi_hidraw_OBJECTS = \
"CMakeFiles/hidapi_hidraw.dir/hid.c.o"

# External object files for target hidapi_hidraw
hidapi_hidraw_EXTERNAL_OBJECTS =

src/hidapi/src/linux/libhidapi-hidraw.a: src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/hid.c.o
src/hidapi/src/linux/libhidapi-hidraw.a: src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/build.make
src/hidapi/src/linux/libhidapi-hidraw.a: src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/project/scanner/build_centos_x86/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libhidapi-hidraw.a"
	cd /home/project/scanner/build_centos_x86/src/hidapi/src/linux && $(CMAKE_COMMAND) -P CMakeFiles/hidapi_hidraw.dir/cmake_clean_target.cmake
	cd /home/project/scanner/build_centos_x86/src/hidapi/src/linux && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/hidapi_hidraw.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/build: src/hidapi/src/linux/libhidapi-hidraw.a
.PHONY : src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/build

src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/clean:
	cd /home/project/scanner/build_centos_x86/src/hidapi/src/linux && $(CMAKE_COMMAND) -P CMakeFiles/hidapi_hidraw.dir/cmake_clean.cmake
.PHONY : src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/clean

src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/depend:
	cd /home/project/scanner/build_centos_x86 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/project/scanner /home/project/scanner/src/hidapi/linux /home/project/scanner/build_centos_x86 /home/project/scanner/build_centos_x86/src/hidapi/src/linux /home/project/scanner/build_centos_x86/src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/hidapi/src/linux/CMakeFiles/hidapi_hidraw.dir/depend

