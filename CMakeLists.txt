cmake_minimum_required(VERSION 3.21.0)
project(kittygui VERSION 0.0.1)

macro(add_prebuild_command PREBUILD_TARGET STITCH)
add_custom_command(TARGET ${PREBUILD_TARGET} PRE_BUILD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND quom "src/stitches/stitch_${STITCH}.hpp" "kittylib_${STITCH}.hpp"
)
endmacro()

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

file(GLOB_RECURSE SRC "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp *.hpp)

find_package(DirectX REQUIRED)

add_subdirectory(examples)
