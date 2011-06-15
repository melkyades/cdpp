#-*-makefile-*-

include ../../../Makefile.common

CPPFLAGS += -I../../common/include -DSEQUENTIAL -g -Wall

COMMONDIR=../../common/
LIBCOMMON=../../common/libcommon.a

ifndef DESTDIR
DESTDIR=/usr/local
endif

ifndef SEQOBJ
SEQOBJ=obj
endif

libdir = $(DESTDIR)/lib
includedir = $(DESTDIR)/include/warped

OBJ      = obj
SEQSRCS  = BaseSequential.cc SequentialBasicEvent.cc SequentialBasicState.cc\
	SequentialInputQueue.cc	NewMemAlloc.cc SequentialLP.cc SequentialObj.cc\
	SequentialEventQueue.cc SplayTree.cc ConfigurationManager.cc 


SEQOBJS = $(OBJ)/BaseSequential.o $(OBJ)/SequentialBasicEvent.o $(OBJ)/SequentialBasicState.o\
	$(OBJ)/SequentialInputQueue.o $(OBJ)/NewMemAlloc.o $(OBJ)/SequentialLP.o\
	$(OBJ)/SequentialObj.o	$(OBJ)/SequentialEventQueue.o \
	$(OBJ)/SplayTree.o $(OBJ)/ConfigurationManager.o

all: obj $(OBJ)/libTW.a

shared: obj $(OBJ)/libWarpedSeq1.so

install: install-headers install-library

install-library: shared
	$(MKINSTALLDIRS) $(libdir)
	$(INSTALL) $(OBJ)/libWarpedSeq1.so $(libdir)/libWarpedSeq1.so
	$(MAKE) -C ../../common install-library

install-headers:
	$(MKINSTALLDIRS) $(includedir)
	for f in *.hh; do $(INSTALL) $$f $(includedir); done
	$(MAKE) -C ../../common install-headers

obj:
	mkdir obj

$(LIBCOMMON):
	make -C $(COMMONDIR) 

$(OBJ)/libTW.a: $(SEQOBJS) $(LIBCOMMON)
	-rm -f $(OBJ)/libTW.a
	$(AR) cr $(OBJ)/libTW.a $(SEQOBJS) $(COMMONDIR)/src/*.o

$(OBJ)/libWarpedSeq1.so: $(SEQOBJS) $(LIBCOMMON)
	$(CXX) $(CXXFLAGS) -shared $^ $(LDFLAGS) -o $@

$(OBJ)/%.o : %.cc
	$(CXX) -c\
	$(CPPFLAGS) $(CXXFLAGS)\
	$< -o $@

$(OBJ)/%.o : util/%.cc
	$(CXX) -c\
	$(CPPFLAGS) $(CXXFLAGS)\
	$< -o $@

depend: cleandep
	makedepend -p$(SEQOBJ)/ -f- -- $(CPPFLAGS) $(CXXFLAGS) -- $(DEPENDINC) $(SEQSRCS) > .depend

cleandep:
	rm -f .depend

clean:
	rm -f *~ \#*\#\
	rm -f $(OBJ)/*.o
	rm -f $(OBJ)/libTW.a
	rm -f util/*.o
	make -C ../../common clean

-include .depend
