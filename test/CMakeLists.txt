enable_testing()

find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS} ../src)

add_executable(test_game_rules test_game_rules.cpp)

target_link_libraries(test_game_rules ${GTEST_LIBRARIES} pthread game_rules)

gtest_discover_tests(test_game_rules)