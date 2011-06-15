.SUFFIXES : .cpp .o .c .y

include ../../Makefile.common

INCLUDES_CPP+=-I$(MAINDIR)

EXAMPLESOBJS=client.o controla.o disco.o multicpu.o  router.o stopq.o transduc2.o
ALLOBJS=${EXAMPLESOBJS} 
ALLSRCS=${ALLOBJS:.o=.cpp}

all: ${EXAMPLESOBJS} libnet.a

clean:
	- rm -f *.o *.a simu core drawlog initest exptest parser makerand toMap

depend:
	makedepend -Y ${ALLSRCS}

backup:
	tar -cvf simu.tar *.cpp *.h *.c *.y makefile* *.ma *.ev *.vpj *.bat *.txt *.val *.inc *.map; gzip simu.tar; mv simu.tar.gz simu.tgz

.cpp.o:
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} ${OPTCPPFLAGS} $<

libnet.a: ${EXAMPLESOBJS}
	ar crs libnet.a ${EXAMPLESOBJS} 

.c.o:
	${CC} -c ${INCLUDES_C} ${DEFINES_C}  ${DEBUGFLAGS} ${CFLAGS} ${OPTCPPFLAGS} $<

.y.o:
	bison -d -v -o gram.c gram.y
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} ${OPTCPPFLAGS} ${@:.o=.c} 
	rm $*.c
	
# DO NOT DELETE

client.o: client.h mformats.h
controla.o: controla.h mformats.h
disco.o: disco.h mformats.h
multicpu.o: multicpu.h mformats.h
router.o: router.h mformats.h
stopq.o: stopq.h
transduc2.o: transduc2.h mformats.h
