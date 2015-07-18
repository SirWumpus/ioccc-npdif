.POSIX :

.SUFFIXES :
A = .a
O = .o
E = 
.SUFFIXES : .c .h .i $O $A $E

CC              := gcc
CC_E            := -o
CC_O            := -o
CFLAGS		:= -g -O0
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

all: npdif

clean:
	rm -f npdif$E

install:
	[ -f cp ]
	[ -f mv ]
	-mv npdif $(INSTALL)/bin
	-cp npdif.txt $(INSTALL)/man/cat1/npdif.1

