#ifndef __CLY_SERVER_H__
#define __CLY_SERVER_H__
#include <string.h>  
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include "io_service_pool.h"

class Session;

class Server {

private:
	// �Ự - ����ָ��
	typedef	boost::shared_ptr<Session>	session_ptr;
public:
	//Server(boost::asio::io_service &_ioService, boost::asio::ip::tcp::endpoint &_endpoint);
	Server(int port, int thread_cnt);
	virtual ~Server(void);
	// ����
	void start(void);
	// �첽
	void run();

	void stop();
	static void threadstart();
	static void thread_end();
private:
	// ���ݵ����ӿ�
	void callback_session(std::string _fromIp, std::string _info);
	// �Ự����
	void accept_handler(session_ptr _chatSession, const boost::system::error_code& _error);
	
private:
	//boost::asio::io_service &ioService_;
	io_service_pool						io_service_pool_;
	boost::asio::ip::tcp::acceptor		acceptor_;
	int									serial_num_;
	boost::shared_ptr<io_service_pool>	io_service_ptr;
};
#endif // __CLY_SERVER_H__
