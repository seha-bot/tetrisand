add_library(Simulation simulation.cpp)
target_include_directories(Simulation INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(Simulation PRIVATE compiler_options Gui)
