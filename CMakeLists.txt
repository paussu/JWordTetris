cmake_minimum_required(VERSION 3.15)
project(WordTetris)

set(CMAKE_CXX_STANDARD 20)

include_directories(OPENGL_INCLUDE_DIR)
include_directories(GLEW_INCLUDE_DIR)
include_directories(SDL2_INCLUDE_DIR)

add_executable(WordTetris main.cpp Game.cpp Game.h Menu.cpp Menu.h nuklear.h nuklear_sdl_gl3.h Block.cpp Block.h)

target_link_libraries(WordTetris SDL2 SDL2_ttf GL GLU GLEW)
