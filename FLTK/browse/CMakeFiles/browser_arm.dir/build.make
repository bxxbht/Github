# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_SOURCE_DIR = /root/Information/G系列业务/多语言/中文GB2312/FLTK/browse

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/Information/G系列业务/多语言/中文GB2312/FLTK/browse

# Include any dependencies generated for this target.
include CMakeFiles/browser_arm.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/browser_arm.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/browser_arm.dir/flags.make

CMakeFiles/browser_arm.dir/browser.o: CMakeFiles/browser_arm.dir/flags.make
CMakeFiles/browser_arm.dir/browser.o: browser.cxx
	$(CMAKE_COMMAND) -E cmake_progress_report /root/Information/G系列业务/多语言/中文GB2312/FLTK/browse/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/browser_arm.dir/browser.o"
	arm-linux-g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/browser_arm.dir/browser.o -c /root/Information/G系列业务/多语言/中文GB2312/FLTK/browse/browser.cxx

CMakeFiles/browser_arm.dir/browser.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/browser_arm.dir/browser.i"
	arm-linux-g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /root/Information/G系列业务/多语言/中文GB2312/FLTK/browse/browser.cxx > CMakeFiles/browser_arm.dir/browser.i

CMakeFiles/browser_arm.dir/browser.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/browser_arm.dir/browser.s"
	arm-linux-g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /root/Information/G系列业务/多语言/中文GB2312/FLTK/browse/browser.cxx -o CMakeFiles/browser_arm.dir/browser.s

CMakeFiles/browser_arm.dir/browser.o.requires:
.PHONY : CMakeFiles/browser_arm.dir/browser.o.requires

CMakeFiles/browser_arm.dir/browser.o.provides: CMakeFiles/browser_arm.dir/browser.o.requires
	$(MAKE) -f CMakeFiles/browser_arm.dir/build.make CMakeFiles/browser_arm.dir/browser.o.provides.build
.PHONY : CMakeFiles/browser_arm.dir/browser.o.provides

CMakeFiles/browser_arm.dir/browser.o.provides.build: CMakeFiles/browser_arm.dir/browser.o

# Object files for target browser_arm
browser_arm_OBJECTS = \
"CMakeFiles/browser_arm.dir/browser.o"

# External object files for target browser_arm
browser_arm_EXTERNAL_OBJECTS =

browser_arm: CMakeFiles/browser_arm.dir/browser.o
browser_arm: CMakeFiles/browser_arm.dir/build.make
browser_arm: CMakeFiles/browser_arm.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable browser_arm"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/browser_arm.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/browser_arm.dir/build: browser_arm
.PHONY : CMakeFiles/browser_arm.dir/build

CMakeFiles/browser_arm.dir/requires: CMakeFiles/browser_arm.dir/browser.o.requires
.PHONY : CMakeFiles/browser_arm.dir/requires

CMakeFiles/browser_arm.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/browser_arm.dir/cmake_clean.cmake
.PHONY : CMakeFiles/browser_arm.dir/clean

CMakeFiles/browser_arm.dir/depend:
	cd /root/Information/G系列业务/多语言/中文GB2312/FLTK/browse && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/Information/G系列业务/多语言/中文GB2312/FLTK/browse /root/Information/G系列业务/多语言/中文GB2312/FLTK/browse /root/Information/G系列业务/多语言/中文GB2312/FLTK/browse /root/Information/G系列业务/多语言/中文GB2312/FLTK/browse /root/Information/G系列业务/多语言/中文GB2312/FLTK/browse/CMakeFiles/browser_arm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/browser_arm.dir/depend
