#include "io_service_pool.h"

int io_service_pool::create_io_service()
{
	io_service_sptr io_service(new boost::asio::io_service);
	work_sptr work(new boost::asio::io_service::work(*io_service));
	m_arr_io_services.push_back(io_service);
	m_arr_work.push_back(work);
	boost::shared_ptr<boost::thread> thread(new boost::thread(
		boost::bind(&boost::asio::io_service::run, m_arr_io_services[m_nNext_io_service])));
	m_arr_thread_status.push_back(1);
	m_arr_threads.push_back(thread);
	return m_nNext_io_service;
}

boost::asio::io_service& io_service_pool::get_io_service( int &serial_num )
{
	boost::mutex::scoped_lock lock(m_mtx);

	if (m_nNext_io_service == m_arr_io_services.size())
	{
		int sn = checkIdleIOService();
		if (sn < 0)
		{
			//新增线程
			create_io_service();
			sn = m_nNext_io_service;
			++m_nNext_io_service;
		}

		boost::asio::io_service&  io_service = *m_arr_io_services[sn];
		serial_num = sn;
		return io_service;
	}
	else
	{
		boost::asio::io_service& io_service = *m_arr_io_services[m_nNext_io_service];
		m_arr_thread_status[m_nNext_io_service] = 1;
		serial_num = m_nNext_io_service;
		++ m_nNext_io_service;
		return io_service;
	}
	return *(boost::asio::io_service*)NULL;
}

int io_service_pool::checkIdleIOService()
{
	for (int i = 0; i < m_arr_thread_status.size(); i++)
	{
		if (!m_arr_thread_status.at(i))
		{
			m_arr_thread_status[i] = 1;
			return i;
		}
	}
	return -1;	
}

void io_service_pool::threadQuit(int thread_num)
{
	boost::mutex::scoped_lock lock(m_mtx);
	std::cout<<"thread quit:"<<thread_num<<","<<boost::this_thread::get_id()<<std::endl;
	m_arr_thread_status[thread_num] = 0;
}
