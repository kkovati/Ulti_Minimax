include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(test_ulti_game test_ulti_game.cpp)
target_link_libraries(test_ulti_game GTest::gtest_main ulti_game)

include(GoogleTest)
gtest_discover_tests(test_ulti_game)

include_directories(${PROJECT_SOURCE_DIR}/cpp/src)
