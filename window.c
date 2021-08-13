#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "x11.h"
#include "pty.h"

extern struct Wind;                    // ansi.c
extern void Handle(struct Wind, char); // ansi.c
extern char FromUTF8(char);            // ansi.c

struct Wind wind;

void Key(XKeyEvent* ev, struct PTY* pty)
{
  char buf[32];
  KeySym ksym;

  int num = XLookupString(ev, buf, sizeof buf, &ksym, 0);
  for (unsigned int i = 0; i < num; i++)
    {
      Handle(wind, FromUTF8(&buf[i]));
      write(pty->master, &buf[i], 1);
    }
}

void Redraw(struct X11* x11)
{
  char buf[1];

  XSetForeground(x11->dpy, x11->termgc, x11->col_bg);
  XFillRectangle(x11->dpy, x11->termwin, x11->termgc, 0, 0, x11->w, x11->h);

  XSetForeground(x11->dpy, x11->termgc, x11->col_fg);
  for (unsigned int y = 0; y < x11->buf_h; y++)
    for (unsigned int x = 0; x < x11->buf_w; x++)
      {
	buf[0] = x11->buf[y * x11->buf_w + x];
	if (!iscntrl(buf[0]))
	  {
	    XDrawString(x11->dpy, x11->termwin, x11->termgc,
			x * x11->font_width,
			y * x11->font_height + x11->xfont->ascent,
			buf, 1);
	  }
      }

  XSetForeground(x11->dpy, x11->termgc, x11->col_fg);
  XFillRectangle(x11->dpy, x11->termwin, x11->termgc,
		 x11->buf_x * x11->font_width,
		 x11->buf_y * x11->font_height,
		 x11->font_width, x11->font_height);

  XSync(x11->dpy, False);
}

bool Setup(struct X11* x11)
{
  Colormap cmap;
  XColor color;
  XSetWindowAttributes wa = {
    .background_pixmap = ParentRelative,
    .event_mask = KeyPressMask | KeyReleaseMask | ExposureMask,
  };

  x11->dpy = XOpenDisplay(NULL);
  if (x11->dpy == NULL)
    {
      fprintf(stderr, "Cannot open display\n");
      return false;
    }

  x11->screen = DefaultScreen(x11->dpy);
  x11->root = RootWindow(x11->dpy, x11->screen);
  x11->fd = ConnectionNumber(x11->dpy);

  x11->xfont = XLoadQueryFont(x11->dpy, "9x15bold");
  if (x11->xfont == NULL)
    {
      fprintf(stderr, "Could not load font\n");
      return false;
    }
  x11->font_width = XTextWidth(x11->xfont, "m", 1);
  x11->font_height = x11->xfont->ascent + x11->xfont->descent;

  cmap = DefaultColormap(x11->dpy, x11->screen);

  if (!XAllocNamedColor(x11->dpy, cmap, "#000000", &color, &color))
    {
      fprintf(stderr, "Could not load bg color\n");
      return false;
    }
  x11->col_bg = color.pixel;

  if (!XAllocNamedColor(x11->dpy, cmap, "#aaaaaa", &color, &color))
    {
      fprintf(stderr, "Could not load fg color\n");
      return false;
    }
  x11->col_fg = color.pixel;

  /* The terminal will have a fixed size of 150x50 cells. This is an
   * arbitrary number. No resizing has been implemented and child
   * processes can't even ask us for the current size (for now).
   *
   * buf_x, buf_y will be the current cursor position. */
  x11->buf_w = 200;
  x11->buf_h = 90;
  x11->buf_x = 0;
  x11->buf_y = 0;
  x11->buf = calloc(x11->buf_w * x11->buf_h, 1);
  if (x11->buf == NULL)
    {
      perror("calloc");
      return false;
    }

  x11->w = x11->buf_w * x11->font_width;
  x11->h = x11->buf_h * x11->font_height;

  x11->termwin = XCreateWindow(x11->dpy, x11->root,
			       0, 0,
			       x11->w, x11->h,
			       0,
			       DefaultDepth(x11->dpy, x11->screen),
			       CopyFromParent,
			       DefaultVisual(x11->dpy, x11->screen),
			       CWBackPixmap | CWEventMask,
			       &wa);
  XStoreName(x11->dpy, x11->termwin, "Y-Terminal");
  XMapWindow(x11->dpy, x11->termwin);
  x11->termgc = XCreateGC(x11->dpy, x11->termwin, 0, NULL);

  XSync(x11->dpy, False);

  return true;
}

