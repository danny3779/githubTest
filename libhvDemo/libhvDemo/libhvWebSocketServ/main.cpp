﻿/*
 * websocket server
 *
 * @build   make examples
 * @server  bin/websocket_server_test 9999
 * @client  bin/websocket_client_test ws://127.0.0.1:9999/
 * @python  scripts/websocket_server.py
 * @js      html/websocket_client.html
 *
 */

#include "WebSocketServer.h"
#include "EventLoop.h"
#include "htime.h"

using namespace hv;
#pragma comment(lib, "hv.lib")

/*
 * #define TEST_WSS 1
 *
 * @build   ./configure --with-openssl && make clean && make
 *
 * @server  bin/websocket_server_test 9999
 *
 * @client  bin/websocket_client_test ws://127.0.0.1:9999/
 *          bin/websocket_client_test wss://127.0.0.1:10000/
 *
 */
#define TEST_WSS 0

using namespace hv;

class MyContext
{
public:
  MyContext()
  {
    printf("MyContext::MyContext()\n");
    timerID = INVALID_TIMER_ID;
  }
  ~MyContext()
  {
    printf("MyContext::~MyContext()\n");
  }

  int handleMessage(const std::string& msg, enum ws_opcode opcode)
  {
    printf("onmessage(type=%s len=%d): %.*s\n", opcode == WS_OPCODE_TEXT ? "text" : "binary",
      (int)msg.size(), (int)msg.size(), msg.data());
    return msg.size();
  }

  TimerID timerID;
};

int main(int argc, char** argv)
{
  int port = 11789;

  WebSocketService ws;
  // ws.setPingInterval(10000);
  ws.onopen = [](const WebSocketChannelPtr& channel, const HttpRequestPtr& req)
  {
    printf("onopen: GET %s\n", req->Path().c_str());
    auto ctx = channel->newContextPtr<MyContext>();
    // send(time) every 1s
    ctx->timerID = setInterval(1000, [channel](TimerID id)
    {
      if (channel->isConnected() && channel->isWriteComplete())
      {
        char str[DATETIME_FMT_BUFLEN] = { 0 };
        datetime_t dt = datetime_now();
        datetime_fmt(&dt, str);
        channel->send(str);
      }
    });
  };
  ws.onmessage = [](const WebSocketChannelPtr& channel, const std::string& msg)
  {
    auto ctx = channel->getContextPtr<MyContext>();
    ctx->handleMessage(msg, channel->opcode);
  };
  ws.onclose = [](const WebSocketChannelPtr& channel)
  {
    printf("onclose\n");
    auto ctx = channel->getContextPtr<MyContext>();
    if (ctx->timerID != INVALID_TIMER_ID)
    {
      killTimer(ctx->timerID);
      ctx->timerID = INVALID_TIMER_ID;
    }
    // channel->deleteContextPtr();
  };

  WebSocketServer server;
  server.port = port;
#if TEST_WSS
  server.https_port = port + 1;
  hssl_ctx_opt_t param;
  memset(&param, 0, sizeof(param));
  param.crt_file = "cert/server.crt";
  param.key_file = "cert/server.key";
  param.endpoint = HSSL_SERVER;
  if (server.newSslCtx(&param) != 0) {
    fprintf(stderr, "new SSL_CTX failed!\n");
    return -20;
  }
#endif
  server.registerWebSocketService(&ws);
  server.start();

  // press Enter to stop
  while (getchar() != '\n');
  return 0;
}