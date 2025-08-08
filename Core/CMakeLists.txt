cmake_minimum_required(VERSION 4.0.3)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(PokerCore
    LANGUAGES CXX
)

add_executable(DummyMain
    main.cpp
)

add_subdirectory(Include)

target_link_libraries(DummyMain
    PRIVATE
        MyHeaders
)

enable_testing()
add_subdirectory(Test)