#include <X11/Xlib.h>

struct X11
{
  int fd;
  Display* dpy;
  int screen;
  Window root;

  Window termwin;
  GC termgc;
  unsigned long col_fg, col_bg;
  int w, h;

  XFontStruct* xfont;
  int font_width, font_height;

  char* buf;
  int buf_w, buf_h;
  int buf_x, buf_y;
};

static inline void Beep(struct X11* x11) { XBell(x11->dpy, 100); }
