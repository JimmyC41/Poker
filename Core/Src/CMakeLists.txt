add_library(MyImpl
    ${CMAKE_CURRENT_SOURCE_DIR}/TableState/Handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TableState/TableState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TableState/Helper.cpp
)

target_link_libraries(MyImpl
    PRIVATE
        MyHeaders
)