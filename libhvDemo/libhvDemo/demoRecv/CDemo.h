#pragma once
#include <string>
#include "CPacketHandler.h"
class CDemo
{
public:
  CDemo();
  virtual ~CDemo();

public:
  int Run();

  void OutputWhole(const std::string& msg);

  CPacketHandler m_PH;
};

