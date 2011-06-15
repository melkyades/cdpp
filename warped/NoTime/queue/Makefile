#-*-makefile-*-
export 

CXX = g++

TOP = ../../TimeWarp
TWDIR = $(TOP)/src
TWOBJ = $(TWDIR)/obj
RNDDIR = ../../TimeWarp/rnd
RNDOBJ = $(RNDDIR)/obj
QUEUEOBJ = ./obj
CXXFLAGS = -g
############################################################################
##### for running with mpi and TimeWarp Kernel #############################
############################################################################

# for solaris machines
#MPIDIR = /local/packages/parallel/mpi

# for linux machines
MPIDIR = /local/packages/mpich

CPPFLAGS = -I$(RNDDIR) -I$(TWDIR) -I$(MPIDIR)/include -DMPI -DUSE_KERNEL_VTIME 
LDFLAGS = -L$(MPIDIR)/lib/LINUX/ch_p4
LOADLIBES = -lmpi -lnsl -lm 

############################################################################
##### for running sequential kernel ########################################
############################################################################

#CPPFLAGS = -I$(RNDDIR) -I$(TWDIR) -DUSE_KERNEL_VTIME -DSEQUENTIAL

############################################################################
##### for running tcpmpl on linux ##########################################
############################################################################

#MPIDIR = /home/urajasek/tcpmpl

# for solaris machines
#LDFLAGS = -L$(MPIDIR)/lib/solaris
#CPPFLAGS = -I$(RNDDIR) -I$(TWDIR) -I$(MPIDIR)/src -DTCPLIB -DSUNOS -DUSE_KERNEL_VTIME

# for linux machines
#LDFLAGS = -L$(MPIDIR)/lib/linux
#CPPFLAGS = -I$(RNDDIR) -I$(TWDIR) -I$(MPIDIR)/src -DTCPLIB -DLINUX -DUSE_KERNEL_VTIME

#LOADLIBES = -lnsl -lTCPMPL -lm 

############################################################################
##### for running with MPI-BIP #############################################
############################################################################

#MPIDIR = /local/bip
#CPPFLAGS = -I$(RNDDIR) -I$(TWDIR) -I$(MPIDIR)/include -I$(MPIDIR)/include/mpi -DMPI -DUSE_KERNEL_VTIME 
#LDFLAGS = -L$(MPIDIR)/lib/ -L$(MPIDIR)/lib/mpi_g77
#LOADLIBES = -lmpi -lbip -lbipt -ldio -lnsl -lm

DEPENDINC = -I/local/packages/gnu/lib/g++-include -I$(RNDDIR) -I$(NTDIR) -I/usr/include/g++ -I/usr/include/g++/std

SIMSRCS = main.cc\
	Server.cc\
	ServerState.cc\
	Queue.cc\
	QueueState.cc\
	Source.cc\
	SourceState.cc\
	StatCollector.cc\
	StatCollectorState.cc\
	ParseInput.cc

QUEUEOBJS = $(QUEUEOBJ)/main.o\
	$(QUEUEOBJ)/Server.o\
	$(QUEUEOBJ)/ServerState.o\
	$(QUEUEOBJ)/Queue.o\
	$(QUEUEOBJ)/QueueState.o\
	$(QUEUEOBJ)/Source.o\
	$(QUEUEOBJ)/SourceState.o\
	$(QUEUEOBJ)/StatCollector.o\
	$(QUEUEOBJ)/StatCollectorState.o\
	$(QUEUEOBJ)/ParseInput.o

.PHONY: $(TWOBJ)/libTW.a

all: $(QUEUEOBJ) QUEUE $(RNDOBJ)/libRnd.a

$(QUEUEOBJ):
	mkdir $(QUEUEOBJ)

#Just build a normal executable
QUEUE: $(QUEUEOBJS) $(TWOBJ)/libTW.a $(RNDOBJ)/libRnd.a
	$(CXX) -g $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

QUEUE.pure: $(QUEUEOBJS) $(TWOBJ)/libTW.a $(RNDOBJ)/libRnd.a
	/local/packages/pure/bin/purify -g++ -windows=yes -follow-child-processes=yes -view-file=./%v.%p.pv $(CXX) $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

QUEUE.quant: $(QUEUEOBJS) $(TWOBJ)/libTW.a $(RNDOBJ)/libRnd.a
	/local/packages/pure/bin/quantify \
	$(CXX) $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

$(TWOBJ)/libTW.a:
	$(MAKE) -j3 -l2 -C $(TWDIR)

$(RNDOBJ)/libRnd.a:
	$(MAKE) -C $(RNDDIR)

# rule to make all the process .o files
$(QUEUEOBJ)/%.o : %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS)\
		$< -o $@

depend:
	makedepend -p$(QUEUEOBJ)/ -- $(CPPFLAGS) $(CXXFLAGS) -- $(DEPENDINC) $(SIMSRCS)
	$(MAKE) -C $(TWDIR) depend
	$(MAKE) -C $(RNDDIR) depend

cleandep:
	$(TOP)/bin/cleandepend ./Makefile > ./Makefile.clean
	mv ./Makefile.clean ./Makefile
	$(MAKE) -C $(TWDIR) cleandep
	$(MAKE) -C $(RNDDIR) cleandep
clean:	
	rm -f *~ \#*\# QUEUE QUEUE.pure QUEUE.quant
	rm -rf $(QUEUEOBJ)/
	$(MAKE) -C $(TWDIR) clean
	$(MAKE) -C $(RNDDIR) clean

it: clean depend QUEUE

# DO NOT DELETE THIS LINE -- make depend depends on it.
