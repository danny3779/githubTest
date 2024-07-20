#ifndef __CLY_SESSION_H__
#define __CLY_SESSION_H__

#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/signals2.hpp>

#define REPLY_SIZE (32)
class io_service_pool;
// �Ự��
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

	// socket ʵ��
	boost::asio::ip::tcp::socket& socket(void);

private:

	// ��һ��Э���
	void init_handler(const boost::system::error_code& _error);

	// ����Э���
	void analyse_handler(const boost::system::error_code& _error, size_t _readSize);

	// ������ݴ���󴥷�����β����
	void done_handler(const boost::system::error_code& _error);

	// ��ȡ�ɹ��󴥷��ĺ���
	void read_handler(const boost::system::error_code& _error, size_t _readSize);

	// д����ɺ󴥷��ĺ���
	void write_handler(const boost::system::error_code& _error);

private:
	// ��ʱ��Ϣ������
	char msg_[1024];
	std::string currentMsg_;
	// ����������
	int sumSize_;
	// �������ݰ���С
	unsigned int maxSize_;
	// socket���
	boost::asio::ip::tcp::socket socket_;
	// �ص�
	pSessionCallback* callback_;

	boost::signals2::signal<void(int)>	sig;
	boost::signals2::connection			thread_quit_connecttion_;
	int serial_num_;
	io_service_pool					*io_service_pool_ptr_;
};


#endif // __CLY_SESSION_H__
