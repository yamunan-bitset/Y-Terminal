CC         = c++
CCFLAGS    = -I/usr/include/X11
LINKERLIBS = -lSDL -lutil
OBJS       = main.o tty.o quickcg.o

.PHONY: clean all

all: $(OBJS)
	$(CC) -o $@ $(OBJS) $(CCFLAGS) $(LINKERLIBS)

clean:
	rm -rf all $(OBJS)
