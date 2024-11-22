#pragma once
#include "DyPool.h"
#include <mutex>
class CDyPoolImpl : public IDyPool
{
public:
  CDyPoolImpl(size_t initial_threads, size_t max_threads, EVENT task = nullptr);
  virtual ~CDyPoolImpl();

public:
  virtual void Release();
public:
  virtual bool schedule(task_type const & task);

protected:
  void Event(const std::string& msg);

private:
  size_t m_max_threads;
  std::mutex m_mtx;
  EVENT m_event;
};

