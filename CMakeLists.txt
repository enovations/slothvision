cmake_minimum_required(VERSION 2.6)
project(SlothVision)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic")

cmake_policy(SET CMP0015 NEW)

include_directories(OpenCV/include)
link_directories(OpenCV/lib)

set(OPENCV_LIBRARIES
        opencv_core
        opencv_videoio
        opencv_highgui
        opencv_aruco
        opencv_imgproc
        opencv_imgcodecs)

add_subdirectory(slothvision)
