cmake_minimum_required(VERSION 3.16)

project(bitStream)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add your source files
set(SOURCES
    src/main.cpp
    src/BitStream.cpp
)

# Add header files (optional)
set(HEADERS
    include/BitStream.h
)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add any additional include directories (optional)
include_directories(
    include
)

# Create an executable from the source files
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})


