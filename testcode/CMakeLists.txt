# 添加测试用例
add_executable(test_add test_add.cpp)

# 链接gtest库
find_package(GTest REQUIRED)
target_link_libraries(test_add GTest::GTest GTest::Main)

# 添加测试
add_test(NAME test_add COMMAND test_add)


