ROOT=..
include $(ROOT)/Make.defines

PROGS =	coap

all:	$(PROGS)

coap:	*.c *.cpp \
	$(LIBM)
	gcc $(CFLAGS) $^ -lstdc++ -o $(ROOT)/out/coap_$@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(PROGS) $(TEMPFILES) *.o file.hole

include $(ROOT)/Make.libapue.inc