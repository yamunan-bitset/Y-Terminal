#include <poll.h>
#include <string.h>

#include "x11.h"

struct Wind
{
  int cx, cy, i, u, b, r, f, g, top, bottom;  
  struct X11* x11;
};

void Left(struct Wind wind)
{ /*TODO: Handle scroll up*/
}

void Right(struct Wind wind)
{ /*TODO: Handle scroll down*/
}

void ScrollFix(struct Wind wind) { if (wind.cx >= wind.x11->buf_x) { wind.cx = 0; /*TODO: Left();*/ } }
void FixCoord(struct Wind wind)
{
  if (wind.bottom >= wind.x11->buf_y) wind.bottom = wind.x11->buf_x - 1;
  if (wind.top > wind.bottom - 2) wind.top = wind.bottom - 2;
  if (wind.top < 0) wind.top = 0;
  if (wind.bottom < wind.top + 2) wind.bottom = wind.top + 2;
  if (wind.cx < 0) wind.cx = 0;
  if (wind.cy < 0) wind.cy = 0;
  if (wind.cx >= wind.x11->buf_x) wind.cx = wind.x11->buf_y - 1;
  if (wind.cy >= wind.bottom) wind.cy = wind.bottom;
}

void Handle(struct Wind wind, char* s)
{
  for (unsigned int i = 0; i < sizeof s; ++i)
    {
      char c = s[i];
      switch (c)
	{
	case 7: Beep(wind.x11); break;
	case 8: ScrollFix(wind); if (wind.cx > 0) { --wind.cx; } break;
	case 9: ScrollFix(wind); wind.cx += 8 - (wind.cx & 7); FixCoord(wind); break;
	case 10: case 11: case 12: Linefeed: ScrollFix(wind);
	  { /*TODO:...*/
	  }
	}
    }
}
