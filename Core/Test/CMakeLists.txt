cmake_minimum_required(VERSION 4.0.3)
enable_testing()

find_package(GTest REQUIRED)

add_executable(AllTests
    Entity/CardTest.cpp
    Entity/DeckTest.cpp
    TableState/PlayerHandler.cpp
    TableState/BettingTest.cpp
)

target_link_libraries(AllTests
    PRIVATE
        MyHeaders
        MyImpl
        GTest::gtest_main
)

target_include_directories(AllTests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Path to test files
target_compile_definitions(AllTests
    PRIVATE
        TEST_CASES_PLAYERS="${CMAKE_CURRENT_SOURCE_DIR}/TestCases/Players"
        TEST_CASES_BETTING_SCENARIOS="${CMAKE_CURRENT_SOURCE_DIR}/TestCases/BettingScenarios"
)

include(GoogleTest)
gtest_discover_tests(AllTests)