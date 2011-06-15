#-*-makefile-*-
export 

CXX = g++

TOP = ..
NTDIR = $(TOP)/src
NTOBJ = $(NTDIR)/obj
RNDDIR = ../../TimeWarp/rnd
RNDOBJ = $(RNDDIR)/obj
QUEUEOBJ = ./obj
MPIDIR = /local/packages/parallel/mpi
#MPIDIR = /local/mpich
#MPIDIR = /local/bip
CPPFLAGS = -I$(NTDIR) -I$(MPIDIR)/include -DUSE_KERNEL_VTIME -DMPI
CXXFLAGS = -g
LDFLAGS = -L$(MPIDIR)/lib/solaris/ch_p4
#LDFLAGS = -L$(MPIDIR)/lib -L$(MPIDIR)/lib/mpi_g77
LOADLIBES = -lm -lmpi -lnsl -lsocket
#LOADLIBES = -lm -lmpi -lbip -lbipt -ldio
DEPENDINC = -I/local/packages/gnu/lib/g++-include -I$(RNDDIR) -I$(NTDIR) -I/usr/include/g++ -I/usr/include/g++/std

SIMSRCS = main.cc\
	Server.cc\
	Queue.cc\
	QueueState.cc\
	Source.cc\
	StatCollector.cc\
	StatCollectorState.cc\
	ServerState.cc\
	ParseInput.cc

QUEUEOBJS = $(QUEUEOBJ)/main.o\
	$(QUEUEOBJ)/Server.o\
	$(QUEUEOBJ)/Queue.o\
	$(QUEUEOBJ)/QueueState.o\
	$(QUEUEOBJ)/Source.o\
	$(QUEUEOBJ)/StatCollector.o\
	$(QUEUEOBJ)/StatCollectorState.o\
	$(QUEUEOBJ)/ServerState.o\
	$(QUEUEOBJ)/ParseInput.o

.PHONY: $(NTOBJ)/libNoTime.a

all: $(QUEUEOBJ) QUEUE $(RNDOBJ)/libRnd.a

$(QUEUEOBJ):
	mkdir $(QUEUEOBJ)

#Just build a normal executable
QUEUE: $(QUEUEOBJS) $(NTOBJ)/libNoTime.a $(RNDOBJ)/libRnd.a
	$(CXX) -g $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

QUEUE.pure: $(QUEUEOBJS) $(NTOBJ)/libNoTime.a $(RNDOBJ)/libRnd.a
	/local/packages/pure/bin/purify -g++ -windows=yes -follow-child-processes=yes -view-file=./%v.%p.pv $(CXX) $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

QUEUE.quant: $(QUEUEOBJS) $(NTOBJ)/libNoTime.a $(RNDOBJ)/libRnd.a
	/local/packages/pure/bin/quantify \
	$(CXX) $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

$(NTOBJ)/libNoTime.a:
	$(MAKE) -j3 -l2 -C $(NTDIR)

$(RNDOBJ)/libRnd.a:
	$(MAKE) -C $(RNDDIR)

# rule to make all the process .o files
$(QUEUEOBJ)/%.o : %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS)\
		$< -o $@

depend:
	makedepend -p$(QUEUEOBJ)/ -- $(CPPFLAGS) $(CXXFLAGS) -- $(DEPENDINC) $(SIMSRCS)
	$(MAKE) -C $(NTDIR) depend
	$(MAKE) -C $(RNDDIR) depend

cleandep:
	$(TOP)/bin/cleandepend ./Makefile > ./Makefile.clean
	mv ./Makefile.clean ./Makefile
	$(MAKE) -C $(NTDIR) cleandep
	$(MAKE) -C $(RNDDIR) cleandep
clean:	
	rm -f *~ \#*\# QUEUE QUEUE.pure QUEUE.quant
	rm -rf $(QUEUEOBJ)/
	$(MAKE) -C $(NTDIR) clean
	$(MAKE) -C $(RNDDIR) clean

it: clean depend QUEUE

# DO NOT DELETE THIS LINE -- make depend depends on it.
