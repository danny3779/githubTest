#pragma once
#include <string>
#include <functional>

using TASK = std::function<void(const std::string&)>;

class CPacketHandler
{
public:
  CPacketHandler();
  virtual ~CPacketHandler();

public:
  void AddMsg(std::string& msg);

  void AddEvent(TASK task);

private:
  // 是否完整包
  bool IsWholePacket(const std::string& msg);

  void DoTask(const std::string& msg);

  /*
  处理消息
  举例：msg = "111aaaaaaaaaa004uuuuaaaaaaaaaa005bbbbbaaaaaaaaaa008ccccccccaaaa"
  分割成：
      aaaaaaaaaa004uuuu
      aaaaaaaaaa005bbbbb
      aaaaaaaaaa008cccccccc
      m_remainMsg = "aaaa"
  */
  void DealMsg(std::string& msg);

private:
  std::string m_remainMsg;
  TASK m_task;
};

