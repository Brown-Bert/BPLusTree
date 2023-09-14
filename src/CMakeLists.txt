add_library(treecpp STATIC tree.cpp)

target_include_directories(treecpp PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(treeMain testMain.cpp)

target_link_libraries(treeMain PRIVATE treecpp)

target_include_directories(treeMain PRIVATE ${CMAKE_SOURCE_DIR}/include)
