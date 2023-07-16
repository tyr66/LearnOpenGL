set(GLFWINCLUDE "./3rdlib/include")
set(GLFWLIB "./3rdlib/lib")

aux_source_directory(. DIR_SRCS)

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(start)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${GLFWINCLUDE})
link_directories(${GLFWLIB})

add_executable(start ${DIR_SRCS})

target_link_libraries(start opengl32.lib)
target_link_libraries(start glfw3)
