.POSIX :

.SUFFIXES :
A = .a
O = .o
.SUFFIXES : .c .h .i $O $A

top_srcdir	:= ..
CC		:= gcc
CFLAGS		:= -g -O2 -std=c99 -Wall -pedantic -Wno-char-subscripts
LDFLAGS		:=
INSTALL 	:= /usr/local

W64CC		:= i686-w64-mingw32-gcc
W64CFLAGS	:= -I. -Os -s -std=c99 -Wno-char-subscripts

.c$O :
	${CC} ${CFLAGS} -c $<

.c$E :
	$(CC) $(CFLAGS) $(LDFLAGS) -o$* $<

$O :
	$(LD) $(LDFLAGS) -o$* $*$O

.c.i:
	${CC} -E ${CFLAGS} $*.c >$*.i

ZIP	:= README.txt prog-w64.exe prog-test.sh avgtime.sh makeholes-w64.exe

all: build

build: ioccc makeholes npdif

clean:
	-rm npdif npdif-w64 *.i *$O *.tmp *.rej *.orig *.core *.out 2>/dev/null
	-cd ${top_srcdir}; rm prog *.exe *.i *$O *.tmp *.rej *.orig *.core *.out 2>/dev/null

distclean: clean
	-rm ioccc iocccsize makeholes transform.gsed 2>/dev/null
	-cd ${top_srcdir}; rm *.c *.sh *.html *.zip 2>/dev/null

install:
	-mv npdif $(INSTALL)/bin
	-cp npdif.txt $(INSTALL)/man/cat1/npdif.1

npdif-w64: err.h npdif.c
	${W64CC} ${W64CFLAGS} ${LDFLAGS} -o$@ npdif.c

iocccsize: ../rules/iocccsize.c
	${CC} ${CFLAGS} -oiocccsize ../rules/iocccsize.c

${top_srcdir}/prog.c: npdif.c transform.sed ioccc
	@echo '***************************************************************'
	@sed -f transform.sed npdif.c | sed -e '/^[ 	]*$$/d' | ./ioccc -r >${top_srcdir}/prog.c
	@echo '***************************************************************'

${top_srcdir}/prog: ${top_srcdir}/prog.c
	${CC} ${CFLAGS} ${LDFLAGS} -o$@ ${top_srcdir}/prog.c

${top_srcdir}/makeholes-w64: makeholes.c
	${W64CC} ${W64CFLAGS} -Ispoiler ${LDFLAGS} -o$@ makeholes.c

${top_srcdir}/prog-test.sh: npdif-test.sh
	sed -e's/npdif/prog/' npdif-test.sh >${top_srcdir}/prog-test.sh
	chmod 755 ${top_srcdir}/prog-test.sh

${top_srcdir}/avgtime.sh: avgtime.sh
	cp avgtime.sh ${top_srcdir}

${top_srcdir}/makeholes.c: makeholes.c
	cp makeholes.c ${top_srcdir}

entry : ${top_srcdir}/prog ${top_srcdir}/prog-test.sh ${top_srcdir}/avgtime.sh ${top_srcdir}/makeholes.c

transform.gsed: ioccc transform.sed npdif.c
	@echo '***************************************************************'
	@sed -e's/\[\[:\([<>]\):\]\]/\\\1/g' transform.sed >transform.gsed
	@sed -f transform.gsed npdif.c | sed -e '/^[ 	]*$$/d' | ./ioccc -r >${top_srcdir}/prog.c
	@echo '***************************************************************'

${top_srcdir}/prog-w64: err.h transform.gsed
	${W64CC} ${W64CFLAGS} -Ispoiler ${LDFLAGS} -o$@ ${top_srcdir}/prog.c

entry64 : ${top_srcdir}/prog-w64 ${top_srcdir}/prog-test.sh ${top_srcdir}/avgtime.sh \
	  ${top_srcdir}/makeholes-w64 ${top_srcdir}/makeholes.c

zip: entry64
	-rm -f ${top_srcdir}/*.zip 2>/dev/null
	cd ${top_srcdir}; 7za a -tzip npdif.zip ${ZIP}
