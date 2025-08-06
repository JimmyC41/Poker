cmake_minimum_required(VERSION 4.0.3)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(PokerCore
    LANGUAGES CXX
)

add_subdirectory(Src)
add_subdirectory(Include)

enable_testing()
add_subdirectory(Test)