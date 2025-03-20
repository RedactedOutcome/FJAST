cmake_minimum_required(VERSION 3.10)
project(FJASTParser)

# Create a static library from the src/ folder
add_library(FJASTP STATIC src/*.cpp)

target_precompile_headers(FJASTP PRIVATE include/jfastp/pch.h)
if (MSVC)
    target_sources(FJASTP PRIVATE src/pch.cpp)
endif()

# Include directory for public headers
target_include_directories(FJASTP PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_include_directories(FJASTP PUBLIC ${CMAKE_SOURCE_DIR}/include/)