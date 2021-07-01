#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "x11.h"
#include "pty.h"

extern bool Pair  (struct PTY*);              // tty.c
extern bool Spawn (struct PTY*);              // tty.c
extern bool Resize(struct PTY*, struct X11*); // tty.c
extern void Key   (XKeyEvent*, struct PTY*);  // window.c
extern void Redraw(struct X11*);              // window.c
extern bool Setup (struct X11*);              // window.c

int Run(struct PTY *pty, struct X11 *x11)
{
  int i, maxfd;
  fd_set readable;
  XEvent ev;
  char buf[1];
  bool just_wrapped = false;

  maxfd = pty->master > x11->fd ? pty->master : x11->fd;

  for (;;)
    {
      FD_ZERO(&readable);
      FD_SET(pty->master, &readable);
      FD_SET(x11->fd, &readable);

      if (select(maxfd + 1, &readable, NULL, NULL, NULL) == -1)
        {
	  perror("select");
	  return 1;
        }

      if (FD_ISSET(pty->master, &readable))
        {
	  if (read(pty->master, buf, 1) <= 0)
            {
	      fprintf(stderr, "Nothing to read from child: ");
	      perror(NULL);
	      return 1;
            }
	  if (buf[0] == '\r') x11->buf_x = 0;
	  else
            {
	      if (buf[0] != '\n')
                {
		  x11->buf[x11->buf_y * x11->buf_w + x11->buf_x] = buf[0];
		  x11->buf_x++;

		  if (x11->buf_x >= x11->buf_w)
                    {
		      x11->buf_x = 0;
		      x11->buf_y++;
		      just_wrapped = true;
                    }
		  else
		    just_wrapped = false;
                }
	      else if (!just_wrapped)
                {
		  x11->buf_y++;
		  just_wrapped = false;
                }

	      if (x11->buf_y >= x11->buf_h)
                {
		  memmove(x11->buf, &x11->buf[x11->buf_w],
			  x11->buf_w * (x11->buf_h - 1));
		  x11->buf_y = x11->buf_h - 1;

		  for (i = 0; i < x11->buf_w; i++)
		    x11->buf[x11->buf_y * x11->buf_w + i] = 0;
                }
            }

	  Redraw(x11);
        }

      if (FD_ISSET(x11->fd, &readable))
        {
	  while (XPending(x11->dpy))
            {
	      XNextEvent(x11->dpy, &ev);
	      switch (ev.type)
                {
		case Expose:   Redraw(x11);        break;
		case KeyPress: Key(&ev.xkey, pty); break;
                }
            }
        }
    }

  return 0;
}

void usage() { puts("Usage: yterm\nThere isn't anything else you can do, \
other than just running this executable"); exit(0); }

int main(int argc, char** argv)
{
  int o;
  while ((o = getopt(argc, argv, "h")) != -1)
    {
      switch (o)
	{
	case 'h': usage(); break;
	}
    }
  struct PTY pty;
  struct X11 x11;

  if (!Setup(&x11))
    return 1;

  if (!Pair(&pty))
    return 1;

  if (!Resize(&pty, &x11))
    return 1;

  if (!Spawn(&pty))
    return 1;

  return Run(&pty, &x11);
}
