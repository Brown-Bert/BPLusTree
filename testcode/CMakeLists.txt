# 添加测试用例
add_executable(test_add testadd.cpp)

# 链接gtest库
target_include_directories(test_add PUBLIC ${CMAKE_SOURCE_DIR}/googletest-main/googletest/include)

target_include_directories(test_add PUBLIC ${CMAKE_SOURCE_DIR}/include)

# 添加测试
add_test(NAME test_add COMMAND test_add)


