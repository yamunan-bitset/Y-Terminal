#include "quickcg.h"
#include "pty.h"
#include <iostream>
#include <unistd.h>
#include <pty.h>

extern int Spawn();

int main(int argc, char *argv[])
{
  QuickCG::screen(800, 800, 0, "Y-Terminal");
  int fd = Spawn();
  char byte;
  std::cout << fd << std::endl;
  //write(fd, "J", 1);
  while (!QuickCG::done())
    {
      ssize_t size = read(fd, &byte, 1);
      if (byte != '\0')
	{
	  
	  QuickCG::print(byte, 8, 8);
	  std::cout << byte << std::endl;
	}
      byte = '\0';
      QuickCG::redraw();
    }
  return 0;
}
