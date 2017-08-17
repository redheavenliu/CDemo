ROOT=..
include $(ROOT)/Make.defines

PROGS =	x

all:	$(PROGS)

x:	x_main.c \
	x_pic.c \
	$(LIBM)

	$(CC) $(CFLAGS) $^ -o $(ROOT)/out/x_$@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(PROGS) $(TEMPFILES) *.o file.hole

include $(ROOT)/Make.libapue.inc
