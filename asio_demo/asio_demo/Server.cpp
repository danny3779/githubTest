#include "Server.h"


#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

#include "Session.h"


using boost::asio::ip::tcp;

Server::Server(int port, int thread_cnt)//boost::asio::io_service &_ioService, boost::asio::ip::tcp::endpoint &_endpoint)
	: io_service_pool_(thread_cnt)
	, serial_num_(0)
	//, io_service_ptr(new io_service_pool(thread_cnt))
	, acceptor_(io_service_pool_.get_io_service(serial_num_), tcp::endpoint(tcp::v4(), port))
	//: ioService_(_ioService),
	//acceptor_(_ioService, _endpoint) 
{
	int n = 0;
	//io_service_ptr = boost::make_shared<io_service_pool>(&io_service_pool_);//new io_service_pool(thread_cnt));
	//acceptor_ = boost::asio::ip::tcp::acceptor(io_service_pool_.get_io_service(n), tcp::endpoint(tcp::v4(), port));
	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	std::cout<<"server thread id:"<<boost::this_thread::get_id()<<std::endl;
	start();
}


Server::~Server(void) {
}

void Server::start(void) 
{
	//session*	new_chat_session(new Session(ioService_));
	int serial_num = -1;
	boost::asio::io_service & tmp_io_service = io_service_pool_.get_io_service(serial_num);
	//session_ptr new_chat_session(new Session(io_service_pool_.get_io_service()));
	session_ptr new_chat_session(new Session(tmp_io_service, serial_num, &io_service_pool_));
  //m_arrSessions.push_back(new_chat_session);

	acceptor_.async_accept(new_chat_session->socket(),
		boost::bind(&Server::accept_handler, this, new_chat_session,
		boost::asio::placeholders::error));
}

void Server::run() 
{
	io_service_pool_.start();
	//boost::thread::sleep(boost::get_system_time() + boost::posix_time::seconds(10));
	io_service_pool_.join();
	//io_service_pool_.stop();
}

void Server::stop()
{
	io_service_pool_.stop();
}

void Server::callback_session(std::string /*_fromIp*/, std::string /*_info*/) 
{
	return;
}

void Server::accept_handler(session_ptr _chatSession, const boost::system::error_code& _error) {
	if (!_error && _chatSession) {
		try {
      m_arrSessions.push_back(_chatSession);
			_chatSession->start();
			start();
		}
		catch (...) {
			return;
		}
	}
}

void Server::threadstart()
{
	std::cout<<"thread start"<<std::endl;
}

void Server::thread_end()
{

}

void Server::SendData(const std::string& msg)
{
  for(auto i = 0; i < m_arrSessions.size(); ++i)
    m_arrSessions[i]->SendData(msg);
}
