/*
 * websocket client
 *
 * @build   make examples
 * @server  bin/websocket_server_test 8888
 * @client  bin/websocket_client_test ws://127.0.0.1:8888/test
 * @clients bin/websocket_client_test ws://127.0.0.1:8888/test 100
 * @python  scripts/websocket_server.py
 * @js      html/websocket_client.html
 *
 */

#include "WebSocketClient.h"

using namespace hv;
#pragma comment(lib, "hv.lib")

int main(int argc, char** argv)
{
  const char* url = "ws://127.0.0.1:11789";

  WebSocketClient ws;

  // set callbacks
  ws.onopen = [&]()
  {
    const HttpResponsePtr& resp = ws.getHttpResponse();
    printf("onopen\n%s\n", resp->body.c_str());
    // printf("response:\n%s\n", resp->Dump(true, true).c_str());
  };
  ws.onmessage = [&](const std::string& msg)
  {
    printf("onmessage(type=%s len=%d): %.*s\n", ws.opcode() == WS_OPCODE_TEXT ? "text" : "binary",
      (int)msg.size(), (int)msg.size(), msg.data());
  };
  ws.onclose = []()
  {
    printf("onclose\n");
  };

  // reconnect: 1,2,4,8,10,10,10...
  reconn_setting_t reconn;
  reconn_setting_init(&reconn);
  reconn.min_delay = 1000;
  reconn.max_delay = 10000;
  reconn.delay_policy = 2;
  ws.setReconnect(&reconn);

  ws.open(url);
  while (getchar() != '\n');
  return 0;
}