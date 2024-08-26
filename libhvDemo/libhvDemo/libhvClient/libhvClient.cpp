#include "hv/TcpClient.h"
#include "hv/htime.h"
#include <string>
#include <thread>
using namespace hv;
using namespace std;

#pragma comment(lib, "hv.lib")

int main(int argc, char* argv[])
{
  int port = 11789;

  TcpClient cli;
  int connfd = cli.createsocket(port);
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
      setInterval(3000, [channel](TimerID timerID)
      {
        if (channel->isConnected()) 
        {
          char str[DATETIME_FMT_BUFLEN] = { 0 };
          datetime_t dt = datetime_now();
          datetime_fmt(&dt, str);

          string sss = "33";

          unpack_setting_t unpackSetting;
          memset(&unpackSetting, 0, sizeof(unpack_setting_t));
          unpackSetting.mode = UNPACK_BY_LENGTH_FIELD;
          unpackSetting.package_max_length = DEFAULT_PACKAGE_MAX_LENGTH;
          unpackSetting.body_offset = 5;
          unpackSetting.length_field_offset = 1;
          unpackSetting.length_field_bytes = 4;
          unpackSetting.length_field_coding = ENCODE_BY_BIG_ENDIAN;
          unpackSetting.length_adjustment = 0;
          channel->setUnpack(&unpackSetting);

          //channel->write(sss);
        }
        else 
        {
          killTimer(timerID);
        }
      });
    }
    else
    {
      printf("disconnected to %s! connfd=%d\n", peeraddr.c_str(), channel->fd());
    }
  };
  cli.onMessage = [](const SocketChannelPtr& channel, Buffer* buf)
  {
    printf("< %.*s\n", (int)buf->size(), (char*)buf->data());
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
  unpackSetting.mode = UNPACK_BY_LENGTH_FIELD;
  unpackSetting.package_max_length = DEFAULT_PACKAGE_MAX_LENGTH;
  unpackSetting.body_offset = 5;
  unpackSetting.length_field_offset = 1;
  unpackSetting.length_field_bytes = 4;
  unpackSetting.length_field_coding = ENCODE_BY_BIG_ENDIAN;
  unpackSetting.length_adjustment = 0;
  cli.setUnpack(&unpackSetting);

  cli.start();

  std::thread t([&]()
  {
    while (true)
    {
      Sleep(3000);
      string str("33");
      cli.send(str);
    }
  });

  // press Enter to stop
  while (getchar() != '\n');
  return 0;
}
