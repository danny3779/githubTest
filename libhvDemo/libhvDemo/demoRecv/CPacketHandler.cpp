#include "CPacketHandler.h"
#include <iostream>
using namespace std;

const std::string HEADER_CHECK = "aaaaaaaaaa";
const std::string HEADER_CHECK_SINGLE = "a";
const int HEADER_CHECK_LEN = HEADER_CHECK.length();
const int SOCK_HEADER_LEN = 13;              // socket流包头长度

CPacketHandler::CPacketHandler()
{

}

CPacketHandler::~CPacketHandler()
{

}

void CPacketHandler::AddMsg(std::string& msg)
{
  //msg = "111aaaaaaaaaa004uuuuaaaaaaaaaa005bbbbbaaaaaaaaaa008ccccccc";
  //DealMsg(msg);
  //return;

  if (IsWholePacket(msg))
  {
    DoTask(msg);
    m_remainMsg.clear();
  }
  else
  {
    cout << endl << endl << "!!!nianle!!!" << msg << endl << endl;
    DealMsg(msg);
    if(!m_remainMsg.empty())
      cout << "reminder:" << m_remainMsg << endl << endl;
  }
}

bool CPacketHandler::IsWholePacket(const std::string& msg)
{
  if (msg.length() < SOCK_HEADER_LEN || HEADER_CHECK != string(msg.substr(0, HEADER_CHECK_LEN)))
    return false;

  string strLen = msg.substr(HEADER_CHECK_LEN, SOCK_HEADER_LEN - HEADER_CHECK_LEN);
  auto rlen = atoi(strLen.c_str());

  if (msg.length() != SOCK_HEADER_LEN + rlen)
    return false;

  return true;
}

void CPacketHandler::AddEvent(TASK task)
{
  m_task = task;
}

void CPacketHandler::DoTask(const std::string& msg)
{
  m_task(msg);
  //std::cout << msg << std::endl;
}

void CPacketHandler::DealMsg(std::string& msg)
{
  //msg = "111aaaaaaaaaa004uuuuaaaaaaaaaa005bbbbbaaaaaaaaaa008ccccccccaaaa";
  msg = m_remainMsg + msg;

  while (true)
  {
    auto pos = msg.find(HEADER_CHECK);
    if (pos == string::npos)
      break;

    msg = msg.substr(pos);

    string outputMsg;
    pos = msg.find(HEADER_CHECK, HEADER_CHECK_LEN);
    if (pos == string::npos)
    {
      if (IsWholePacket(msg))
        DoTask(msg);
      else
        m_remainMsg = msg;

      break;
    }
    else
    {
      outputMsg = msg.substr(0, pos);
      if (IsWholePacket(outputMsg))
        DoTask(outputMsg);

      msg = msg.substr(pos);
    }
  }
}
