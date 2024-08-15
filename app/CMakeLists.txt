# Image Source files
file(GLOB APP_SOURCES *.c *.cpp)

list(APPEND
    APP_SOURCES
    "CMakeLists.txt"
)

set(APP_HEADERS
    "${PROJECT_SOURCE_DIR}/include/app/app.h"
    "${PROJECT_SOURCE_DIR}/include/app/shader.h"
    "${PROJECT_SOURCE_DIR}/include/utils.h"
)

add_executable(app ${APP_SOURCES} ${APP_HEADERS})

#########################################################
# Project Properties
#########################################################

set_property(TARGET app PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

#########################################################
# Link Libraries
#########################################################

target_link_libraries(app PRIVATE imgui glm)
target_link_libraries(app PRIVATE vid_lib)
