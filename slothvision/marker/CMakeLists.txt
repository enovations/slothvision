SET(MARKER_LIBRARY_FILES
        marker.cpp)

add_library(marker ${MARKER_LIBRARY_FILES})

add_executable(marker_test marker_test.cpp)
target_link_libraries(marker_test marker ${OPENCV_LIBRARIES})
