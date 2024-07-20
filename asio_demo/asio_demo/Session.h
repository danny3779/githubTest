#ifndef __CLY_SESSION_H__
#define __CLY_SESSION_H__

#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/signals2.hpp>

#define REPLY_SIZE (32)
class io_service_pool;
// 会话类
class Session : public boost::enable_shared_from_this<Session>
{

public:

	typedef void pSessionCallback(std::string, std::string);
	typedef boost::shared_ptr<io_service_pool>	io_service_pool_ptr;
public:

	Session(boost::asio::io_service& _ioService, int sn, io_service_pool* _io_service_pool);
	virtual ~Session(void);

	void start(void);

	void setCallback(pSessionCallback* _callback) { callback_ = _callback; }

	// socket 实例
	boost::asio::ip::tcp::socket& socket(void);

private:

	// 第一个协议包
	void init_handler(const boost::system::error_code& _error);

	// 解析协议包
	void analyse_handler(const boost::system::error_code& _error, size_t _readSize);

	// 完成数据传输后触发的收尾工作
	void done_handler(const boost::system::error_code& _error);

	// 读取成功后触发的函数
	void read_handler(const boost::system::error_code& _error, size_t _readSize);

	// 写入完成后触发的函数
	void write_handler(const boost::system::error_code& _error);

private:
	// 临时信息缓冲区
	char msg_[1024];
	std::string currentMsg_;
	// 数据总数量
	int sumSize_;
	// 单个数据包大小
	unsigned int maxSize_;
	// socket句柄
	boost::asio::ip::tcp::socket socket_;
	// 回调
	pSessionCallback* callback_;

	boost::signals2::signal<void(int)>	sig;
	boost::signals2::connection			thread_quit_connecttion_;
	int serial_num_;
	io_service_pool					*io_service_pool_ptr_;
};


#endif // __CLY_SESSION_H__
