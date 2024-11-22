#include "hv/TcpServer.h"
using namespace hv;



#include <sstream>
#include <iostream>
#include <shared_mutex>
using namespace std;

#pragma comment(lib, "hv.lib")

std::map<int, SocketChannelPtr> gDictChannel;
//std::mutex mtx;
std::shared_mutex gSharedMtx;

typedef std::shared_lock<std::shared_mutex> ReadLock; // 读锁
typedef std::lock_guard<std::shared_mutex> WriteLock; // 写锁

int main(int argc, char* argv[])
{
  int port = 11789;

  TcpServer srv;
  int listenfd = srv.createsocket(port);
  if (listenfd < 0)
  {
    return -20;
  }
  printf("server listen on port %d, listenfd=%d ...\n", port, listenfd);
  srv.onConnection = [](const SocketChannelPtr& channel)
  {
    //std::lock_guard<std::mutex> lck(mtx);
    WriteLock lock(gSharedMtx);

    std::string peeraddr = channel->peeraddr();
    if (channel->isConnected())
    {
      printf("%s connected! connfd=%d\n", peeraddr.c_str(), channel->fd());
      gDictChannel[channel->fd()] = channel;
    }
    else
    {
      printf("%s disconnected! connfd=%d\n", peeraddr.c_str(), channel->fd());
      gDictChannel.erase(channel->fd());
    }

    cout << "!!-total:" << gDictChannel.size() << endl;

    // //启动定时器
    //setInterval(3000, [=](TimerID timerID)
    //{
    //  channel->close();
    //});
  };
  srv.onMessage = [](const SocketChannelPtr& channel, Buffer* buf)
  {
    // echo
    printf("< %.*s\n", (int)buf->size(), (char*)buf->data());
    //channel->write(buf);
  };
  srv.onWriteComplete = [](const SocketChannelPtr& channel, Buffer* buf)
  {
    string msg((char*)buf->data(), (int)buf->size());
    stringstream ss;
    ss << channel->fd();
    msg = ss.str() + msg;
    //printf("> %.*s\n", (int)buf->size(), (char*)buf->data());
    //cout << msg << endl;
  };

  unpack_setting_t unpackSetting;
  memset(&unpackSetting, 0, sizeof(unpack_setting_t));
  unpackSetting.mode = UNPACK_BY_LENGTH_FIELD; // 设置解包模式：按照字段长度解包
  unpackSetting.package_max_length = DEFAULT_PACKAGE_MAX_LENGTH; // 设置整包最大长度，默认2M
  unpackSetting.body_offset = 5; // 设置包体偏移量
  unpackSetting.length_field_offset = 1; // 设置长度偏移量
  unpackSetting.length_field_bytes = 4; // 设置长度字节数
  unpackSetting.length_adjustment = 0; // 设置长度调整
  unpackSetting.length_field_coding = ENCODE_BY_BIG_ENDIAN; // 设置字节编码规则，默认"大字节"序
  srv.setUnpack(&unpackSetting);

  srv.setThreadNum(4);
  srv.setLoadBalance(LB_LeastConnections);
  srv.start();

  std::thread t([&]()
  {
    int counter = 0;
    while (true)
    {
      std::stringstream ss;
      ss << counter++ << "..sdfsdwerewrewrewr444444!!!!!..sdfsdwerewrewrewr444444!!!!!..sdfsdwerewrewrewr444444!!!!!..sdfsdwerewrewrewr444444!!!!!";
      auto msg = ss.str();

      char headerPrefix = 'x';

      std::vector<char> buf_new(msg.size() + 5);
      auto len_2 = htonl(msg.size());
      memcpy(&buf_new[0], &headerPrefix, sizeof(char));
      memcpy(&buf_new[0] + 1, &len_2, sizeof(len_2));
      memcpy(&buf_new[0] + 5, msg.c_str(), msg.size());

      {
        //std::lock_guard<std::mutex> lck(mtx);
        ReadLock lock(gSharedMtx);
        for (auto it = gDictChannel.begin(); it != gDictChannel.end(); ++it)
        {
          // 向某个客户端发送数据
          //it->second->write(ss.str());
        }
      }

      // 向全部客户端发送数据
      srv.broadcast(&buf_new[0], buf_new.size());

      std::this_thread::sleep_for(std::chrono::seconds(1));
      //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });

  // press Enter to stop
  while (getchar() != '\n');
  return 0;
}
