ROOT=..
include $(ROOT)/Make.defines

PROGS =	stanet

all:	$(PROGS)

stanet:	*.c \
	$(LIBM)
	$(CC) $(CFLAGS) $^ -o $(ROOT)/out/stanet_$@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(PROGS) $(TEMPFILES) *.o file.hole

include $(ROOT)/Make.libapue.inc

