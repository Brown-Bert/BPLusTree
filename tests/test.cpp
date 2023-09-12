#include <gtest/gtest.h>

#include <iostream>

#include "tree.h"
using namespace std;

TEST(ADDTEST, ADD) { EXPECT_EQ(5, add(2, 3)); }

TEST(SUBTEST, SUB) { EXPECT_EQ(1, sub(3, 2)); }

class MyEnvironment : public testing::Environment {
 public:
  virtual void SetUp() { cout << "Global event : start" << endl; }
  virtual void TearDown() { cout << "Global event : end" << endl; }
};

class testsuite : public ::testing::Test {
 public:
  static void SetUpTestCase() {
    cout << "SetUpTestCase()" << endl;  // 测试套件的初始环境搭建
  }

  static void TearDownTestCase() {
    cout << "TearDownTestCase()" << endl;  // 测试套件的环境销毁
  }

  virtual void SetUp() {
    cout << "SetUp()" << endl;  //测试实例的初始环境搭建
  }
  virtual void TearDown() {
    cout << "TearDown()" << endl;  //测试实例的环境销毁
  }
};

TEST_F(testsuite, add) { cout << add(3, 2) << endl; }

TEST_F(testsuite, sub) { cout << sub(3, 2) << endl; }

int main(int argc, char* argv[]) {
  testing::AddGlobalTestEnvironment(new MyEnvironment);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}