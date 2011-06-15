#-*-makefile-*-
export 

CXX = g++

TOP = ../../TimeWarp
TWDIR = $(TOP)/src
TWOBJ = $(TWDIR)/obj
RNDDIR = ../../TimeWarp/rnd
RNDOBJ = $(RNDDIR)/obj
NETOBJ = ./obj
#MPIDIR = /local/packages/parallel/mpi
MPIDIR = /local/mpich
#MPIDIR = /local/bip
CPPFLAGS = -I$(TWDIR) -I$(MPIDIR)/include -DMPI -DUSE_KERNEL_VTIME
CXXFLAGS = -g
LDFLAGS = -L$(MPIDIR)/lib/LINUX/ch_p4 
#LDFLAGS = -L$(MPIDIR)/lib -L$(MPIDIR)/lib/mpi_g77
LOADLIBES = -lmpi -lm -lnsl 
#LOADLIBES = -lm -lmpi -lbip -lbipt -ldio -lnsl
DEPENDINC = -I/local/packages/gnu/lib/g++-include -I$(RNDDIR) -I/usr/include/g++ -I/usr/inlcude/g++/std

SIMSRCS = main.cc\
	Node.cc\
	NodeState.cc\
	Router.cc\
	RouteState.cc\
	BuffLessRouter.cc\
	BuffLessRouteState.cc\
	StatCollector.cc\
	StatCollState.cc\
	ConfigNet.cc

NETOBJS = $(NETOBJ)/main.o\
	$(NETOBJ)/Node.o\
	$(NETOBJ)/NodeState.o\
	$(NETOBJ)/Router.o\
	$(NETOBJ)/RouteState.o\
	$(NETOBJ)/BuffLessRouter.o\
	$(NETOBJ)/BuffLessRouteState.o\
	$(NETOBJ)/StatCollector.o\
	$(NETOBJ)/StatCollState.o\
	$(NETOBJ)/ConfigNet.o

.PHONY: $(TWOBJ)/libTW.a

all: $(NETOBJ) NET $(RNDOBJ)/libRnd.a

$(NETOBJ):
	mkdir $(NETOBJ)

#Just build a normal executable
NET: $(NETOBJS) $(TWOBJ)/libTW.a $(RNDOBJ)/libRnd.a
	$(CXX) -g $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

NET.pure: $(NETOBJS) $(TWOBJ)/libTW.a $(RNDOBJ)/libRnd.a
	/local/packages/pure/bin/purify -g++ -windows=yes -follow-child-processes=yes -view-file=./%v.%p.pv $(CXX) $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

NET.quant: $(NETOBJS) $(TWOBJ)/libTW.a $(RNDOBJ)/libRnd.a
	/local/packages/pure/bin/quantify \
	$(CXX) $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

$(TWOBJ)/libTW.a:
	$(MAKE) -j3 -l2 -C $(TWDIR)

$(RNDOBJ)/libRnd.a:
	$(MAKE) -C $(RNDDIR)

# rule to make all the process .o files
$(NETOBJ)/%.o : %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS)\
		$< -o $@

depend:
	makedepend -p$(NETOBJ)/ -- $(CPPFLAGS) $(CXXFLAGS) -- $(DEPENDINC) $(SIMSRCS)
	$(MAKE) -C $(TWDIR) depend
	$(MAKE) -C $(RNDDIR) depend

cleandep:
	$(TOP)/bin/cleandepend ./Makefile > ./Makefile.clean
	mv ./Makefile.clean ./Makefile
	$(MAKE) -C $(TWDIR) cleandep
	$(MAKE) -C $(RNDDIR) cleandep
clean:	
	rm -f *~ \#*\# NET NET.pure NET.quant
	rm -rf $(NETOBJ)/
	$(MAKE) -C $(TWDIR) clean
	$(MAKE) -C $(RNDDIR) clean

it: clean depend NET

# DO NOT DELETE THIS LINE -- make depend depends on it.
