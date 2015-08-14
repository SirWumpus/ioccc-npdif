.POSIX :

.SUFFIXES :
A = .a
O = .o
E =
.SUFFIXES : .c .h .i $O $A $E

top_srcdir	:= ..
CC              := gcc
CC_E            := -o
CC_O            := -o
CFLAGS		:= -pg -g -O0 -std=c99 -Wall -Wno-char-subscripts -pedantic
LDFLAGS		:=
INSTALL 	:= /usr/local

.c$O :
	${CC} ${CFLAGS} -c $<

.c$E :
	$(CC) $(CFLAGS) $(LDFLAGS) $(CC_E)$*$E $<

$O$E :
	$(LD) $(LDFLAGS) $(CC_E)$*$E $*$O

.c.i:
	${CC} -E ${CFLAGS} $*.c >$*.i

all: build

build: ioccc$E makeholes$E npdif$E

clean:
	-rm npdif.i npdif$O npdif$E *.tmp *.rej *.orig *.core 2>/dev/null

distclean: clean
	-rm ioccc$E makeholes$E ${top_srcdir}/prog$E ${top_srcdir}/prog.c \
	${top_srcdir}/prog-test.sh iocccsize$E 2>/dev/null

install:
	-mv npdif$E $(INSTALL)/bin
	-cp npdif.txt $(INSTALL)/man/cat1/npdif.1

iocccsize$E: ../rules/iocccsize.c
	${CC} ${CFLAGS} -oiocccsize$E ../rules/iocccsize.c

${top_srcdir}/prog.c: npdif.c transform.sed ioccc$E
	@echo '***************************************************************'
	@sed -f transform.sed npdif.c | sed -e '/^[ 	]*$$/d' | ./ioccc$E -r >${top_srcdir}/prog.c
	@echo '***************************************************************'

${top_srcdir}/prog$E: ${top_srcdir}/prog.c
	${CC} ${CFLAGS} ${LDFLAGS} -o$@ ${top_srcdir}/prog.c ${LIBS}

${top_srcdir}/prog-test.sh: npdif.sh
	sed -e'/^PROG/s/npdif/prog/' npdif.sh >${top_srcdir}/prog-test.sh
	chmod 755 ${top_srcdir}/prog-test.sh

entry : ${top_srcdir}/prog$E ${top_srcdir}/prog-test.sh

