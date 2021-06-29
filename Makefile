CC         = cc
CCFLAGS    = -I/usr/include/X11
LINKERLIBS = -lX11 
OBJS       = main.o

all: $(OBJS)
	$(CC) -o $@ $(OBJS) $(CCFLAGS) $(LINKERLIBS)

clean:
	rm -rf all $(OBJS)
