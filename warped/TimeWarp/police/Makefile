#-*-makefile-*-
export

CXX = g++

TOP = ..
TWDIR = $(TOP)/src
TWOBJ = $(TWDIR)/obj
RNDDIR = $(TOP)/rnd
RNDOBJ = $(RNDDIR)/obj
QDIR = $(TOP)/objectLib
QOBJ = $(QDIR)/obj

SIMOBJ = obj

MPIDIR = /usr/local/mpich-1.2.0

CPPFLAGS = + -I../src -I$(QDIR) -I$(MPIDIR)/include -I../../common/include -DUSE_KERNEL_VTIME -DMPI
CXXFLAGS = -g -Wall
LDFLAGS = -L$(MPIDIR)/lib/LINUX/ch_p4
LOADLIBES = -lmpich -lnsl -lm
DEPENDINC = -I/usr/include/g++

SIMSRCS = main.cc\
	PoliceSourceObject.cc\
	PoliceForkObject.cc\
	PoliceSinkObject.cc\
	PoliceServerObject.cc\
	PoliceSourceState.cc \
	PoliceForkState.cc 

SIMOBJS = $(SIMOBJ)/main.o\
	$(SIMOBJ)/PoliceSourceObject.o\
	$(SIMOBJ)/PoliceForkObject.o\
	$(SIMOBJ)/PoliceSinkObject.o\
	$(SIMOBJ)/PoliceServerObject.o \
	$(SIMOBJ)/PoliceSourceState.o \
	$(SIMOBJ)/PoliceForkState.o

.PHONY: $(TWOBJ)/libTW.a
.PHONY: $(QOBJ)/libQ.a

all: $(SIMOBJ) police $(RNDOBJ)/libRnd.a

#Just build a normal executable
police: $(SIMOBJS) $(QOBJ)/libQ.a $(TWOBJ)/libTW.a $(RNDOBJ)/libRnd.a
	$(CXX) $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

police.pure: $(SIMOBJS) $(QOBJ)/libQ.a $(TWOBJ)/libTW.a $(RNDOBJ)/libRnd.a
	/local/packages/pure/bin/purify -follow-child-processes=yes -view-file=../police/%v.%p.pv -g++ $(CXX) $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

police.quant: $(SIMOBJS) $(QOBJ)/libQ.a $(TWOBJ)/libTW.a $(RNDOBJ)/libRnd.a
	/local/packages/pure/bin/quantify\
	$(CXX) $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

$(TWOBJ)/libTW.a:
	$(MAKE) -C $(TWDIR)

$(RNDOBJ)/libRnd.a:
	$(MAKE) -C $(RNDDIR)

$(QOBJ)/libQ.a:
	$(MAKE) -C $(QDIR)

# rule to make all the process .o files
$(SIMOBJ)/%.o : %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS)\
		$< -o $@

$(SIMOBJ):
	mkdir $(SIMOBJ)

depend:
	makedepend -p$(SIMOBJ)/ -- $(CPPFLAGS) $(CXXFLAGS) -- $(DEPENDINC) $(SIMSRCS)
	$(MAKE) -C $(TWDIR) depend
	$(MAKE) -C $(QDIR) depend
	$(MAKE) -C $(RNDDIR) depend

cleandep:
	$(TOP)/bin/cleandepend ./Makefile > ./Makefile.clean
	mv ./Makefile.clean ./Makefile
	$(MAKE) -C $(QDIR) cleandep
	$(MAKE) -C $(RNDDIR) cleandep

clean:	
	rm -f *~ \#*\# police
	rm -rf $(SIMOBJ)/
	$(MAKE) -C $(QDIR) clean
	$(MAKE) -C $(TWDIR) clean
	$(MAKE) -C $(RNDDIR) clean

it: clean depend dummy

# DO NOT DELETE THIS LINE -- make depend depends on it.

