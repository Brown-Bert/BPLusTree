#include "ThreadPool.h"
#include "tree.h"
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

using namespace std;

void test(vector<double> &datas, int degree, vector<int> v);

// 读取文件的函数
vector<int> readFile(string file_path_name) {
  ifstream ifs;
  ifs.open(file_path_name, ios::in);
  // if (!ifs.is_open()) {
  //     return false;
  // }
  vector<int> v;
  string buf;
  while (getline(ifs, buf)) {
    // 把字符串按照空格分开存入V容器中
    int pos = buf.find(" ");
    while (pos != -1) {
      v.push_back(stoi(buf.substr(0, pos)));
      buf = buf.substr(pos + 1);
      pos = buf.find(" ");
    }
    v.push_back(stoi(buf));
  }
  ifs.close();
  return v;
}

// int main(int argc, char** argv){

//     string file_path_name = "../doc/data100.txt";
//     ofstream ofs;
// 	ofs.open(file_path_name, ios::out);
// 	if (!ofs.is_open()) {
// 		return false;
// 	}
//     int sum = 3;
// 	vector<double> datas;
//     vector<int> v = readFile("../doc/B+treeCopy.txt");
// 	datas.resize(sum - 2);
// 	{
// 		int  i = 2;
// 		MyThreadPool pool(300);
// 		for(;i < sum;){
// 			i++;
// 			pool.enqueue(test, ref(datas), i, v);
// 		}
// 	}
//     for(vector<double>::iterator it = datas.begin(); it != datas.end(); it++)
//     {
// 		cout << *it << endl;
// 		ofs  << *it << "\n";
// 	}
//     return 0;
// }
mutex mtx;
void test(vector<double> &datas, int degree, vector<int> v) {
  BPlusTree<int> tree;
  Menu<int> m;
  m.setDegree(degree);
  auto start = chrono::steady_clock::now();
  tree.initializeBPlusTree(m.getDegree(), "../doc/B+tree10.txt", 1, v);
  auto end = chrono::steady_clock::now();
  auto t = chrono::duration_cast<chrono::milliseconds>(end - start).count();
  cout << "初始化B+树的运行时间为： " << t << " ms" << endl;
  {
    unique_lock<mutex> lock(mtx);
    datas[degree - 3] = t;
  }
  // unique_lock<mutex> lk2(mtt);
  // tree.deleteTree();
  // lk2.unlock();
  // BPlusNodeNoLeaf<int>* p = (BPlusNodeNoLeaf<int>*) tree.getRootPtr();
  // tree.print02(tree.getRootPtr());
  // m.bPlusTreeOperator(tree);
}