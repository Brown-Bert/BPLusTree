#ifndef TREE_H
#define TREE_H
#include <math.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

using namespace std;

// B+树的申明
template <typename T>
class BPlusTree;

template <typename T>
int binarySearch(const vector<T>& v, T value, int flag);

// B+树节点声明
template <typename T>
class BPlusNode;

// 叶子节点
template <typename T>
class BPlusNodeLeaf;

// 非叶子节点
template <typename T>
class BPlusNodeNoLeaf;

// 测试输出vector容器的内容
template <typename T>
void printvector(T val) {
  cout << val << " ";
}

// 菜单显示以及操作类
template <typename T>
class Menu {
 private:
  int chioce;  // 菜单中选择的序号
  int degree;  // 构建的B+树的阶数
 public:
  // 构造函数
  Menu() = default;
  Menu(int degree) { this->degree = degree; }
  // 析构函数
  ~Menu() = default;
  // 展示菜单
  void showMenu() {
    cout << "**********" << this->degree << "阶B+树"
         << "**********" << endl;
    cout << "---1、查找" << endl;
    cout << "---2、插入" << endl;
    cout << "---3、修改" << endl;
    cout << "---4、删除" << endl;
    cout << "---5、退出" << endl;
  }
  // 获取选择
  int getChoice() { return chioce; }
  // 获取阶数
  int getDegree() { return degree; }
  // 设置阶数
  void setDegree(int degree) { this->degree = degree; }
  // 操作B+树
  void bPlusTreeOperator(BPlusTree<T>& tree) {
    T key;    // 关键字
    T value;  // 修改之后的关键字
    bool result;
    while (true) {
      this->showMenu();
      cout << "请输入你的选项：";
      cin >> this->chioce;
      if (this->chioce == 5) {
        break;
      }
      switch (this->chioce) {
        case 1:
          cout << "请输入要查找的关键字：";
          cin >> key;
          tree.findInBPlusTree(key);
          break;
        case 2:
          cout << "请输入要插入的关键字";
          cin >> key;
          tree.insertKey(key);

          tree.print02(tree.getRootPtr());
          // print01(tree.getFirstNodePtr());
          break;
        case 3:
          cout << "请输入要修改的关键字：";
          cin >> key;
          // 查找要修改的关键字，如果关键字不存在于B+树中，则直接输出不能修改，如果在B+树中查到了
          // 关键字，则再次要求用户输入修改之后的关键字，并进行修改（修改的本质就是删除key关键字，
          // 添加value关键字）
          result = tree.findInBPlusTree(key);
          if (result) {  // 表明找到了key关键字
            cout << "请输入修改之后的关键字：";
            cin >> value;
            // 1、删除关键字
            tree.deleteKeyInTree(tree.getRootPtr(), key);
            tree.insertKey(value);
          } else {
            cout << "该关键字不在B+树中，不能修改" << endl;
          }
          tree.print02(tree.getRootPtr());
          break;
        case 4:
          cout << "请输入要删除的关键字：";
          cin >> key;
          // 删除关键字之前也要先进行关键字的查找，如果关键字不存在，则是不能进行删除的；
          // 如果关键字在B+树中存在，则是可以进行删除的
          result = tree.findInBPlusTree(key);
          if (result) {  // 表明找到了key关键字，可以进行删除
            bool delete_res = tree.deleteKeyInTree(tree.getRootPtr(), key);
            if (delete_res) {
              cout << "删除成功" << endl;
            } else {
              cout << "删除失败" << endl;
            }
            tree.print02(tree.getRootPtr());
          } else {
            cout << "该关键字不在B+树中，不能删除" << endl;
          }
          break;
        default:
          cout << "输入范围是(1~5),输入不合法，重新输入" << endl;
          break;
      }
    }
  }
};
// B+树类，包含B+树的基本操作，例如增删改查
template <typename T>
class BPlusTree {
 private:
  mutex mtt;
  int min_num = 0;  // 存储结点关键字个数范围的下限
  int max_num = 0;  // 存储结点关键字个数范围的上限
  BPlusNode<T>* root_ptr = nullptr;        // 存储指向根结点的指针
  BPlusNode<T>* first_node_ptr = nullptr;  // 存储指向第一个结点的指针
 public:
  // 构造函数
  BPlusTree() = default;

  // 析构函数
  ~BPlusTree() {
    // 释放整棵树
    mtt.lock();
    // cout << root_ptr << endl;
    BPlusNode<T>* ptr = nullptr;
    queue<BPlusNode<T>*> q;
    q.push(root_ptr);
    while (!q.empty()) {
      ptr = q.front();
      q.pop();
      if (ptr->getFlag() == 0) {  // 非叶子结点
        vector<BPlusNode<T>*> v = ((BPlusNodeNoLeaf<T>*)ptr)->getNodePtrs();
        for (auto it = v.begin(); it != v.end(); it++) {
          q.push(*it);
        }
      }
      delete ptr;
      // cout << "删除" << endl;
    }
    mtt.unlock();
  }

  // 删除 树
  void deleteTree() {
    // 释放整棵树
    BPlusNode<T>* ptr = nullptr;
    queue<BPlusNode<T>*> q;
    q.push(std::move(root_ptr));
    while (!q.empty()) {
      ptr = std::move(q.front());
      q.pop();
      if (ptr->getFlag() == 0) {  // 非叶子结点
        vector<BPlusNode<T>*> v = ((BPlusNodeNoLeaf<T>*)ptr)->getNodePtrs();
        for (auto it = v.begin(); it != v.end(); it++) {
          q.push(std::move(*it));
        }
      }
      delete ptr;
    }
  }

  // 获取节点关键字范围下限
  int getMinNum() { return min_num; }

  // 获取节点关键字范围上限
  int getMaxNum() { return max_num; }

  // 获取指向根节点的指针
  BPlusNode<T>* getRootPtr() { return root_ptr; }

  // 获取指向第一个节点的指针
  BPlusNode<T>* getFirstNodePtr() { return first_node_ptr; }

  // 设置节点关键字范围下限
  void setMinNum(int num) { this->min_num = num; }

  // 设置节点关键字范围上限
  void setMaxNum(int num) { this->max_num = num; }

  // 设置指向根节点的指针
  void setRootPtr(BPlusNode<T>* ptr) { this->root_ptr = ptr; }

  // 设置指向第一个节点的指针
  void setFirstNodePtr(BPlusNode<T>* ptr) { this->first_node_ptr = ptr; }

  // 删除一个关键字
  bool deleteKeyInTree(BPlusNode<T>* ptr, T key) {
    ptr->deleteKey(key, min_num);
    return true;
  }

  // 初始化一棵树
  BPlusTree& initializeBPlusTree(int degree, string file_path_name, int flag,
                                 vector<T> v) {
    // 1、根据degree计算每个节点关键字范围
    //  min_num = (degree + 1) / 2;
    //  max_num = degree;

    min_num = ceil(degree * 1.0 / 2) - 1;
    max_num = degree - 1;
    if (flag == 0) {
      // 2、打开文件读取关键字
      ifstream ifs;
      ifs.open(file_path_name, ios::in);
      // if (!ifs.is_open()) {
      //     return false;
      // }
      string buf;
      while (getline(ifs, buf)) {
        // 把字符串按照空格分开存入V容器中
        int pos = buf.find(" ");
        while (pos != -1) {
          insertKey(stoi(buf.substr(0, pos)));
          buf = buf.substr(pos + 1);
          pos = buf.find(" ");
          // 插入关键字算法
        }
        // V.push_back(stoi(buf));
        insertKey(stoi(buf));
      }
      /*for_each(V.begin(), V.end(), printVector);
      cout << endl;*/
      ifs.close();
    } else {
      for (auto it = v.begin(); it != v.end(); it++) {
        insertKey(*it);
      }
    }
    return *this;
  }

  // 插入关键字算法
  void insertKey(T key) {
    if (root_ptr == nullptr) {  // 表明是一颗空树
      // 新new一个节点（叶子节点）
      BPlusNode<T>* node = new BPlusNodeLeaf<T>;
      root_ptr = node;
      first_node_ptr = node;
      node->insert(key);
      node->setFlag(1);
    } else {  // 不是一颗空树
      // 1、从根节点开始进行递归查找到叶节点，采用预分裂算法

      // 2、在递归查找叶节点的时候，在回溯过程的时候进行分裂

      pair<bool, pair<T, BPlusNode<T>*>> res = recallSplit(root_ptr, key);
      if (res.first) {  // 表明根节点需要分裂
        BPlusNodeNoLeaf<T>* node = new BPlusNodeNoLeaf<T>;
        node->setFlag(0);
        node->insert(res.second.first);
        node->putPtr(root_ptr);
        node->putPtr(res.second.second);
        root_ptr = node;
      }
    }
  }

  // 回溯分裂
  pair<bool, pair<T, BPlusNode<T>*>> recallSplit(BPlusNode<T>* root, T key) {
    BPlusNode<T>* ptr = root;
    int position = binarySearch<T>(ptr->getKeys(), key, 0);  // 先找到位置
    if (root->getFlag() == 1) {                  // 表明是叶子节点
      if ((ptr->getKeysSize() + 1) > max_num) {  // 表明当前叶子节点需要分裂
        BPlusNodeLeaf<T>* node = new BPlusNodeLeaf<T>;
        node->setFlag(1);
        // 1、先把关键字插入进去
        ptr->insertByPos(position, key);
        // 2、复制关键字以及设置指针
        node->putIntoNewNode((BPlusNodeLeaf<T>*)ptr);
        return make_pair(
            true, make_pair(ptr->getKeys()[ptr->getKeysSize() - 1], node));
      } else {  // 表明当前 不需要分裂，可以直接插入
        ptr->insert(key);
        return make_pair(false, make_pair(key, nullptr));
      }
    } else {  // 表明不是叶子节点，需要进行递归
      BPlusNodeNoLeaf<T>* p = (BPlusNodeNoLeaf<T>*)ptr;
      pair<bool, pair<T, BPlusNode<T>*>> res =
          recallSplit(p->getNodePtrs()[position], key);
      // if (root_ptr == p) return res;
      if (res.first) {  // 需要往父节点中插入
        // 1、先把关键字插入进去
        p->insertByPos(position, res.second.first);
        // 2、直接插入指针
        p->putPtr(res.second.second);
        // 3、分裂 (可能会产生分裂不一定一定会产生)
        if (p->getKeysSize() > max_num) {  // 分裂
          BPlusNodeNoLeaf<T>* node = new BPlusNodeNoLeaf<T>;
          T key = p->getKeys()[p->getKeysSize() / 2];
          node->setFlag(0);
          node->putIntoNewNode(p);
          return make_pair(true, make_pair(key, node));
        } else {  // 不分裂
          return make_pair(false, make_pair(key, nullptr));
        }
        // BPlusNodeNoLeaf<T>* node = new BPlusNodeNoLeaf<T>;
        // node->setFlag(0);
        // node->putIntoNewNode(p);
        // return make_pair(true, make_pair(p->getKeys()[p->getKeysSize() / 2],
        // ptr));
      } else {  // 不需要往父节点中插入
        return make_pair(false, make_pair(key, nullptr));
      }
    }
  }

  // 预分裂算法
  // void predictSplit(BPlusNode* root, T key){
  //     BPlusNode* ptr = root;
  //     if(root.getFlag() == 1){ // 表明是叶子节点
  //         if((ptr.getKeysSize() + 1) > max_num){ // 表明当前叶子节点需要分裂
  //             BPlusNode* node = new BPlusNodeLeaf<T>;

  //         }else{ // 当前叶子节点不需要分裂，可以直接插入
  //             ptr.insert(key);
  //         }
  //     }else{ // 非叶子节点
  //         // 判断当前节点是不是可以预先分裂，如果可以就分裂
  //         if ((ptr.getKeysSize() + 1) == max_num){ // 可以进行预分裂
  //             BPlusNode* node = new BPlusNodeNoLeaf<T>;
  //             node.setFlag(0);

  //         }else{ // 表明不用进行预分裂
  //             // 采用二分法，查找关键字在当前节点的位置
  //             int position = binarySearch(ptr.getKeys(), key);
  //             BPlusNodeNoLeaf* p = ptr;
  //             predictSplit(p.getNodePtrs()[position], key);
  //         }
  //     }
  // }

  // 测试输出函数 借助queue使用广度优先搜索B+树(层序遍历)
  void print02(BPlusNode<T>* root) {
    BPlusNode<T>* ptr;
    queue<BPlusNode<T>*> q;
    q.push(root);
    while (!q.empty()) {
      ptr = q.front();
      q.pop();
      if (ptr->getFlag() == 0) {  // 非叶子结点
        BPlusNodeNoLeaf<T>* p = (BPlusNodeNoLeaf<T>*)ptr;
        vector<BPlusNode<T>*> v = p->getNodePtrs();
        for (auto it = v.begin(); it != v.end(); it++) {
          q.push(*it);
        }
      }
      cout << "----------------------------------" << endl;
      cout << "结点的地址 = " << ptr << endl;
      cout << "size = " << ptr->getKeysSize() << endl;
      cout << "****node****" << endl;
      vector<T> v = ptr->getKeys();
      for_each(v.begin(), v.end(), printvector<T>);
      cout << endl;
      cout << "flag = " << ptr->getFlag() << endl;
      cout << "****nodeptr****" << endl;
      // BPlusNodeNoLeaf<T>* p = (BPlusNodeNoLeaf<T>*)ptr;
      // vector<BPlusNode<T>*> vptr = p->getNodePtrs();
      // for (auto it = vptr.begin(); it != vptr.end(); it++) {
      //     cout << (*it) << " " << endl;
      // }
      cout << endl;
      // cout << "父结点指针" << endl;
      // cout << ptr->getParent() << endl;
      // if (ptr->getParent() != nullptr) {
      //     cout << "父结点" << endl;
      //     vector<int> fv = ptr->getParent()->getNode();
      //     for_each(fv.begin(), fv.end(), printvector);
      //     cout << endl;
      // }
    }
  }

  // 通过根结点开始查找关键字所在结点，并返回指向关键字所在结点的指针
  BPlusNode<T>* getPositionOfKey(BPlusNode<T>* root_ptr, T key) {
    while (true) {
      if (root_ptr->getFlag() ==
          1) {  // 表明查找算法已经查找到叶子结点了，可以返回指向该结点的指针
        return root_ptr;
      }
      // 表明还没有找到相应的叶子结点，采用二分查找的算法找到要查询的下一个结点的指针所在的下标
      vector<T> v = root_ptr->getKeys();
      int position = binarySearch(v, key, 0);
      vector<BPlusNode<T>*> v_ptr =
          ((BPlusNodeNoLeaf<T>*)root_ptr)->getNodePtrs();
      root_ptr = v_ptr[position];
    }
  }

  // 查找算法
  bool findInBPlusTree(T key) {
    BPlusNode<T>* p = nullptr;
    vector<T> v;
    int res;
    p = this->getPositionOfKey(root_ptr, key);
    // 对该结点使用二分查找
    v = p->getKeys();
    res = binarySearch(v, key, 1);
    if (res == -1) {
      cout << "该B+树中不存在该关键字" << endl;
      return false;
    } else {
      cout << "该关键字所在结点的指针为：" << p << endl;
      cout << "该关键字所在结点的所有关键字为：";
      for (auto it = v.begin(); it != v.end(); it++) {
        cout << (*it) << " ";
      }
      cout << endl;
      return true;
    }
  }
};

// 二分查找算法，就是要在该节点中准确找到指向下一个结点的指针在当前结点指针vector里面的下标,
//因为多个地方使用二分查找算法，只是算法中有小地方改动，则使用flag手动控制，0表示走初始化的查找路线
// 1表示走初始化之后，B+树构建成功之后的路线
template <typename T>
int binarySearch(const vector<T>& v, T value, int flag) {
  int left_index = 0;              // 左边的下标
  int right_index = v.size() - 1;  // 右边的下标
  int mid_index;                   // 计算中间下标值
  while (left_index <= right_index) {
    mid_index = (left_index + right_index) / 2;
    if (v[mid_index] == value) {
      return mid_index;
    }
    if (v[mid_index] > value) {
      right_index = mid_index - 1;
    } else {
      left_index = mid_index + 1;
    }
  }
  if (flag == 0) {
    return left_index;
  } else {
    return -1;
  }
}

// 节点的基类
template <typename T>
class BPlusNode {
 protected:
  int flag;  // 标识是不是叶子节点，1代表叶子节点，0代表非叶子节点
  vector<T> keys;  // 每个节点的关键字
 public:
  // 析构函数
  ~BPlusNode() {
    // cout << "执行析构函数" << endl;
  }

  // 获取flag标识
  int getFlag() { return flag; }

  // 设置标识
  void setFlag(int flag) { this->flag = flag; }

  // 往keys中插入关键字
  void insert(T key) { keys.push_back(key); }

  // 获取节点关键字的个数
  size_t getKeysSize() { return keys.size(); }

  // 获取节点所有关键字
  vector<T> getKeys() { return keys; }

  // 按照给定的位置插入关键字
  void insertByPos(int pos, T key) { keys.insert(keys.begin() + pos, key); }

  // // 重新resize keys
  // virtual void resizeKeys();

  // 删除节点当中的关键字
  virtual void deleteKey(T key, T& min_num) = 0;

  // 拷贝右边的节点信息到左边
  virtual void copyRightToLeft(BPlusNode<T>* ptr, T key) = 0;
};

// 叶子节点
template <typename T>
class BPlusNodeLeaf : public BPlusNode<T> {
 private:
  BPlusNode<T>* pre = nullptr;   // 指向前一个节点指针
  BPlusNode<T>* next = nullptr;  // 指向后一个节点指针

 public:
  // 析构函数
  ~BPlusNodeLeaf() {}

  // 得到指向前一个节点的指针
  BPlusNode<T>* getPre() { return pre; }

  // 得到指向后一个节点的指针
  BPlusNode<T>* getNext() { return next; }

  // 设置指向前一个节点的指针
  void setPre(BPlusNodeLeaf<T>* ptr) { pre = ptr; }

  // 设置指向后一个节点的指针
  void setNext(BPlusNodeLeaf<T>* ptr) { next = ptr; }

  // 把关键字复制到新的节点中，并且设置指针
  void putIntoNewNode(BPlusNodeLeaf<T>* ptr) {
    vector<T> v = ptr->getKeys();
    BPlusNode<T>::keys.assign(v.begin() + (v.size() + 1) / 2, v.end());
    ptr->resizeKeys();
    pre = ptr;
    ptr->setNext(this);
  }

  // 重新resize keys
  void resizeKeys() {
    BPlusNode<T>::keys.resize((BPlusNode<T>::keys.size() + 1) / 2);
  }
  // 叶子节点直接删除，重载父类方法
  virtual void deleteKey(T key, T& min_num) override {
    // 1、查找删除关键字在当前节点的位置
    int position = binarySearch(BPlusNode<T>::keys, key, 0);
    BPlusNode<T>::keys.erase(BPlusNode<T>::keys.begin() + position);
  }

  // 拷贝右边的节点信息到左边
  virtual void copyRightToLeft(BPlusNode<T>* ptr, T key) override {
    // 向下转型
    BPlusNodeLeaf<T>* p = (BPlusNodeLeaf<T>*)ptr;
    // BPlusNode<T>::insert(key);
    vector<T> v = p->getKeys();
    for (auto it = v.begin(); it != v.end(); it++) {
      BPlusNode<T>::insert(*it);
    }
    // 叶子节点调整节点之间的指针
    next = p->getNext();
    if (p->getNext() != nullptr) {
      ((BPlusNodeLeaf<T>*)p->getNext())->setPre(this);
    }
  }
};

// 非叶子节点
template <typename T>
class BPlusNodeNoLeaf : public BPlusNode<T> {
 private:
  vector<BPlusNode<T>*> node_ptrs;

 public:
  // 析构函数
  ~BPlusNodeNoLeaf() {}

  // 获取所有指针
  vector<BPlusNode<T>*> getNodePtrs() { return node_ptrs; }

  // 往非叶子节点直接插入指针
  void putPtr(BPlusNode<T>* ptr) { node_ptrs.push_back(ptr); }

  // 删除指定位置的指针
  void deletePtrByPos(int pos) { node_ptrs.erase(node_ptrs.begin() + pos); }

  // 重新resize keys
  void resizeKeys() {
    BPlusNode<T>::keys.resize(BPlusNode<T>::keys.size() / 2);
  }

  // 重新resize ptrs
  void resizePtrs() { node_ptrs.resize((node_ptrs.size() + 1) / 2); }

  // 把当前节点的一半数据放到新的节点中
  void putIntoNewNode(BPlusNodeNoLeaf<T>* ptr) {
    vector<T> v_keys = ptr->getKeys();
    BPlusNode<T>::keys.assign(v_keys.begin() + v_keys.size() / 2 + 1,
                              v_keys.end());
    ptr->resizeKeys();
    vector<BPlusNode<T>*> v_ptrs = ptr->getNodePtrs();
    node_ptrs.assign(v_ptrs.begin() + (v_ptrs.size() + 1) / 2, v_ptrs.end());
    ptr->resizePtrs();
  }

  // 非叶子节点直接删除，重载父类方法
  virtual void deleteKey(T key, T& min_num) override {
    // 1、查找关键字在当前节点的位置
    int position = binarySearch(BPlusNode<T>::keys, key, 0);
    // 2、递归调用删除关键字的方法
    BPlusNode<T>* child_ptr = node_ptrs[position];
    child_ptr->deleteKey(key, min_num);
    // 判断子节点的删除对父节点是不是会产生影响
    if (child_ptr->getKeysSize() < min_num) {  // 子节点需要借或者合并
      if (position != 0) {                     // 表明有左兄弟
        BPlusNode<T>* leftbro_ptr = node_ptrs[position - 1];
        if (leftbro_ptr->getKeysSize() == min_num) {
          // 表明不够借，可以不用看有没有右兄弟，直接和左兄弟合并
          // 先把右边的节点信息全部拷贝到左边
          leftbro_ptr->copyRightToLeft(child_ptr,
                                       BPlusNode<T>::keys[position - 1]);
          BPlusNode<T>::keys.erase(BPlusNode<T>::keys.begin() + position - 1);
          node_ptrs.erase(node_ptrs.begin() + position);
          // 清除废掉的节点
          delete child_ptr;
          child_ptr = nullptr;
        } else {
          // 够借可以借
          child_ptr->insert(BPlusNode<T>::keys[position]);
          BPlusNode<T>::keys.erase(BPlusNode<T>::keys.begin() + position);
          BPlusNode<T>::insertByPos(
              position, leftbro_ptr->getKeys()[leftbro_ptr->getKeysSize() - 1]);
          // 判断是不是叶子节点
          if (child_ptr->getFlag() == 0) {  // 不是叶子节点
            ((BPlusNodeNoLeaf<T>*)child_ptr)
                ->putPtr(((BPlusNodeNoLeaf<T>*)leftbro_ptr)
                             ->getNodePtrs()[leftbro_ptr->getKeysSize() - 1]);
            ((BPlusNodeNoLeaf<T>*)leftbro_ptr)
                ->deletePtrByPos(leftbro_ptr->getKeysSize() - 1);
          }
        }
      } else {  // 表明没有左兄弟，那么肯定有右兄弟
        BPlusNode<T>* rightbro_ptr = node_ptrs[position + 1];
        if (rightbro_ptr->getKeysSize() == min_num) {
          // 右边兄弟借不了，直接合并
          child_ptr->copyRightToLeft(rightbro_ptr,
                                     BPlusNode<T>::keys[position]);
          BPlusNode<T>::keys.erase(BPlusNode<T>::keys.begin() + position - 1);
          node_ptrs.erase(node_ptrs.begin() + position + 1);
          // 清除废掉的节点
          delete rightbro_ptr;
          rightbro_ptr = nullptr;
        } else {
          // 右边兄弟可以借
          child_ptr->insert(BPlusNode<T>::keys[position]);
          BPlusNode<T>::keys.erase(BPlusNode<T>::keys.begin() + position);
          BPlusNode<T>::insertByPos(position, rightbro_ptr->getKeys()[0]);
          // 判断是不是叶子节点
          if (child_ptr->getFlag() == 0) {  // 不是叶子节点
            ((BPlusNodeNoLeaf<T>*)child_ptr)
                ->putPtr(((BPlusNodeNoLeaf<T>*)rightbro_ptr)->getNodePtrs()[0]);
            ((BPlusNodeNoLeaf<T>*)rightbro_ptr)->deletePtrByPos(0);
          }
        }
      }
    }
    // BPlusNode<T>::keys.erase();
  }

  // 拷贝右边的节点信息到左边
  virtual void copyRightToLeft(BPlusNode<T>* ptr, T key) override {
    // 向下转型
    BPlusNodeNoLeaf<T>* p = (BPlusNodeNoLeaf<T>*)ptr;
    BPlusNode<T>::insert(key);
    for (auto it = p->getKeys().begin(); it != p->getKeys().end(); it++) {
      BPlusNode<T>::insert(*it);
    }
    for (auto it = p->getNodePtrs().begin(); it != p->getNodePtrs().end();
         it++) {
      putPtr(*it);
    }
  }
};

// 加法
int add(int val1, int val2);

// 减法
int sub(int val1, int val2);
#endif