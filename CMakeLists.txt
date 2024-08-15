cmake_minimum_required(VERSION 3.30)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)

# Set CMake Toolchain to use vcpkg
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}//scripts//buildsystems//vcpkg.cmake")

# Project
project(video_stabilizer VERSION 0.0.1)

# Enable IDE Project Folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Create a directory for our executable
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/bin" ${CMAKE_MODULE_PATH})

#########################################################
# Force Output Directories
# In general, this isn't a very friendly thing to do,
# but we'll do it anyway so the .exe files are in a
# known location.
#########################################################

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

#########################################################
# External Libraries
#########################################################

set(OpenCV_DIR "${VCPKG_INSTALLED_DIR}/x64-windows/share/opencv4")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(nfd CONFIG REQUIRED)

#########################################################
# Set Compiler Flags
#########################################################

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # # C++ latest
    add_compile_options(/std:c++latest)
    # UTF-8 source and execution charsets
    add_compile_options(/utf-8)
    # Full normal warnings, multithreaded build
    add_compile_options(/W4 /MP)
    # Disable C4800: forcing X to bool (performance warning)
    add_compile_options(/wd4800)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options("$<$<NOT:$<CONFIG:Debug>>:-O2>")
    # # C++17, full normal warnings
    add_compile_options(-std=c++1z -Wall -Wextra -pedantic)
    # C++14, full normal warnings
    # add_compile_options(-std=c++1y -Wall -Wextra -pedantic)
    # don't export by default
    add_compile_options(-fvisibility=hidden)
    # Threading support, enable SSE2
    add_compile_options(-pthread -msse2)
    # Promote missing return to error
    add_compile_options(-Werror=return-type)
    # enable coloured output if gcc >= 4.9
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (GCC_VERSION VERSION_GREATER 4.9 OR GCC_VERSION VERSION_EQUAL 4.9)
        add_compile_options(-fdiagnostics-color)
    endif()
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "^(Apple)?Clang$")
    add_compile_options("$<$<NOT:$<CONFIG:Debug>>:-O2>")
    # # C++17, full normal warnings
    add_compile_options(-std=c++1z -Wall -Wextra -pedantic)
    # C++14, full normal warnings
    # add_compile_options(-std=c++1y -Wall -Wextra -pedantic)
    # don't export by default
    add_compile_options(-fvisibility=hidden)
    # Threading support, enable SSE2
    add_compile_options(-pthread -msse2)
    # Promote missing return to error
    add_compile_options(-Werror=return-type)
endif()

#########################################################
# Include directory for header files
#########################################################

include_directories(
    "${VCPKG_INSTALLED_DIR}/${MATCHED_TRIPLET}/include"
    "${PROJECT_SOURCE_DIR}/include"
    )

#########################################################
# Source Files
#########################################################

add_subdirectory(app)
add_subdirectory(src)
add_subdirectory(shaders)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT app)
