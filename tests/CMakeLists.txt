cmake_minimum_required(VERSION 3.0.0)
project(test)

AUX_SOURCE_DIRECTORY("../src" src)
AUX_SOURCE_DIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}" test_src)

add_executable(${PROJECT_NAME} ${src} ${test_src})

# find_package(GTest REQUIRED)
# target_link_libraries(test_add GTest::GTest GTest::Main)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/googletest-main/googletest/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/googletest-main/googlemock/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    gtest
    gtest_main
)
# add_dependencies(${PROJECT_NAME} gtest gtest_main)
