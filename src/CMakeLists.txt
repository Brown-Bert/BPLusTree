add_library(treecpp STATIC tree.cpp)

target_include_directories(treecpp PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(test testMain.cpp)

target_link_libraries(test PRIVATE treecpp)

target_include_directories(test PRIVATE ${CMAKE_SOURCE_DIR}/include)