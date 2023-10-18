cmake_minimum_required(VERSION 3.16)
project( imageManipulation )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(copy PPMImage.cpp PPMImage.hpp copy.cpp)