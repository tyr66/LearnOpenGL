set(GLFWINCLUDE "./3rdlib/include")
set(GLFWLIB "./3rdlib/lib")

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(start)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${GLFWINCLUDE})
link_directories(${GLFWLIB})

add_executable(start main.cpp App.h App.cpp glad.c)

target_link_libraries(start opengl32.lib)
target_link_libraries(start glfw3)
