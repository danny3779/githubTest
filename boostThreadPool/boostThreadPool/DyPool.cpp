#include "DyPool.h"
#include <iostream>
#include "DyPoolImpl.h"

IDyPool::IDyPool(size_t initial_threads)
  : boost::threadpool::pool(initial_threads)
{

}

IDyPool* IDyPool::CreateApi(size_t initial_threads, size_t max_threads, EVENT task /*= nullptr*/)
{
  return new CDyPoolImpl(initial_threads, max_threads, task);
}

bool IDyPool::schedule(task_type const & task)
{
  return __super::schedule(task);
}