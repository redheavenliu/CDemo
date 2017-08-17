ROOT=..
include $(ROOT)/Make.defines

LIBMTMP=libm.a
OBJS=m_string.o m_http.o m_sock.o m_log.o m_file.o m_error.o m_wrap.o m_util.o \
	  m_signal.o

all:	$(LIBMTMP)

$(LIBMTMP): $(OBJS)
#	cp -rf $(ROOT)/lib/ $(ROOT)/out/
	$(AR) rv $(LIBMTMP)  $?
	$(RANLIB) $(LIBMTMP)
#	rm -f *.o


clean:
	rm -f *.o a.out core temp.* $(LIBMTMP)

include $(ROOT)/Make.libapue.inc
