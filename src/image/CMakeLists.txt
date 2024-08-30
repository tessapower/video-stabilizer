# Image Source files
file(GLOB IMAGE_SOURCES *.c *.cpp)

list(APPEND
    IMAGE_SOURCES
    "CMakeLists.txt"
)

set(IMAGE_HEADERS
    "${PROJECT_SOURCE_DIR}/include/image/feature_tracker.h"
)

add_library(img_lib STATIC
    ${IMAGE_SOURCES}
    ${IMAGE_HEADERS}
)

target_link_libraries(img_lib PUBLIC ${OpenCV_LIBS})

# Support <my_lib/my_lib.h> imports in public headers
target_include_directories(img_lib PUBLIC ../include)
# Support "my_lib.h" imports in private headers and source files
target_include_directories(img_lib PRIVATE ../include/image)
