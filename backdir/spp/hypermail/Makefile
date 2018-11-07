#
# Makefile for Hypermail
# Kevin Hughes, 8/1/94
#

BINDIR= /usr/local/bin
# This is where you want hypermail to be installed

MANDIR= /usr/local/man/man1
# This is where the man page goes

HTMLDIR= /usr/local/www/software/hypermail
# This is where the HTML documentation goes

CGIDIR= /usr/local/httpd/cgi-bin
# This is where your CGI programs live

CC= cc

# CFLAGS= -O2

OBJS=		file.o mem.o string.o print.o \
		parse.o struct.o date.o hypermail.o

MAILOBJS=	mail.o libcgi/libcgi.a

.c.o:
		$(CC) -c $(CFLAGS) $<

all:		hypermail

hypermail:	$(OBJS)
		$(CC) -o hypermail $(CFLAGS) $(OBJS)
		chmod 0755 hypermail
		chmod 0644 hypermail.1
		chmod 0644 hypermail.html
		chmod 0644 hypermail.gif

libcgi/libcgi.a:
		cd libcgi; make all CC="$(CC)" CFLAGS="$(CFLAGS)"

mail:		$(MAILOBJS)
		$(CC) -o mail $(CFLAGS) $(MAILOBJS)
		chmod 0755 mail

$(OBJS):	Makefile hypermail.h config.h

install:
		install -cs -m 0755 hypermail $(BINDIR)
		install -c -m 0644 hypermail.1 $(MANDIR)

html.install:
		install -c -m 0644 hypermail.html $(HTMLDIR)
		install -c -m 0644 hypermail.gif $(HTMLDIR)

mail.install:
		install -c -m 0755 mail $(CGIDIR)

install.alpha:
		install -c $(BINDIR) -s -m 0755 hypermail
		install -c $(MANDIR) -m 0644 hypermail.1

html.install.alpha:
		install -c $(HTMLDIR) -m 0644 hypermail.html
		install -c $(HTMLDIR) -m 0644 hypermail.gif

mail.install.alpha:
		install -c $(CGIDIR) -m 0755 mail

pure:
	 	make CFLAGS="-g" $(OBJS)
		purify $(CC) -o hypermail -g $(CFLAGS) $(OBJS)

quant:
	 	make CFLAGS="-g" $(OBJS)
		quantify $(CC) -o hypermail -g $(CFLAGS) $(OBJS)

clean:
		rm -f ./hypermail ./mail *.o .pure hypermail.pure* *qx *qv
		rm -fr ./archive
		cd libcgi; make clean
