add_subdirectory(marker)
add_subdirectory(network)
add_subdirectory(render)
add_subdirectory(pid)
add_subdirectory(control)

include_directories(marker)
include_directories(network)
include_directories(render)
include_directories(pid)
include_directories(control)

set(SLOTHVISION_FILES
        slothvision.cpp
        )

add_executable(SlothVision ${SLOTHVISION_FILES})
target_link_libraries(SlothVision marker network pid control ${OPENCV_LIBRARIES})