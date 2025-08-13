add_library(MyImpl
    ${CMAKE_CURRENT_SOURCE_DIR}/TableState/TableState.cpp
)

target_link_libraries(MyImpl
    PRIVATE
        MyHeaders
)