cmake_minimum_required(VERSION 3.19)
project(tetrisand
	VERSION 0.0.1
	LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_TTF REQUIRED IMPORTED_TARGET SDL2_ttf)
pkg_check_modules(SDL2_IMAGE REQUIRED IMPORTED_TARGET SDL2_image)

add_executable(tetrisand
	kiss_sdl/kiss_draw.c
	kiss_sdl/kiss_general.c
	kiss_sdl/kiss_posix.c
	kiss_sdl/kiss_widgets.c

	src/main.cpp
	src/graphics/graphics.cpp
)

target_include_directories(tetrisand PRIVATE
	${SDL2_INCLUDE_DIRS}
	kiss_sdl

	src/include
	src/graphics/include
)

set(BASE_FLAGS, -Wall -Wextra -Wshadow -Wunused)

if(DEBUG)
	target_compile_options(tetrisand PRIVATE ${BASE_FLAGS} -Werror -O3)
else()
	target_compile_options(tetrisand PRIVATE ${BASE_FLAGS} -g)
endif()

target_link_libraries(tetrisand PRIVATE
	SDL2::SDL2
	PkgConfig::SDL2_TTF
	PkgConfig::SDL2_IMAGE
)
