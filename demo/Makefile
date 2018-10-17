ROOT=..
include $(ROOT)/Make.defines

PROGS =	demo thread wifi_video_server wifi_video_client wifi_video_collect_client temp_demo queue_demo \
		coap_demo

all:	$(PROGS)

%:	%.c $(LIBM)
	$(CC) $(CFLAGS) $@.c -o $(ROOT)/out/demo_$@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(PROGS) $(TEMPFILES) *.o file.hole

include $(ROOT)/Make.libapue.inc
