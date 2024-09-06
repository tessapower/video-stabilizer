# Completion Source files
file(GLOB VIDEO_SOURCES *.c *.cpp)

list(APPEND
    VIDEO_SOURCES
    "CMakeLists.txt"
)

set(VIDEO_HEADERS
    "${PROJECT_SOURCE_DIR}/include/video/stabilizer.h"
    "${PROJECT_SOURCE_DIR}/include/video/vid.h"
)

add_library(vid_lib STATIC
    ${VIDEO_SOURCES}
    ${VIDEO_HEADERS}
)

target_link_libraries(vid_lib PUBLIC ${OpenCV_LIBS})
target_link_libraries(vid_lib PRIVATE img_lib)

# Support <my_lib/my_lib.h> imports in public headers
target_include_directories(vid_lib PUBLIC ../include)
# Support "my_lib.h" imports in private headers and source files
target_include_directories(vid_lib PRIVATE ../include/video)
