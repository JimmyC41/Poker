add_library(MyCore
    Card.cpp
)

target_link_libraries(MyCore
    PRIVATE
        MyHeaders
)