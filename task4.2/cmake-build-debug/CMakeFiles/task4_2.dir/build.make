# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /snap/clion/85/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/85/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zygalo/C/Mipt_3_sem/task4.2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zygalo/C/Mipt_3_sem/task4.2/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/task4_2.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/task4_2.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/task4_2.dir/flags.make

CMakeFiles/task4_2.dir/main.c.o: CMakeFiles/task4_2.dir/flags.make
CMakeFiles/task4_2.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zygalo/C/Mipt_3_sem/task4.2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/task4_2.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/task4_2.dir/main.c.o   -c /home/zygalo/C/Mipt_3_sem/task4.2/main.c

CMakeFiles/task4_2.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/task4_2.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zygalo/C/Mipt_3_sem/task4.2/main.c > CMakeFiles/task4_2.dir/main.c.i

CMakeFiles/task4_2.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/task4_2.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zygalo/C/Mipt_3_sem/task4.2/main.c -o CMakeFiles/task4_2.dir/main.c.s

# Object files for target task4_2
task4_2_OBJECTS = \
"CMakeFiles/task4_2.dir/main.c.o"

# External object files for target task4_2
task4_2_EXTERNAL_OBJECTS =

task4_2: CMakeFiles/task4_2.dir/main.c.o
task4_2: CMakeFiles/task4_2.dir/build.make
task4_2: CMakeFiles/task4_2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zygalo/C/Mipt_3_sem/task4.2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable task4_2"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/task4_2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/task4_2.dir/build: task4_2

.PHONY : CMakeFiles/task4_2.dir/build

CMakeFiles/task4_2.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/task4_2.dir/cmake_clean.cmake
.PHONY : CMakeFiles/task4_2.dir/clean

CMakeFiles/task4_2.dir/depend:
	cd /home/zygalo/C/Mipt_3_sem/task4.2/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zygalo/C/Mipt_3_sem/task4.2 /home/zygalo/C/Mipt_3_sem/task4.2 /home/zygalo/C/Mipt_3_sem/task4.2/cmake-build-debug /home/zygalo/C/Mipt_3_sem/task4.2/cmake-build-debug /home/zygalo/C/Mipt_3_sem/task4.2/cmake-build-debug/CMakeFiles/task4_2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/task4_2.dir/depend

