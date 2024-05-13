cmake_minimum_required(VERSION 3.10)

project(Ulti_Minimax VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Ulti_Minimax src/main.cpp src/game_manager.cpp src/game_rules.cpp)

target_include_directories(Ulti_Minimax PRIVATE ${PROJECT_SOURCE_DIR}/src)
