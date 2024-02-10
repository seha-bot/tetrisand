cmake_minimum_required(VERSION 3.10)

project(tetrisand VERSION 1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(compiler_options INTERFACE)
target_compile_options(compiler_options INTERFACE
    "-Wall;-Wextra;-Wshadow;-Wunused;-g"
)

add_subdirectory(src/gui)
add_subdirectory(src/simulation)

add_executable(tetrisand src/main.cpp)

target_include_directories(tetrisand PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(tetrisand PRIVATE
    compiler_options
    Gui
    Simulation
)
