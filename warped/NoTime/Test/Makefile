#-*-makefile-*-
export 

CXX = g++ 

TOP = ..
TWDIR = $(TOP)/src
TWOBJ = $(TWDIR)/obj
MPIDIR = /local/packages/parallel/mpi
CPPFLAGS = -I$(TWDIR) -I$(MPIDIR)/include -DUSE_KERNEL_VTIME -DHAS_BOOLEAN
CXXFLAGS = -g -O2 
LDFLAGS = -L$(MPIDIR)/lib/solaris/ch_p4
LOADLIBES = -lm
DEPENDINC = -I/local/packages/gnu/lib/g++-include 

SIMSRCS = main.cc
OBJS = main.o

.PHONY: $(TWOBJ)/libTW.a

all: TEST

#Just build a normal executable
TEST: $(OBJS) $(TWOBJ)/libTW.a
	$(CXX) -g $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

TEST.pure: $(OBJS) $(TWOBJ)/libTW.a
	/local/packages/pure/bin/purify -$(CXX) -windows=yes -follow-child-processes=yes -view-file=%v.%p.pv $(CXX) $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

TEST.quant: $(OBJS) $(TWOBJ)/libTW.a
	/home/common/pure/bin/quantify -follow-child-processes=yes \
	$(CXX) $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

$(TWOBJ)/libTW.a:
	$(MAKE) -C $(TWDIR)

%.o : %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS)\
		$< -o $@

depend:
	makedepend -p$(RAIDOBJ)/ -- $(CPPFLAGS) $(CXXFLAGS) -- $(DEPENDINC) $(SIMSRCS)
	$(MAKE) -C $(TWDIR) depend

cleandep:
	$(TOP)/bin/cleandepend ./Makefile > ./Makefile.clean
	mv ./Makefile.clean ./Makefile
	$(MAKE) -C $(TWDIR) cleandep

clean:	
	rm -f *~ \#*\# RAID
	rm -f *.o
	$(MAKE) -C $(TWDIR) clean

it: clean depend TEST

# DO NOT DELETE THIS LINE -- make depend depends on it.

