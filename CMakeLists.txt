cmake_minimum_required(VERSION 3.10)
project(FJASTP)

# Create a static library from the src/ folder
file(GLOB SOURCES "src/*.cpp")
add_library(FJASTP STATIC ${SOURCES})

target_precompile_headers(FJASTP PRIVATE include/FJASTP/pch.h)
if (MSVC)
    target_sources(FJASTP PRIVATE src/pch.cpp)
endif()

set(CMAKE_CXX_STANDARD 17)        # Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Ensure at least C++17 is used

# Include directory for public headers
target_include_directories(FJASTP PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_include_directories(FJASTP PUBLIC ${CMAKE_SOURCE_DIR}/include/)
target_include_directories(FJASTP PUBLIC ${CMAKE_SOURCE_DIR}/include/FJASTP/)

target_include_directories(FJASTP PUBLIC ${CMAKE_SOURCE_DIR}/libs/HBuffer/include/)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)  # Static libraries
set(CMAKE_INTERMEDIATE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/int)  # Intermediate files (Custom)