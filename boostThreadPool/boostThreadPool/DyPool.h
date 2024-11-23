#pragma once
#include "boost/threadpool.hpp"
#include <string>
#include <functional>

/*! \ժҪ����չthreadpool.
  *
  * ��threadpool�����ϣ�������1. ָ����ʼ�߳�����������߳�����; 2. �߳�ָ����̬��չ
  *
  * \���� ��
  *
  * \ע�� ��
  *
  * \�ο� ��
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

