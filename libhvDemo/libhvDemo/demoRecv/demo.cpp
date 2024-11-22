#include "CDemo.h"

#pragma comment(lib, "hv.lib")

int main(int argc, char* argv[])
{
  CDemo demo;
  demo.Run();

  // press Enter to stop
  while (getchar() != '\n');
  return 0;
}
