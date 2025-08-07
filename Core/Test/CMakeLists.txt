cmake_minimum_required(VERSION 4.0.3)
enable_testing()

find_package(GTest REQUIRED)

add_executable(AllTests
    CardTest.cpp
    DeckTest.cpp
)

target_link_libraries(AllTests
    PRIVATE
        MyCore
        MyHeaders
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(AllTests)