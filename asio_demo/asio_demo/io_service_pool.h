#ifndef IO_SERVICE_POOL_H
#define IO_SERVICE_POOL_H

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>

// 不可拷贝和复制
class io_service_pool
	: public boost::noncopyable
{
public:

	explicit io_service_pool(std::size_t pool_size)
		: m_nNext_io_service(0)
	{ 
		for (std::size_t i = 0; i < pool_size; ++ i)
		{
			io_service_sptr io_service(new boost::asio::io_service);
			work_sptr work(new boost::asio::io_service::work(*io_service));
			m_arr_io_services.push_back(io_service);
			m_arr_work.push_back(work);
		}
		for (std::size_t i = 0; i < m_arr_io_services.size(); ++ i)
		{
			boost::shared_ptr<boost::thread> thread(new boost::thread(
				boost::bind(&boost::asio::io_service::run, m_arr_io_services[i])));
			m_arr_thread_status.push_back(0);
			m_arr_threads.push_back(thread);
		}
	}

	void start()
	{ 
		
	}

	void join()
	{
		for (std::size_t i = 0; i < m_arr_threads.size(); ++ i)
		{
			m_arr_threads[i]->join();
		} 
	}

	void stop()
	{ 
		for (std::size_t i = 0; i < m_arr_io_services.size(); ++ i)
		{
			m_arr_io_services[i]->stop();
		}
	}

	
	boost::asio::io_service& get_io_service(int &serial_num);
	
	void threadQuit(int thread_num);

private:
	int create_io_service();

	int checkIdleIOService();

	
private:
	typedef boost::shared_ptr<boost::asio::io_service> io_service_sptr;
	typedef boost::shared_ptr<boost::asio::io_service::work> work_sptr;
	typedef boost::shared_ptr<boost::thread> thread_sptr;

	boost::mutex										m_mtx;

  // ios容器
	std::vector<io_service_sptr>						m_arr_io_services;
  // 工作线程容器
	std::vector<work_sptr>								m_arr_work;
  // 线程容器
	std::vector<thread_sptr>							m_arr_threads; 
 // // 线程状态映射表
	//std::map<std::string, bool>							m_map_thread_status;

  // ios线程使用状态。 0: 未使用；1: 使用
	std::vector<int>									m_arr_thread_status;

  // 最大ios的索引index
	std::size_t											m_nNext_io_service;
};

#endif
