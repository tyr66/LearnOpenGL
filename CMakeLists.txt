set(INCLUDEDIR "./3rdlib/include")
set(LIBDIR "./3rdlib/lib")

aux_source_directory(. DIR_SRCS)

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(start)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Specify MSVC UTF-8 encoding
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(${INCLUDEDIR})
link_directories(${LIBDIR})

add_executable(start ${DIR_SRCS})

target_link_libraries(start opengl32.lib)
target_link_libraries(start glfw3.lib)
target_link_libraries(start assimp-vc143-mt.lib)
