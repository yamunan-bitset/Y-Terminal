#define _XOPEN_SOURCE 600
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <pty.h>

#include "pty.h"
#include "x11.h"

#define TERM  "xterm-256color"
#define SHELL getenv("SHELL")

bool Pair(struct PTY* pty)
{
  char* slave_name;

  pty->master = posix_openpt(O_RDWR | O_NOCTTY);
  if (pty->master == -1)
    {
      perror("posix_openpt");
      return false;
    }

  if (grantpt(pty->master) == -1)
    {
      perror("grantpt");
      return false;
    }

  if (unlockpt(pty->master) == -1)
    {
      perror("grantpt");
      return false;
    }

  slave_name = ptsname(pty->master);
  if (slave_name == NULL)
    {
      perror("ptsname");
      return false;
    }

  pty->slave = open(slave_name, O_RDWR | O_NOCTTY);
  if (pty->slave == -1)
    {
      perror("open(slave_name)");
      return false;
    }

  return true;
}

bool Spawn(struct PTY* pty)
{
  pid_t p;
  char* env[] = { "TERM="TERM, NULL };

  p = fork();
  if (p == 0)
    {
      close(pty->master);

      setsid();
      if (ioctl(pty->slave, TIOCSCTTY, NULL) == -1)
        {
	  perror("ioctl(TIOCSCTTY)");
	  return false;
        }

      dup2(pty->slave, 0);
      dup2(pty->slave, 1);
      dup2(pty->slave, 2);
      close(pty->slave);

      execle(SHELL, SHELL, (char *)NULL, env);
      return false;
    }
  else if (p > 0)
    {
      close(pty->slave);
      return true;
    }

  perror("fork");
  return false;
}

bool Resize(struct PTY* pty, struct X11* x11)
{
  struct winsize ws = {
    .ws_col = x11->buf_w,
    .ws_row = x11->buf_h,
  };

  if (ioctl(pty->master, TIOCSWINSZ, &ws) == -1)
    {
      perror("ioctl(TIOCSWINSZ)");
      return false;
    }

  return true;
}
