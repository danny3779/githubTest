#include "hv/TcpServer.h"
using namespace hv;

#include <sstream>
using namespace std;

#pragma comment(lib, "hv.lib")

std::map<int, SocketChannelPtr> gDictChannel;

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
  };
  srv.onMessage = [](const SocketChannelPtr& channel, Buffer* buf)
  {
    // echo
    printf("< %.*s\n", (int)buf->size(), (char*)buf->data());
    //channel->write(buf);
  };
  srv.onWriteComplete = [](const SocketChannelPtr& channel, Buffer* buf)
  {
    //printf("> %.*s\n", (int)buf->size(), (char*)buf->data());
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
  srv.start();

  std::thread t([&]()
  {
    while (true)
    {
      int counter = 0;
      std::stringstream ss;
      ss << counter++;
      for (auto it = gDictChannel.begin(); it != gDictChannel.end(); ++it)
      {
        //it->second->write(ss.str());
      }

      srv.broadcast(ss.str());

      std::this_thread::sleep_for(std::chrono::seconds(3));
    }
  });

  // press Enter to stop
  while (getchar() != '\n');
  return 0;
}
