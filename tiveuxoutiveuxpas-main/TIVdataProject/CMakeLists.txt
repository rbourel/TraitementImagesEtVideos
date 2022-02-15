cmake_minimum_required(VERSION 3.14)

set(CMAKE_C_COMPILER /usr/bin/gcc-9 CACHE PATH "" FORCE)
set(CMAKE_CXX_COMPILER /usr/bin/g++-9 CACHE PATH "" FORCE)

project(TIVdataProject)


set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)

include(CheckIncludeFileCXX)
check_include_file_cxx(opencv2/face/facerec.hpp HAVE_OPENCV_CONTRIB)
if(HAVE_OPENCV_CONTRIB)
    add_definitions(-DHAVE_OPENCV_CONTRIB)
endif(HAVE_OPENCV_CONTRIB)


include_directories(include ${OpenCV_INCLUDE_DIRS})


add_executable(TIVdataProject
        include/histogram.hpp
        src/main.cpp)

target_link_libraries(TIVdataProject ${OpenCV_LIBS})

