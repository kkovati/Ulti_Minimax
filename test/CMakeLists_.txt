enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1 
)

FetchContent_MakeAvailable(googletest)

include_directories(${GTEST_SOURCE_DIR}/include ${GTEST_SOURCE_DIR} ../src)

add_executable(test_game_rules test_game_rules.cpp)

target_link_libraries(test_game_rules ${GTEST_LIBRARIES} gtest gtest_main pthread ulti_game game_rules)

target_include_directories(test_game_rules PRIVATE ${PROJECT_SOURCE_DIR}/src)

include(GoogleTest)

gtest_discover_tests(test_game_rules)
