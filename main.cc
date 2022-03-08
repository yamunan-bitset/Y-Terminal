#include "quickcg.h"
#include "pty.h"
#include <iostream>
#include <unistd.h>
#include <pty.h>
#include <cstring>

extern int Spawn();

bool empty(std::string_view str)
{
  if (str.size() == 0)
    {
      std::cout << "Empty" << std::endl;
      return true;
    }
  std::cout << "Full" << std::endl;
  return false;
}

int main(int argc, char *argv[])
{
  QuickCG::screen(800, 800, 0, "Y-Terminal");
  int fd = Spawn();
  char buffer[4096];
  std::cout << fd << std::endl;
  while (!QuickCG::done())
    {
      ssize_t size = read(fd, buffer, 1);
      std::cout << size << std::endl; // TODO: should not ouput -1
      if (!empty(buffer))
	{
	  QuickCG::print(buffer, 8, 8);
	  std::cout << "\033[0;31m" << buffer << "\033[0m" << std::endl;
	}
      std::memset(buffer, '\0', sizeof buffer);
      QuickCG::redraw();
    }
  return 0;
}
