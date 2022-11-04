cmake_minimum_required(VERSION 3.21.0)
project(examples VERSION 0.0.1)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_STANDARD 20)

# Find packages
find_package(DirectX REQUIRED)
find_package(OpenGL)

# set(SRC  ) # for header only mode
file(GLOB_RECURSE SRC_EXAMPLE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
list(APPEND SRC ${SRC_EXAMPLE})
message("${SRC}")
add_executable(testing ${SRC})

add_prebuild_command(testing d3d9)

include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})

target_link_libraries(testing PRIVATE
  # DirectX
  ${DirectX_LIBRARY}
  ${DirectX_D3DX9_LIBRARY}
  
  # OpenGL
  ${OPENGL_LIBRARIES} 
  ${GLUT_LIBRARY})

set_target_properties(testing
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/$<CONFIG>/testing)