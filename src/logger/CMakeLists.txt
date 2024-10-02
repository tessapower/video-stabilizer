# Image Source files
file(GLOB LOGGER_SOURCES *.c *.cpp)

list(APPEND
LOGGER_SOURCES
    "CMakeLists.txt"
)

set(LOGGER_HEADERS
    "${PROJECT_SOURCE_DIR}/include/logger/logger.h"
)

add_library(logger_lib STATIC
    ${LOGGER_SOURCES}
    ${LOGGER_HEADERS}
)

target_link_libraries(logger_lib PUBLIC imgui::imgui)

# Support <my_lib/my_lib.h> imports in public headers
target_include_directories(logger_lib PUBLIC ../include)
# Support "my_lib.h" imports in private headers and source files
target_include_directories(logger_lib PRIVATE ../include/logger)
