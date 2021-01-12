# Set the CMake minimum version
cmake_minimum_required(VERSION 3.5)

PROJECT(rmcss VERSION 0.1.1 LANGUAGES C CXX)

set ( CMAKE_PREFIX_PATH /usr/lib/x86_64-linux-gnu/cmake/ )

# Find your dependencies for the project here
find_package(shunyaInterfaces REQUIRED )

# Global Install Directories
include(GNUInstallDirs)

set(SRC_FILES main.cpp)

# ----------------------------------
#    Section for compiling 1 binary 
# ----------------------------------
# Compile for one binary
add_executable(rmcss ${SRC_FILES})

# Define Compiler Flags to your library
target_compile_options(rmcss PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra -Wpedantic>)

# Define include directories 
target_include_directories(rmcss PUBLIC ${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Define the Include directories 
# <BUILD_INTERFACE> for those directories that are needed for compilation.
target_include_directories(rmcss PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

# Link your external libraries here
target_link_libraries(rmcss  PUBLIC  -lcurl siCore siYaml siPeripheral siConn siCloud siIiot)
# ----------------------------------
#    End Section for binary
# ----------------------------------