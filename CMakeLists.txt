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

#Just keeping this even though we are probably never going to build for x86 ever
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(TARGET_ARCH "x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(TARGET_ARCH "x86")
endif()

#Example
add_executable(FJASTPExample examples/test/Main.cpp)
target_link_libraries(FJASTPExample PRIVATE FJASTP)
target_include_directories(FJASTPExample PUBLIC ${CMAKE_SOURCE_DIR}/libs/HBuffer/include/)
target_include_directories(FJASTPExample PUBLIC ${CMAKE_SOURCE_DIR}/include/)

#Output Directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_PROJECT_NAME}/${TARGET_ARCH}) #Output Directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_PROJECT_NAME}/${TARGET_ARCH}) #Output Directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_PROJECT_NAME}/${TARGET_ARCH}) #Output Directories

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set_target_properties(FJASTP PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    )
    set_target_properties(FJASTPExample PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin/FJASTPExample/${TARGET_ARCH}
        LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin/FJASTPExample/${TARGET_ARCH}
        ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin/FJASTPExample/${TARGET_ARCH}
    )
endforeach()
set_target_properties(FJASTP PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
)
set_target_properties(FJASTPExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/FJASTPExample/${TARGET_ARCH}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/FJASTPExample/${TARGET_ARCH}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/FJASTPExample/${TARGET_ARCH}
)
# **Important: Fix for NMake Makefiles**
if(CMAKE_GENERATOR MATCHES "NMake Makefiles")
    set_target_properties(FJASTP PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_PROJECT_NAME}/${TARGET_ARCH}"
    )
    set_target_properties(FJASTPExample PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/FJASTPExample/${TARGET_ARCH}"
    )
endif()
set_target_properties(FJASTP PROPERTIES
    OBJECT_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/int/JFASTP/
)

set_target_properties(FJASTPExample PROPERTIES
    OBJECT_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/int/FJASTPExample/
)

#Run the program
add_custom_target(run
    COMMAND $<TARGET_FILE:FJASTPExample>
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS FJASTPExample
)