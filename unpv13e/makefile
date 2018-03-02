ROOT=..
include $(ROOT)/Make.defines

PROGS =	main intro_cli intro_ser tcp_cli tcp_ser udp_cli udp_ser

all:	$(PROGS)

%:	%.c $(LIBM)
	$(CC) $(CFLAGS) $@.c -o $(ROOT)/out/unpv13e_$@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(PROGS) $(TEMPFILES) *.o file.hole

include $(ROOT)/Make.libapue.inc
