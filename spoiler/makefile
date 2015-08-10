.POSIX :

.SUFFIXES :
A = .a
O = .o
E =
.SUFFIXES : .c .h .i $O $A $E

top_srcdir	:= ..
PACKAGE 	:= npdif
CC              := gcc
CC_E            := -o
CC_O            := -o
CFLAGS		:= -g -O0 -std=c99 -Wall -Wno-char-subscripts -pedantic
LDFLAGS		:=
INSTALL 	:= /usr/local
IT		:= ioccc

.c$O :
	${CC} ${CFLAGS} -c $<

.c$E :
	$(CC) $(CFLAGS) $(LDFLAGS) $(CC_E)$*$E $<

$O$E :
	$(LD) $(LDFLAGS) $(CC_E)$*$E $*$O

.c.i:
	${CC} -E ${CFLAGS} $*.c >$*.i

all: build

build: ${PACKAGE}$E

clean:
	-rm ${PACKAGE}.i ${PACKAGE}$O ${PACKAGE}$E tmp* *.tmp *.core 2>/dev/null

distclean: clean
	-rm ${IT}$E ../${PACKAGE}.c ../${PACKAGE}$E iocccsize$E 2>/dev/null

install:
	-mv ${PACKAGE}$E $(INSTALL)/bin
	-cp ${PACKAGE}.txt $(INSTALL)/man/cat1/${PACKAGE}.1

${IT}$E: ${IT}.c
	${CC} ${CFLAGS} -o${IT}$E ${IT}.c

iocccsize$E: ../rules/iocccsize.c
	${CC} ${CFLAGS} -oiocccsize$E ../rules/iocccsize.c

${top_srcdir}/prog.c: ${PACKAGE}.c transform.sed ${IT}$E
	@echo '***************************************************************'
	@sed -f transform.sed ${PACKAGE}.c | sed -e '/^[ 	]*$$/d' >tmp
	@./${IT}$E -r <tmp >${top_srcdir}/prog.c
	@echo '***************************************************************'

${top_srcdir}/prog$E: ${top_srcdir}/prog.c
	${CC} ${CFLAGS} ${LDFLAGS} -o$@ ${top_srcdir}/prog.c ${LIBS}

entry : ${top_srcdir}/prog$E

