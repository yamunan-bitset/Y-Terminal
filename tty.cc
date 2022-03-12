#include "tty.hh"

int Spawn(struct PTY* pty)
{
  struct winsize ws = {};
  ws.ws_col = 80; // This is default, TODO: Resize
  ws.ws_row = 25; // This is default, TODO: Resize
  pty->pid = forkpty(&pty->fd, nullptr, nullptr, &ws);
  if (!pty->pid)
    {
      static char termstr[] = "TERM="TERM;
      putenv(termstr);
      execl(SHELL, SHELL, "-l", "-i", nullptr);
    }
  fcntl(pty->fd, F_SETFL, fcntl(pty->fd, F_GETFL) | O_NONBLOCK);
  return pty->fd;
}

int Send(struct PTY* pty, std::string_view buffer)
{
  return write(pty->fd, buffer.data(), buffer.size());
}

std::pair<std::string,int> Recv(struct PTY* pty)
{
    char buffer[4096];
    std::pair<std::string,int> result;
    result.second = read(pty->fd, buffer, sizeof(buffer));
    if(result.second > 0)
        result.first.assign(buffer, buffer+result.second);
    return result;
}
/*
char* Recv(struct PTY* pty)
{
  char buffer[4096];
  read(pty->fd, buffer, sizeof(buffer));
  return buffer;
}
*/
void Close(struct PTY* pty)
{
  kill(pty->pid, SIGTERM);
  close(pty->fd);
  waitpid(pty->pid, nullptr, 0);
}
