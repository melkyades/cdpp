#-*-makefile-*-
export 

CXX = g++

TOP = ..
TWDIR = $(TOP)/src
COMMON = ../../common
TWOBJ = $(TWDIR)/obj
RNDDIR = $(TOP)/rnd
RNDOBJ = $(RNDDIR)/obj
PINGOBJ = ./obj
#MPIDIR = /vast/dmartin/download/mpich.ch_p4
MPIDIR = /home/atroccol/mpich-1.2.0
CPPFLAGS = -I$(TWDIR) -I$(COMMON)/include -I$(MPIDIR)/include -DMPI -DUSE_USER_VTIME -DHAS_BOOLEAN -DBEST_PARTITION -DAPPL_DEBUG 
#-DOBJECTDEBUG
#-DUSENONBLOCKINGMPICALLS\

#CXXFLAGS = -g -Wall -O2 -pipe
CXXFLAGS = -g -O2 
LDFLAGS = -L$(MPIDIR)/lib/
LOADLIBES = -lm -lnsl -lmpich 
#DEPENDINC = -I/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include

SIMSRCS = main.cc\
	PingObject.cc

PINGOBJS = $(PINGOBJ)/main.o\
	$(PINGOBJ)/PingObject.o

.PHONY: $(TWOBJ)/libTW.a

all: $(PINGOBJ) PING $(RNDOBJ)/libRnd.a

$(PINGOBJ):
	mkdir $(PINGOBJ)

#Just build a normal executable
PING: $(PINGOBJS) $(TWOBJ)/libTW.a $(RNDOBJ)/libRnd.a 
	$(CXX) -g $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

PING.pure: $(PINGOBJS) $(TWOBJ)/libTW.a $(RNDOBJ)/libRnd.a
	/local/packages/pure/bin/purify -g++ -follow-child-processes=yes -view-file=../PING/%v.%p.pv $(CXX) $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

PING.quantify: $(PINGOBJS) $(TWOBJ)/libTW.a $(RNDOBJ)/libRnd.a
	/home/common/pure/solaris/quantify-2.0.1-solaris2/quantify \
	$(CXX) $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

$(TWOBJ)/libTW.a:
	$(MAKE) -j6 -l5 -C $(TWDIR)

$(RNDOBJ)/libRnd.a:
	$(MAKE) -C $(RNDDIR)

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

