.POSIX :

.SUFFIXES :
A = .a
O = .o
.SUFFIXES : .c .h .i $O $A

top_srcdir	:= ..
CC		:= gcc
CFLAGS		:= -g -O2 -std=c99 -Wall -pedantic
LDFLAGS		:=
INSTALL 	:= /usr/local

W64CC		:= i686-w64-mingw32-gcc
W64CFLAGS	:= -I. -Os -s -std=c99

.c$O :
	${CC} ${CFLAGS} -c $<

.c$E :
	$(CC) $(CFLAGS) $(LDFLAGS) -o$* $<

$O :
	$(LD) $(LDFLAGS) -o$* $*$O

.c.i:
	${CC} -E ${CFLAGS} $*.c >$*.i

all: build

build: ioccc makeholes npdif

clean:
	-rm npdif npdif-w64 *.i *$O *.tmp *.rej *.orig *.core *.out 2>/dev/null

distclean: clean
	-rm ioccc makeholes ${top_srcdir}/prog ${top_srcdir}/prog.c \
	${top_srcdir}/prog-test.sh iocccsize 2>/dev/null

install:
	-mv npdif $(INSTALL)/bin
	-cp npdif.txt $(INSTALL)/man/cat1/npdif.1

npdif-w64: err.h npdif.c
	${W64CC} -DTEST ${W64CFLAGS} ${LDFLAGS} -o$@ npdif.c ${LIBS}

iocccsize: ../rules/iocccsize.c
	${CC} ${CFLAGS} -oiocccsize ../rules/iocccsize.c

${top_srcdir}/prog.c: npdif.c transform.sed ioccc
	@echo '***************************************************************'
	@sed -f transform.sed npdif.c | sed -e '/^[ 	]*$$/d' | ./ioccc -r >${top_srcdir}/prog.c
	@echo '***************************************************************'

${top_srcdir}/prog: ${top_srcdir}/prog.c
	${CC} ${CFLAGS} ${LDFLAGS} -o$@ ${top_srcdir}/prog.c ${LIBS}

${top_srcdir}/prog-test.sh: npdif.sh
	sed -e'/^PROG/s/npdif/prog/' npdif.sh >${top_srcdir}/prog-test.sh
	chmod 755 ${top_srcdir}/prog-test.sh

entry : ${top_srcdir}/prog ${top_srcdir}/prog-test.sh

