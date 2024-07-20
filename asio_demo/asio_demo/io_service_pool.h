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
		: next_io_service_(0)
	{ 
		for (std::size_t i = 0; i < pool_size; ++ i)
		{
			io_service_sptr io_service(new boost::asio::io_service);
			work_sptr work(new boost::asio::io_service::work(*io_service));
			io_services_.push_back(io_service);
			work_.push_back(work);
		}
		for (std::size_t i = 0; i < io_services_.size(); ++ i)
		{
			boost::shared_ptr<boost::thread> thread(new boost::thread(
				boost::bind(&boost::asio::io_service::run, io_services_[i])));
			thread_status_vector_.push_back(0);
			threads_.push_back(thread);
		}
	}

	void start()
	{ 
		
	}

	void join()
	{
		for (std::size_t i = 0; i < threads_.size(); ++ i)
		{
			threads_[i]->join();
		} 
	}

	void stop()
	{ 
		for (std::size_t i = 0; i < io_services_.size(); ++ i)
		{
			io_services_[i]->stop();
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

	boost::mutex										mtx;

	std::vector<io_service_sptr>						io_services_;
	std::vector<work_sptr>								work_;
	std::vector<thread_sptr>							threads_; 
	std::map<std::string, bool>							thread_status_map_;
	std::vector<int>									thread_status_vector_;
	std::size_t											next_io_service_;
};

#endif
