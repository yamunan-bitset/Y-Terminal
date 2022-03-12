#include "quickcg.h"
#include "tty.hh"
#include "pty.h"
#include <pty.h>
#include <poll.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

int main(int argc, char *argv[])
{
  PTY pty;
  QuickCG::screen(800, 800, 0, "Y-Terminal");
  std::cout << Spawn(&pty) << std::endl;
  while (!QuickCG::done())
    {
      struct pollfd p[2] = { { 3/*pty.fd*/, POLLIN, 0 },
			     { 0, POLLIN, 0 } };
      int pollres = poll(p, 2, 30);
      std::cout << pollres << std::endl;
      if (pollres < 0) break;
      if (p[0].revents & POLLIN)
	{
	  QuickCG::print(Recv(&pty).second, 8, 8);
	  std::cout << "\033[0;31mFrom Recv" << Recv(&pty).second << "\033[0m" << std::endl;
	}
      if (p[1].revents & POLLIN)
        {
	  char buffer[4096];
	  int r = read(0, buffer, sizeof(buffer));
	  std::cout << "\033[0;31mFrom Send" << buffer << "\033[0m" << std::endl;
	  if (r > 0) Send(&pty, std::string_view(buffer, r));
        }
      
      QuickCG::redraw();
    }
  return 0;
}
