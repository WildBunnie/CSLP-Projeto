cmake_minimum_required(VERSION 3.10)

project(image_video_compression)

# Set the C++ standard (you can change it based on your requirements)
set(CMAKE_CXX_STANDARD 11)

# Add include directories for header files
include_directories(include)

# Collect all header files in the 'include' folder
file(GLOB HEADER_FILES include/*.h)

# Collect all source files in the 'src' folder
file(GLOB SOURCE_FILES src/*.cpp)
find_package( OpenCV REQUIRED )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
include_directories( ${OpenCV_INCLUDE_DIRS} )

# Add executable and specify source files
add_executable(main ${HEADER_FILES} ${SOURCE_FILES})
target_compile_options(main PRIVATE -O2 -Wall)
target_link_libraries( main ${OpenCV_LIBS} )