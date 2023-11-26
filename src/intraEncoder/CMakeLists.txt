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
    BitStream.cpp
    intraEncoder.cpp)

set(HEADERS
    include/BitStream.h
    include/Golomb.h
    include/intraEncoder.h
)

# set(CMAKE_CXX_FLAGS "-Wall -Wextra")
# set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
add_executable(main main.cpp ${SOURCES})
target_compile_options(main PRIVATE -g)
target_link_libraries( main ${OpenCV_LIBS} )