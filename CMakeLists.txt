cmake_minimum_required(VERSION 3.15)
project(JWordTetris)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(SDL2_INCLUDE_DIR)
include_directories(SDL2_TTF_INCLUDE_DIR)

FILE(GLOB JWordTetris_Headers *.h)
FILE(GLOB JWordTetris_Sources *.cpp)

file(GLOB ImGui_Headers ImGui/*.h)
file(GLOB ImGui_Sources ImGui/*.cpp)

add_executable(JWordTetris ${ImGui_Headers} ${ImGui_Sources} ${JWordTetris_Headers} ${JWordTetris_Sources})

target_link_libraries(JWordTetris PRIVATE glfw glm::glm OpenGL SDL2 SDL2_ttf)
