cmake_minimum_required(VERSION 3.16)
project( encoder )
#find_package ( OpenCV REQUIRED PATHS ~/build NO_DEFAULT_PATH)
find_package( OpenCV REQUIRED )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories(
    include
)
set(SOURCES
    Golomb.cpp
    BitStream.cpp)

set(HEADERS
    include/BitStream.h
    include/Golomb.h
)
add_executable(encoder encoder.cpp ${SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE -g)
target_link_libraries( encoder ${OpenCV_LIBS} )