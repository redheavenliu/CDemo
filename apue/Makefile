ROOT=..
include $(ROOT)/Make.defines

PROGS =	main chapter_1 chapter_2 chapter_3 chapter_4 chapter_8 chapter_10 chapter_11 chapter_13

all:	$(PROGS)

%:	%.c $(LIBM)
	$(CC) $(CFLAGS) $@.c -o $(ROOT)/out/apue_$@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(PROGS) $(TEMPFILES) *.o file.hole

include $(ROOT)/Make.libapue.inc
