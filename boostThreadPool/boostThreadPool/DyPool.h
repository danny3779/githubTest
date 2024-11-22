#pragma once
#include "boost/threadpool.hpp"
#include <string>
#include <functional>

/*! \摘要：扩展threadpool.
  *
  * 在threadpool基础上，增加了1. 指定初始线程数量及最大线程数量; 2. 线程指数动态扩展
  *
  * \参数 略
  *
  * \注意 无
  *
  * \参考 略
*/
class IDyPool : public boost::threadpool::pool
{
public:
  using EVENT = std::function<void(std::string)>;
public:
  IDyPool(size_t initial_threads);
public:
  static IDyPool* CreateApi(size_t initial_threads, size_t max_threads, EVENT task = nullptr);
  virtual void Release() = 0;
public:
  virtual bool schedule(task_type const & task);
};

