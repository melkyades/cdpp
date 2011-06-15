#-*-makefile-*-

include ../../../Makefile.common

COMMONINCLUDE = ../../common/include
TWINCLUDE = ../src
UTILDIR = ../src/util
QDIR = ../objectLib
RNDDIR = ../rnd
RNDOBJ = $(RNDDIR)/obj
QOBJ = $(QDIR)/obj
SIMOBJ = obj
TWLIBDIR = $(TWINCLUDE)/obj

###########################################################################
##### for running with mpi and TimeWarp Kernel #############################
############################################################################

# for solaris machines
# export MPIDIR = /local/packages/parallel/mpi

# for linux machines
 export MPIDIR = /local/usr/mpich-1.2.0 

export CPPFLAGS += -I$(COMMONINCLUDE) -I$(TWINCLUDE) -I$(UTILDIR) -I$(QDIR) -I$(RNDDIR) -DUSE_KERNEL_VTIME -DMPI
# export LDFLAGS = -L$(MPIDIR)/lib/solaris/ch_p4

############################################################################
##### for running sequential kernel ########################################
############################################################################

#export CPPFLAGS = -I$(TWDIR) -I$(UTILDIR) -I$(QDIR) -DUSE_KERNEL_VTIME -DSEQUENTIAL

############################################################################
##### for running tcpmpl on linux ##########################################
############################################################################
# -DSUNOS for solaris and -DLINUX for linux
#export MPIDIR = /home/urajasek/tcpmpl
#export CPPFLAGS = -I$(TWDIR) -I$(UTILDIR) -I$(QDIR) -I$(MPIDIR)/src -DTCPLIB -DSUNOS -DUSE_KERNEL_VTIME

#for solaris machines
#export LDFLAGS = -L$(MPIDIR)/lib/solaris/

# for linux machines
#export LDFLAGS = -L$(MPIDIR)/lib/linux/

#export LOADLIBES = -lnsl -lTCPMPL -lm -lsocket

############################################################################
##### for running with MPI-BIP #############################################
############################################################################

#export MPIDIR = /local/bip
#export CPPFLAGS = -I$(TWDIR) -I$(UTILDIR) -I$(QDIR) -I$(MPIDIR)/include -I$(MPIDIR)/include/mpi -DMPI -DUSE_KERNEL_VTIME 
#export LDFLAGS = -L$(MPIDIR)/lib/ -L$(MPIDIR)/lib/mpi_g77
#export LOADLIBES = -lmpi -lbip -lbipt -ldio -lnsl -lm



export CXXFLAGS = -g -Wall -O2 -pipe
export DEPENDINC = -I/local/packages/gnu/lib/g++-include -I/usr/include/g++

SIMSRCS = main.cc\
	MemSourceObject.cc\
	MemRouterObject.cc\
	MemSourceState.cc

SIMOBJS = $(SIMOBJ)/main.o\
	$(SIMOBJ)/MemSourceObject.o\
	$(SIMOBJ)/MemRouterObject.o\
	$(SIMOBJ)/MemSourceState.o

.PHONY: $(QOBJ)/libQ.a

all: $(SIMOBJ) SMMP $(RNDOBJ)/libRnd.a

#Just build a normal executable
SMMP: $(SIMOBJS) $(QOBJ)/libQ.a $(RNDOBJ)/libRnd.a
	$(CXX) $^\
	-L$(TWLIBDIR) -lTW $(LDFLAGS) $(LIBS)\
	-o $@

SMMP.pure: $(SIMOBJS) $(QOBJ)/libQ.a  $(RNDOBJ)/libRnd.a
	/local/packages/pure/bin/purify -follow-child-processes=yes -view-file=/work/dmadhava/%v.%p.pv -windows=yes -g++ $(CXX) $^\
	-L$(TWLIBDIR) -lTW $(LDFLAGS) $(LIBS)\
	-o $@

SMMP.quant: $(SIMOBJS) $(QOBJ)/libQ.a $(RNDOBJ)/libRnd.a
	/local/packages/pure/bin/quantify -view-file=/work/dmadhava/%v.%p.qv -g++ $(CXX) -windows=yes $^\
	-L$(TWLIBDIR) -lTW $(LDFLAGS)\
	-o $@

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

main.cc: MemGen
	./MemGen

MemGen: MemGen.cc
	g++ -ggdb MemGen.cc -o MemGen

depend:
	makedepend -p$(SIMOBJ)/ -- $(CPPFLAGS) $(CXXFLAGS) -- $(DEPENDINC) $(SIMSRCS)
	$(MAKE) -C $(QDIR) depend
	$(MAKE) -C $(RNDDIR) depend

cleandep:
	$(TOP)/bin/cleandepend ./Makefile > ./Makefile.clean
	mv ./Makefile.clean ./Makefile
	$(MAKE) -C $(QDIR) cleandep
	$(MAKE) -C $(RNDDIR) cleandep

clean:	
	rm -f *~ \#*\# SMMP
	rm -rf $(SIMOBJ)/
	$(MAKE) -C $(QDIR) clean
	$(MAKE) -C $(RNDDIR) clean

it: clean depend SMMP

# DO NOT DELETE THIS LINE -- make depend depends on it.
