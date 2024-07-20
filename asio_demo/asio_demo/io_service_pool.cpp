#include "io_service_pool.h"

int io_service_pool::create_io_service()
{
	io_service_sptr io_service(new boost::asio::io_service);
	work_sptr work(new boost::asio::io_service::work(*io_service));
	io_services_.push_back(io_service);
	work_.push_back(work);
	boost::shared_ptr<boost::thread> thread(new boost::thread(
		boost::bind(&boost::asio::io_service::run, io_services_[next_io_service_])));
	thread_status_vector_.push_back(1);
	threads_.push_back(thread);
	return next_io_service_;
}

boost::asio::io_service& io_service_pool::get_io_service( int &serial_num )
{
	boost::mutex::scoped_lock lock(mtx);

	if (next_io_service_ == io_services_.size())
	{
		int sn = checkIdleIOService();
		if (sn < 0)
		{
			//新增线程
			create_io_service();
			sn = next_io_service_;
			++next_io_service_;
		}

		boost::asio::io_service&  io_service = *io_services_[sn];
		serial_num = sn;
		return io_service;
	}
	else
	{
		boost::asio::io_service& io_service = *io_services_[next_io_service_];
		thread_status_vector_[next_io_service_] = 1;
		serial_num = next_io_service_;
		++ next_io_service_;
		return io_service;
	}
	return *(boost::asio::io_service*)NULL;
}

int io_service_pool::checkIdleIOService()
{
	for (int i = 0; i < thread_status_vector_.size(); i++)
	{
		if (!thread_status_vector_.at(i))
		{
			thread_status_vector_[i] = 1;
			return i;
		}
	}
	return -1;	
}

void io_service_pool::threadQuit(int thread_num)
{
	boost::mutex::scoped_lock lock(mtx);
	std::cout<<"thread quit:"<<thread_num<<","<<boost::this_thread::get_id()<<std::endl;
	thread_status_vector_[thread_num] = 0;
}
