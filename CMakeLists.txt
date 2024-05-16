cmake_minimum_required(VERSION 3.10)

project(Ulti_Minimax VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(ulti_game src/game_rules.cpp src/game_manager.cpp)
add_executable(ulti_minimax src/main.cpp)

target_link_libraries(ulti_minimax ulti_game)
target_include_directories(ulti_minimax PRIVATE ${PROJECT_SOURCE_DIR}/src)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Header Files" FILES
    src/game_rules.hpp
    src/game_manager.hpp
)

add_subdirectory(test)