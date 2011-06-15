#-*-makefile-*-
include ../../Makefile.common
export 

CXX = g++

COMMONINCLUDE = ../../common/include
TOP = ..
TWDIR = $(TOP)/src
RNDDIR = $(TOP)/rnd
RNDOBJ = $(RNDDIR)/obj
RAIDOBJ = $(RNDDIR)/obj
TWLIBDIR = $(TWDIR)/obj

PUCKOBJ = $(TOP)/collidingPucks/obj

###########################################################################
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

# for solaris machines
#LDFLAGS = -L$(MPIDIR)/lib/solaris
#CPPFLAGS = -I$(RNDDIR) -I$(TWDIR) -I$(MPIDIR)/src -DTCPLIB -DSUNOS -DUSE_KERNEL_VTIME

# for linux machines
#LDFLAGS = -L$(MPIDIR)/lib/linux
#CPPFLAGS = -I$(RNDDIR) -I$(TWDIR) -I$(MPIDIR)/src -DTCPLIB -DLINUX -DUSE_KERNEL_VTIM

#LOADLIBES = -lnsl -lTCPMPL -lm -lsocket

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
	InitObject.cc\
	tableSector.cc\
	tableEvent.cc\
	tableSectorState.cc\
	UtilityFunc.cc\

PUCKOBJS = $(PUCKOBJ)/main.o\
	$(PUCKOBJ)/InitObject.o\
	$(PUCKOBJ)/tableSector.o\
        $(PUCKOBJ)/tableEvent.o\
	$(PUCKOBJ)/tableSectorState.o\
	$(PUCKOBJ)/UtilityFunc.o

all: $(PUCKOBJ) PUCK $(RNDOBJ)/libRnd.a

$(PUCKOBJ):
	mkdir $(PUCKOBJ)

#Just build a normal executable
PUCK: $(PUCKOBJS) $(RNDOBJ)/libRnd.a
	$(CXX) -g $^\
	-L$(TWLIBDIR) -lTW $(LDFLAGS) $(LIBS)\
	-o $@

PUCK.pure: $(PUCKOBJS) $(TWOBJ)/libTW.a $(RNDOBJ)/libRnd.a
	/local/packages/pure/bin/purify -g++ -follow-child-processes=yes -view-file=../collidingPucks/%v.%p.pv $(CXX) $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@


PUCK.quantify: $(PUCKOBJS) $(TWOBJ)/libTW.a $(RNDOBJ)/libRnd.a
	/local/packages/pure/bin/quantify \
	$(CXX) $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

$(RNDOBJ)/libRnd.a:
	$(MAKE) -C $(RNDDIR)

 # rule to make all the process .o files
$(PUCKOBJ)/%.o : %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS)\
		$< -o $@

depend:
	makedepend -p$(PUCKOBJ)/ -- $(CPPFLAGS) $(CXXFLAGS) -- $(DEPENDINC) $(SIMSRCS)
	$(MAKE) -C $(RNDDIR) depend

cleandep:
	$(TOP)/bin/cleandepend ./Makefile > ./Makefile.clean
	mv ./Makefile.clean ./Makefile
	$(MAKE) -C $(RNDDIR) cleandep

clean:	
	rm -f *~ \#*\# PUCK
	rm -rf $(PUCKOBJ)/
	$(MAKE) -C $(RNDDIR) clean

it: clean depend PUCK

# DO NOT DELETE THIS LINE -- make depend depends on it.
