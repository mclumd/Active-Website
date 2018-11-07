# CGI virdoc library makefile

# You shouldn't have to edit anything else.

INSTALL=install
AUX_LIBS=
RANLIB=ranlib
RM= /bin/rm -f

.c.o:
	$(CC) -c $(CFLAGS) $(AUX_CFLAGS) $(INF_INCS) $<

ALL = libcgi.a

.c.o:	cgi.h $<
	$(CC) -c $(CFLAGS) $(AUX_CFLAGS) $(INF_INCS) $<

all: $(ALL)

libcgi.a: form_ent.o get_cgi_info.o main.o syn_mime.o syn_url.o mcode.o\
	form_tags.o strops.o html.o
	ar r $@ $?
	${RANLIB} $@

install: $(ALL)

clean:
	-/bin/rm -f *.o *~ *.a
