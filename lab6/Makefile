CC 	= gcc

CFLAGS  = -Wall -g -I .

LD 	= gcc

LDFLAGS  = -Wall -g -L/home/pn-cs453/Given/Asgn2

PUBFILES =  README  hungrymain.c  libPLN.a  libsnakes.a  lwp.h\
	    numbersmain.c  snakemain.c  snakes.h lwp.c

TARGET =  pn-cs453@hornet:Given/asgn2

PROGS	= snakes nums hungry lwp

SNAKEOBJS  = snakemain.o 

HUNGRYOBJS = hungrymain.o 

NUMOBJS    = numbersmain.o

LWPOBJS	   = lwp.o

OBJS	= $(SNAKEOBJS) $(HUNGRYOBJS) $(NUMOBJS) $(LWPOBJS)

SRCS	= lwp.c snakemain.c numbersmain.c

HDRS	= 

EXTRACLEAN = core $(PROGS)

all: 	$(PROGS)

allclean: clean
	@rm -f $(EXTRACLEAN)

clean:	
	rm -f $(OBJS) *~ TAGS

snakes: snakemain.o libPLN.a libsnakes.a
	$(LD) $(LDFLAGS) -o snakes snakemain.o -L. -lncurses -lsnakes -lPLN

hungry: hungrymain.o libPLN.a libsnakes.a
	$(LD) $(LDFLAGS) -o hungry hungrymain.o -L. -lncurses -lsnakes -lPLN

nums: numbersmain.o libPLN.a 
	$(LD) $(LDFLAGS) -o nums numbersmain.o -L. -lPLN

lwp: lwp.o lwp.h
	$(CC) $(CFLAGS) -c $<

hungrymain.o: lwp.h snakes.h

snakemain.o: lwp.h snakes.h

numbermain.o: lwp.h

#libPLN.a: ../Publish/lwp.c
#	gcc -c ../Publish/lwp.c
#	ar r libPLN.a lwp.o
#	rm lwp.o

pub:
	scp $(PUBFILES) $(TARGET)

