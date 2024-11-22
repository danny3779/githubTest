#include "CDemo.h"
#include "hv/TcpClient.h"
#include "hv/htime.h"
#include <string>
#include <thread>
#include <iostream>
#include "CPacketHandler.h"
using namespace hv;
using namespace std;

CDemo::CDemo()
{

}

CDemo::~CDemo()
{

}

int CDemo::Run()
{
  m_PH.AddEvent( std::bind(&CDemo::OutputWhole, this, std::placeholders::_1) );

  int port = 8755;

  TcpClient cli;
  int connfd = cli.createsocket(port, "8.149.236.117");
  if (connfd < 0) {
    return -20;
  }
  printf("client connect to port %d, connfd=%d ...\n", port, connfd);
  cli.onConnection = [](const SocketChannelPtr& channel)
  {
    std::string peeraddr = channel->peeraddr();
    if (channel->isConnected())
    {
      printf("connected to %s! connfd=%d\n", peeraddr.c_str(), channel->fd());
      // send(time) every 3s
      //setInterval(3000, [channel](TimerID timerID)
      //{
      //  //if (channel->isConnected()) 
      //  //{
      //  //  char str[DATETIME_FMT_BUFLEN] = { 0 };
      //  //  datetime_t dt = datetime_now();
      //  //  datetime_fmt(&dt, str);

      //  //  string sss = "33";

      //  //  unpack_setting_t unpackSetting;
      //  //  memset(&unpackSetting, 0, sizeof(unpack_setting_t));
      //  //  unpackSetting.mode = UNPACK_BY_LENGTH_FIELD;
      //  //  unpackSetting.package_max_length = DEFAULT_PACKAGE_MAX_LENGTH;
      //  //  unpackSetting.body_offset = 5;
      //  //  unpackSetting.length_field_offset = 1;
      //  //  unpackSetting.length_field_bytes = 4;
      //  //  unpackSetting.length_field_coding = ENCODE_BY_BIG_ENDIAN;
      //  //  unpackSetting.length_adjustment = 0;
      //  //  channel->setUnpack(&unpackSetting);

      //  //  //channel->write(sss);
      //  //}
      //  //else 
      //  //{
      //  //  killTimer(timerID);
      //  //}
      //});
    }
    else
    {
      printf("disconnected to %s! connfd=%d\n", peeraddr.c_str(), channel->fd());
    }
  };
  cli.onMessage = [=](const SocketChannelPtr& channel, Buffer* buf)
  {
    auto pdata = (char*)buf->data();
    auto pid = channel->fd();

    auto size = (int)buf->size();

    string msg((char*)buf->data(), (int)buf->size());
    //std::cout << msg << std::endl;

    m_PH.AddMsg(msg);

    return;

    /*
    // ���ݰ�����tLen
    {
      string strLen((char*)buf->data() + 1, 4);
      long len = 0;
      memcpy(&len, strLen.c_str(), 4);
      //auto len = atol(strLen.c_str());
      auto tLen = ntohl(len);
    }

    string msg((char*)buf->data() + 5, (int)buf->size() - 5);
    std::cout << msg << std::endl;
    */

    //printf("< %.*s\n", (int)buf->size(), (char*)buf->data()+5);
  };
  cli.onWriteComplete = [](const SocketChannelPtr& channel, Buffer* buf)
  {
    printf("> %.*s\n", (int)buf->size(), (char*)buf->data());
  };
  // reconnect: 1,2,4,8,10,10,10...
  reconn_setting_t reconn;
  reconn_setting_init(&reconn);
  reconn.min_delay = 1000;
  reconn.max_delay = 10000;
  reconn.delay_policy = 2;
  cli.setReconnect(&reconn);

  unpack_setting_t unpackSetting;
  memset(&unpackSetting, 0, sizeof(unpack_setting_t));
  unpackSetting.mode = UNPACK_MODE_NONE; // ���ý��ģʽ�������ֶγ��Ƚ��
  unpackSetting.package_max_length = DEFAULT_PACKAGE_MAX_LENGTH; // ����������󳤶ȣ�Ĭ��2M
  unpackSetting.body_offset = 0; // ���ð���ƫ����
  unpackSetting.length_field_offset = 0; // ���ó���ƫ����
  unpackSetting.length_field_bytes = 0; // ���ó����ֽ���
  unpackSetting.length_adjustment = 0; // ���ó��ȵ���
  unpackSetting.length_field_coding = ENCODE_BY_BIG_ENDIAN; // �����ֽڱ������Ĭ��"���ֽ�"��
  //cli.setUnpack(&unpackSetting);

  cli.start();

  while (getchar() != '\n');
  return 0;
}

void CDemo::OutputWhole(const std::string& msg)
{
  std::cout << msg << std::endl;
}
