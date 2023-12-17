add_library(Gui gui.cpp)
target_include_directories(Gui INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(Gui PRIVATE compiler_options)

find_library(SDL2 /usr/lib)
target_link_libraries(Gui PUBLIC SDL2)
