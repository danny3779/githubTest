#include <boost/bind.hpp>

#include "Session.h"
#include "Server.h"

#include <boost/thread/thread.hpp>

Session::Session(boost::asio::io_service& _ioService, int sn, io_service_pool *_io_service_pool)
	: socket_(_ioService)
	, serial_num_(sn)
	, io_service_pool_ptr_(_io_service_pool)
{
	memset(msg_, 0, sizeof(msg_));
	
	thread_quit_connecttion_ = sig.connect(boost::bind(&io_service_pool::threadQuit, io_service_pool_ptr_, _1));
}


Session::~Session(void)
{
	if (thread_quit_connecttion_.connected())
	{
		thread_quit_connecttion_.disconnect();
	}
	std::cout<<"thread quit:"<<boost::this_thread::get_id()<<std::endl;
}


void Session::start(void) 
{
	const boost::system::error_code err;
	init_handler(err);
}

boost::asio::ip::tcp::socket& Session::socket(void) {
	return socket_;
}


// 第一个协议包
void Session::init_handler(const boost::system::error_code& _error) {
	if (_error.value() == 2)
	{
		std::cout<<"socket close"<<std::endl;
		sig(serial_num_);
		return;
	}
	// 读取客户端发来的 10 bytes，确定单个包的大小以及数据总大小
	boost::asio::async_read(socket_, boost::asio::buffer(msg_, 34),
		boost::bind(&Session::analyse_handler, shared_from_this(),
		boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

}

void Session::analyse_handler(const boost::system::error_code& _error, size_t _readSize) {
	if (_error && _error.value() == 2)
	{
		std::cout<<"socket close"<<std::endl;
		sig(serial_num_);
		return;
	}
	std::cout<<"info:"<<msg_<<", threadid:"<<boost::this_thread::get_id()<<std::endl;
	//boost::this_thread::sleep(boost::posix_time::seconds(5));
	//毫秒
	//boost::posix_time::millisec
	std::cout<<"available:"<<socket_.available()<<std::endl;
	memset(msg_,0, 35);
	boost::asio::async_read(socket_, boost::asio::buffer(msg_, 34),
		boost::bind(&Session::analyse_handler, shared_from_this(),
		boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}


// 完成数据传输
void Session::done_handler(const boost::system::error_code& _error) {
	if (_error) {
		return;
	}
	currentMsg_ += msg_;
	// 发送信息到回调
	if (!currentMsg_.empty() && callback_ != NULL) {
		callback_(socket_.remote_endpoint().address().to_string(), currentMsg_);
		currentMsg_.clear();
	}
	memset(msg_, 0, sizeof(msg_));

	char msg[32] = "001:will done.";
	boost::asio::async_write(socket_, boost::asio::buffer(msg, REPLY_SIZE),
		boost::bind(&Session::init_handler, shared_from_this(),
		boost::asio::placeholders::error));
}

void Session::read_handler(const boost::system::error_code& _error, size_t _readSize) {
	if (_error) {
		return;
	}
	// 数据处理
	currentMsg_ += msg_;
	if (currentMsg_.size() > 1024 * 512) {
		// 发送信息到回调
		if (callback_ != NULL) {
			callback_(socket_.remote_endpoint().address().to_string(), currentMsg_);
			currentMsg_.clear();
		}
	}
	memset(msg_, 0, sizeof(msg_));

	// 计算当前剩余数据数量
	sumSize_ -= _readSize;

	// 接收完成
	if (0 > sumSize_) {
		done_handler(_error);
	}
	// 继续接收
	else {
		char msg[REPLY_SIZE];
		sprintf_s(msg, "001:%d.", sumSize_);
		boost::asio::async_write(socket_, boost::asio::buffer(msg, REPLY_SIZE),
			boost::bind(&Session::write_handler, shared_from_this(),
			boost::asio::placeholders::error));

		std::cout << "send client recv succeed: " << msg << std::endl;
	}



}
void Session::write_handler(const boost::system::error_code& _error) {
	if (_error) {
		return;
	}

	boost::asio::async_read(socket_, boost::asio::buffer(msg_, maxSize_),
		boost::bind(&Session::read_handler, shared_from_this(),
		boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}
