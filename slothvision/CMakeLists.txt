add_subdirectory(marker)
add_subdirectory(network)
add_subdirectory(render)

include_directories(marker)
include_directories(network)
include_directories(render)

set(SLOTHVISION_FILES
        slothvision.cpp
        )

add_executable(SlothVision ${SLOTHVISION_FILES})
target_link_libraries(SlothVision marker network ${OPENCV_LIBRARIES})