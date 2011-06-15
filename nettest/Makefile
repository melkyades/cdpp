#-*-makefile-*-
export 

CXX = g++

TOP = ..
TWDIR = /home/atroccol/tesis/PCD++/warped/NoTime/src
COMMON = $(TWDIR)/../../common
TWOBJ = $(TWDIR)/obj
PINGOBJ = ./obj
MPIDIR = /home/atroccol/mpich-1.2.0
CPPFLAGS = -I$(TWDIR) -I$(COMMON)/include -I$(MPIDIR)/include -DMPI -DHAS_BOOLEAN  -DUSE_USER_VTIME
#-DOBJECTDEBUG
#-DUSENONBLOCKINGMPICALLS\

#CXXFLAGS = -g -Wall -O2 -pipe
CXXFLAGS =  -O3 
LDFLAGS = -L$(MPIDIR)/lib/
LOADLIBES = -lm -lnsl -lmpich 
#DEPENDINC = -I/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include

SIMSRCS = main.cc\
	PingObject.cc

PINGOBJS = $(PINGOBJ)/main.o\
	$(PINGOBJ)/PingObject.o

.PHONY: $(TWOBJ)/libTW.a

all: $(PINGOBJ) nettest

$(PINGOBJ):
	mkdir $(PINGOBJ)

#Just build a normal executable
nettest: $(PINGOBJS) $(TWOBJ)/libNoTime.a
	$(CXX) -g $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

$(TWOBJ)/libTW.a:
	$(MAKE) -j6 -l5 -C $(TWDIR)

# rule to make all the process .o files
$(PINGOBJ)/%.o : %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS)\
		$< -o $@

depend:
	makedepend -p$(PINGOBJ)/ -- $(CPPFLAGS) $(CXXFLAGS) -- $(DEPENDINC) $(SIMSRCS)
	$(MAKE) -C $(TWDIR) depend
	$(MAKE) -C $(RNDDIR) depend

cleandep:
	$(TOP)/bin/cleandepend ./Makefile > ./Makefile.clean
	mv ./Makefile.clean ./Makefile
	$(MAKE) -C $(TWDIR) cleandep
	$(MAKE) -C $(RNDDIR) cleandep
clean:	
	rm -f *~ \#*\# PING
	rm -rf $(PINGOBJ)/
	$(MAKE) -C $(TWDIR) clean
	$(MAKE) -C $(RNDDIR) clean

it: clean depend PING

# DO NOT DELETE THIS LINE -- make depend depends on it.

