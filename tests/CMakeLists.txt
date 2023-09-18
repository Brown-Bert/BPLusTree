cmake_minimum_required(VERSION 3.0.0)
project(testTree)

# find_package(GTest REQUIRED)  

AUX_SOURCE_DIRECTORY("${CMAKE_SOURCE_DIR}/src" SRCS)
AUX_SOURCE_DIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}" TEST_SRCS)

add_executable(${PROJECT_NAME} ${SRCS} ${TEST_SRCS})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/googletest-main/googletest/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/googletest-main/googlemock/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)


target_link_libraries(${PROJECT_NAME} gtest gtest_main)


add_dependencies(${PROJECT_NAME} gtest gtest_main)
add_test(NAME test_add COMMAND test_add)
