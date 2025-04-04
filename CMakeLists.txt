cmake_minimum_required(VERSION 3.12)
project(FJASTP)

option(BUILD_EXAMPLE "Build Example Executable" ON)  # Default to ON

set(CMAKE_CXX_STANDARD 17)        # Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Ensure at least C++17 is used

# Create a static library from the src/ folder
file(GLOB SOURCES "src/*.cpp")
add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_precompile_headers(${PROJECT_NAME} PRIVATE include/FJASTP/pch.h)
if (MSVC)
    target_sources(${PROJECT_NAME} PRIVATE src/pch.cpp)
endif()

#Output Directories
if(CMAKE_BUILD_TYPE)
    set(BUILD_TYPE ${CMAKE_BUILD_TYPE}) # For Make, Ninja
else()
    set(BUILD_TYPE $<CONFIG>) # For Visual Studio, Xcode
endif()

# Include directory for public headers
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include/)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}/)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/libs/HBuffer/include/)

add_compile_definitions(FJASTP_USE_PCH FJASTP_PCH_DIR=\"FJASTP/pch.h\")

source_group(TREE ${CMAKE_SOURCE_DIR} PREFIX "Source" FILES src)
source_group(TREE ${CMAKE_SOURCE_DIR} PREFIX "Include" FILES include)

#Just keeping this even though we are probably never going to build for x86 ever
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(TARGET_ARCH "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(TARGET_ARCH "x86")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_PROJECT_NAME}/${TARGET_ARCH}-${BUILD_TYPE}) #Output Directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_PROJECT_NAME}/${TARGET_ARCH}-${BUILD_TYPE}) #Output Directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_PROJECT_NAME}/${TARGET_ARCH}-${BUILD_TYPE}) #Output Directories

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG}  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG}  ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG}  ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    )
endforeach()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
)

add_subdirectory("example/")