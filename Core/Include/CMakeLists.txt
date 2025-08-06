add_library(MyHeaders INTERFACE)

# Any target that links to this will also get these include directories
target_include_directories(MyHeaders
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)