CC         = cc
CCFLAGS    = -I/usr/include/X11
LINKERLIBS = -lX11 -lutil
OBJS       = tty.o window.o main.o

.PHONY: clean all

all: $(OBJS)
	$(CC) -o $@ $(OBJS) $(CCFLAGS) $(LINKERLIBS)

clean:
	rm -rf all $(OBJS)
