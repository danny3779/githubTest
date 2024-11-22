#include "iostream"
#include "vector"
#include <string>
#include "boost/threadpool.hpp"
#include <mutex>
#include "DyPool.h"

using namespace std;
//using namespace boost::threadpool;

#define THREAD_COUNT_INIT		4			// 线程池的线程最大个数
#define THREAD_COUNT_MAX		500			// 线程池的线程最大个数
#define SLEEP_SECOND		5		// 每一个进程结束任务退出时，需要等待的时长
std::mutex gLock;

// 每个线程的任务
void Task(const string& msg, int index)
{
  // sleep for a short time
  std::this_thread::sleep_for(std::chrono::seconds(SLEEP_SECOND));

  std::lock_guard<std::mutex> lock(gLock);
  cout << msg << endl;
}

void Output(string prefix, string msg)
{
  cout << prefix << ", " << msg << endl;
}

class MyClass
{
public:
  void Output(string prefix, string msg)
  {
    cout << prefix << ", " << msg << endl;
  }
};

// 给线程池中的每一个线程分配任务，schedule会先把全部的任务接下来，如果没有线程空闲，则自动等待；wait遇到还有线程没有结束任务，那么其会进行阻塞等待
void ExecuteWithThreadpool()
{
  MyClass my;
  //设置允许开启的线程数
  //IDyPool* tp = IDyPool::CreateApi(THREAD_COUNT_INIT, THREAD_COUNT_MAX, std::bind(&Output, "prefix", std::placeholders::_1 ));
  IDyPool* tp = IDyPool::CreateApi(THREAD_COUNT_INIT, THREAD_COUNT_MAX, std::bind(&MyClass::Output, &my, "prefix", std::placeholders::_1));
  vector<std::string> v;
  for(auto i = 1; i <= 100; i++)
  {
    stringstream ss;
    ss << i;
    v.push_back(ss.str());
  }

  int index = 0;
  for(const auto& val : v)
  {
    // 传入线程中作为参数
    tp->schedule(boost::bind(Task, val, index));
  }
  tp->wait();
  cout << "end." << endl;
  tp->Release();
  cout << "release." << endl;
}

int main()
{
  double start, end;
  start = clock();
  ExecuteWithThreadpool();
  end = clock();
  printf("ExecuteWithThreadpool:%fs \n", (end - start) / 1000);

  while (true)
  {
    getchar();
  }
  
  return 0;
}