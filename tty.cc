#define _XOPEN_SOURCE 600
#include <fcntl.h>
#include <unistd.h>
#include <pty.h>
#include <cstdlib>

#include "pty.h"

#define TERM  "xterm-256color"
#define SHELL getenv("SHELL")

int Spawn()
{
  int fd, pid;
  struct winsize ws = {};
  ws.ws_col = 800;
  ws.ws_row = 800;
  pid = forkpty(&fd, nullptr, nullptr, &ws);
  if(!pid)
    {
      static char termstr[] = "TERM=xterm";
      putenv(termstr);
      execl(std::getenv("SHELL"), std::getenv("SHELL"), "-l", "-i", nullptr);
    }
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
  return fd;
}
