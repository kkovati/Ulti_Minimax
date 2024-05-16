add_executable(ulti_minimax main.cpp)

add_library(ulti_game game_rules.cpp game_manager.cpp)

target_link_libraries(ulti_minimax ulti_game)
target_include_directories(ulti_minimax PRIVATE ${PROJECT_SOURCE_DIR}/src)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Header Files" FILES
    src/game_rules.hpp
    src/game_manager.hpp
)