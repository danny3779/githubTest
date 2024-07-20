// asio_demo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"


#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "Server.h"

int main(void) {
	try {
		std::cout << "server start." << std::endl;
		// ����������
		//boost::asio::io_service ios;
		// ����ķ�������ַ��˿�
		//boost::asio::ip::tcp::endpoint endpotion(boost::asio::ip::tcp::v4(), 13695);
		// ����Serverʵ��
		Server server(12345, 2);//ios, endpotion);
		// �����첽�����¼�����ѭ��
		//server.run();
		boost::shared_ptr<boost::thread> t(new boost::thread(boost::bind(&Server::run, &server)));
		//t->join();
		while (true)
		{
			boost::thread::sleep(boost::get_system_time() + boost::posix_time::seconds(10));
		}
		server.stop();
	}
	catch (std::exception& _e) {
		std::cout << _e.what() << std::endl;
	}
	std::cout << "server end." << std::endl;
	
	return 0;
}
