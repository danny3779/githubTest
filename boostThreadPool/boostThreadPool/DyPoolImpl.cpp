#include "DyPoolImpl.h"
using namespace boost::threadpool;
using namespace std;

CDyPoolImpl::CDyPoolImpl(size_t initial_threads, size_t max_threads, EVENT event /*= nullptr*/)
  : IDyPool(initial_threads), m_max_threads(max_threads), m_event(event)
{
}

CDyPoolImpl::~CDyPoolImpl()
{
}

void CDyPoolImpl::Release()
{
  delete this;
}

bool CDyPoolImpl::schedule(task_type const & task)
{
  std::lock_guard<std::mutex> lock(m_mtx);
  auto curr_size = size();
  auto curr_active = active();
  if (curr_active >= curr_size && curr_size < m_max_threads)
  {
    //size_t newSize(curr_size * 2);  //  每次扩充为 2 倍
    size_t newSize(curr_size + 10);   //  每次扩充10个线程
    bool max(false);
    if (newSize > m_max_threads)
    {
      newSize = m_max_threads;
      max = true;
    }

    size_controller().resize(newSize);

    {
      stringstream ss;
      ss << "resize-to:" << size();
      if (max)
        ss << ". max!";
      Event(ss.str());
    }
  }
  return __super::schedule(task);
}

void CDyPoolImpl::Event(const std::string& msg)
{
  if (m_event)
    m_event(msg);
}
