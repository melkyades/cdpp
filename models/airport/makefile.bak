.SUFFIXES : .cpp .o .c .y

include ../../Makefile.common

INCLUDES_CPP+=-I$(MAINDIR)

EXAMPLESOBJS=modulo.o selector.o control.o cola.o deposito.o salida.o
ALLOBJS=${EXAMPLESOBJS} 
ALLSRCS=${ALLOBJS:.o=.cpp}

all: ${EXAMPLESOBJS} libairport.a

clean:
	- rm -f *.o *.a simu core drawlog initest exptest parser makerand toMap

depend:
	makedepend -Y ${ALLSRCS}

backup:
	tar -cvf simu.tar *.cpp *.h *.c *.y makefile* *.ma *.ev *.vpj *.bat *.txt *.val *.inc *.map; gzip simu.tar; mv simu.tar.gz simu.tgz

.cpp.o:
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} ${OPTCPPFLAGS} $<

libairport.a: ${EXAMPLESOBJS}
	ar crs libairport.a ${EXAMPLESOBJS} 

.c.o:
	${CC} -c ${INCLUDES_C} ${DEFINES_C}  ${DEBUGFLAGS} ${CFLAGS} ${OPTCPPFLAGS} $<

.y.o:
	bison -d -v -o gram.c gram.y
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} ${OPTCPPFLAGS} ${@:.o=.c} 
	rm $*.c
	
# DO NOT DELETE

modulo.o: modulo.h
selector.o: selector.h
control.o: control.h
cola.o: cola.h
deposito.o: deposito.h
salida.o: salida.h
