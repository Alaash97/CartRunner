cmake_minimum_required(VERSION 3.10)

project(CartRunner CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/DinoGame/include)

# Gather all source files
file(GLOB SOURCES "DinoGame/src/*.cpp")

# Define executable
add_executable(CartRunner ${SOURCES})

# Add compiler warnings
if(MSVC)
    # Replace MSVC's default /W3 with /W4 to avoid the override warning
    string(REGEX REPLACE "/W[0-3]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
    target_compile_options(CartRunner PRIVATE -Wall -Wextra)
endif()
