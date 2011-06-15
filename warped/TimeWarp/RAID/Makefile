#-*-makefile-*-
include ../../../Makefile.common
export 

CXX = g++

COMMONINCLUDE = ../../common/include
TOP = ..
TWDIR = $(TOP)/src
RNDDIR = $(TOP)/rnd
RNDOBJ = $(RNDDIR)/obj
RAIDOBJ = $(RNDDIR)/obj
TWLIBDIR = $(TWDIR)/obj

############################################################################
##### for running with mpi and TimeWarp Kernel #############################
############################################################################

# for solaris machines
#MPIDIR = /local/packages/parallel/mpi

# for linux machines
#MPIDIR = /local/packages/mpich 

CPPFLAGS += -I$(COMMONINCLUDE) -I$(RNDDIR) -I$(TWDIR) -DUSE_KERNEL_VTIME 
#LDFLAGS = -L$(MPIDIR)/lib/solaris/ch_p4
#LOADLIBES = -lmpi -lnsl -lm -lsocket

############################################################################
##### for running sequential kernel ########################################
############################################################################

#CPPFLAGS = -I$(RNDDIR) -I$(TWDIR) -DUSE_KERNEL_VTIME -DSEQUENTIAL

############################################################################
##### for running tcpmpl on linux ##########################################
############################################################################

#MPIDIR = /home/urajasek/tcpmpl
#CPPFLAGS = -I$(RNDDIR) -I$(TWDIR) -I$(MPIDIR)/src -DTCPLIB -DLINUX -DUSE_KERNEL_VTIME

# for solaris machines
#LDFLAGS = -L$(MPIDIR)/lib/solaris/

# for linux machines
#LDFLAGS = -L$(MPIDIR)/lib/linux/

#LOADLIBES = -lnsl -lTCPMPL -lm

############################################################################
##### for running with MPI-BIP #############################################
############################################################################

#MPIDIR = /local/bip
#CPPFLAGS = -I$(RNDDIR) -I$(TWDIR) -I$(MPIDIR)/include -I$(MPIDIR)/include/mpi -DMPI -DUSE_KERNEL_VTIME 
#LDFLAGS = -L$(MPIDIR)/lib/ -L$(MPIDIR)/lib/mpi_g77
#LOADLIBES = -lmpi -lbip -lbipt -ldio -lnsl -lm

CXXFLAGS = -g -Wall -O2 -pipe

DEPENDINC = -I/local/packages/gnu/lib/g++-include -I/usr/include/g++

SIMSRCS = main.cc\
	RAIDProcess.cc\
	RAIDFork.cc\
        RAIDDisk.cc\
	RAIDDiskState.cc\
	RAIDProcessState.cc\
	RAIDForkState.cc

RAIDOBJS = $(RAIDOBJ)/main.o\
	$(RAIDOBJ)/RAIDProcess.o\
	$(RAIDOBJ)/RAIDFork.o\
        $(RAIDOBJ)/RAIDDisk.o\
	$(RAIDOBJ)/RAIDDiskState.o\
	$(RAIDOBJ)/RAIDProcessState.o\
	$(RAIDOBJ)/RAIDForkState.o

all: $(RAIDOBJ) RAID $(RNDOBJ)/libRnd.a

$(RAIDOBJ):
	mkdir $(RAIDOBJ)

#Just build a normal executable
RAID: $(RAIDOBJS) $(RNDOBJ)/libRnd.a
	$(CXX) -g $^\
	-L$(TWLIBDIR) -lTW $(LDFLAGS) $(LIBS)\
	-o $@

RAID.pure: $(RAIDOBJS) $(RNDOBJ)/libRnd.a
	/local/packages/pure/bin/purify -g++ -windows=yes -follow-child-processes=yes -view-file=../RAID/%v.%p.pv $(CXX) $^\
	-L$(TWLIBDIR) -lTW $(LDFLAGS) $(LOADLIBES)\
	-o $@

RAID.quantify: $(RAIDOBJS) $(RNDOBJ)/libRnd.a
	/home/common/pure/solaris/quantify-2.0.1-solaris2/quantify \
	$(CXX) $^\
	-L$(TWLIBDIR) -lTW $(LDFLAGS) $(LOADLIBES)\
	-o $@

$(RNDOBJ)/libRnd.a:
	$(MAKE) -C $(RNDDIR)

 # rule to make all the process .o files
$(RAIDOBJ)/%.o : %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS)\
		$< -o $@

depend:
	makedepend -p$(RAIDOBJ)/ -- $(CPPFLAGS) $(CXXFLAGS) -- $(DEPENDINC) $(SIMSRCS)
	$(MAKE) -C $(RNDDIR) depend

cleandep:
	$(TOP)/bin/cleandepend ./Makefile > ./Makefile.clean
	mv ./Makefile.clean ./Makefile
	$(MAKE) -C $(RNDDIR) cleandep
clean:	
	rm -f *~ \#*\# RAID
	rm -rf $(RAIDOBJ)/
	$(MAKE) -C $(RNDDIR) clean

it: clean depend RAID

# DO NOT DELETE THIS LINE -- make depend depends on it.
