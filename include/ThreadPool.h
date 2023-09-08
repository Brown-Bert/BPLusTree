#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

using namespace std;

class MyThreadPool;
static MyThreadPool* ptr = nullptr;
static once_flag once;

class MyThreadPool {
 private:
  vector<thread> Threads;
  queue<function<void()>> tasks_queue;
  int count = 0;
  mutex mtx;
  mutex mt;
  condition_variable condition;
  bool stop;

 public:
  MyThreadPool(int numThread) : stop(false) {
    for (int i = 0; i < numThread; i++) {
      Threads.emplace_back([this] {
        while (1) {
          // cout << "123" <<endl;
          unique_lock<mutex> lock(mtx);
          // cout << stop << endl;
          condition.wait(lock,
                         [this] { return (!tasks_queue.empty()) || stop; });
          // condition.wait_for(lock, chrono::seconds(3), [this]{
          //     return (!tasks_queue.empty()) || stop;
          // });
          if (stop && tasks_queue.empty()) {
            return;
          }
          // cout << "sddf"<< count <<endl;
          function<void()> task = move(tasks_queue.front());
          tasks_queue.pop();
          // count++;
          lock.unlock();
          task();
          // {
          //     unique_lock<mutex> lk(mt);
          //     count--;
          // }
          // count--;
        }
      });
    }
  }
  // void threadJoin(){
  //     cout << Threads.size() <<endl;
  //     for (auto& t: Threads){
  //         t.join();
  //     }
  // }
  ~MyThreadPool() {
    {
      unique_lock<mutex> lock(mtx);
      stop = true;
    }
    // cout << "执行析构函数" << endl;
    // 当线程比程序多的时候，很有可能有些线程没有被唤醒
    for (int i = 0; i < Threads.size(); i++) {
      condition.notify_one();
    }
    for (auto& t : Threads) {
      t.join();
    }
  }
  template <class F, class... Args>
  void enqueue(F&& f, Args&&... args) {
    function<void()> task = bind(forward<F>(f), forward<Args>(args)...);
    {
      unique_lock<mutex> lock(mtx);
      // condition.wait(lock, [this]{
      //     return tasks_queue.size() < 10;
      // });
      tasks_queue.emplace(task);
    }
    condition.notify_one();
  }
  // MyThreadPool(const MyThreadPool& myThreadPool) = delete;
  // MyThreadPool& operator=(const MyThreadPool& myThreadPool) = delete;

  // static void getInstance(int numThread){
  //     call_once(once, init, numThread);

  // }

  // static void init(int numThread){
  //     if(ptr == nullptr){
  //         // cout<<numThread<<endl;
  //         ptr = new MyThreadPool(numThread);
  //     }
  // }
};
#endif

// void test(){
//     MyThreadPool pool(4);
//     for (int i = 0; i < 10; i++) {
//         pool.enqueue([i]{
//             mutex m;
//             m.lock();
//             cout << "task : " << i << " is running" << endl;
//             m.unlock();
//             this_thread::sleep_for(chrono::seconds(1));
//             m.lock();
//             cout << "task : " << i << " is done" << endl;
//             m.unlock();
//         });
//     }
// }

// int main(){
//     {
//         MyThreadPool pool(4);
//         for (int i = 0; i < 10; i++) {
//             pool.enqueue([i]{
//                 mutex m;
//                 m.lock();
//                 cout << "task : " << i << " is running" << endl;
//                 m.unlock();
//                 this_thread::sleep_for(chrono::seconds(1));
//                 m.lock();
//                 cout << "task : " << i << " is done" << endl;
//                 m.unlock();
//             });
//         }
//     }
//     cout << "over" << endl;
//     return 0;
// }