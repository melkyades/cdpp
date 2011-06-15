#-*-makefile-*-

include ../../../Makefile.common

CPPFLAGS+= $(DEFINES_CPP) $(INCLUDES_CPP) 

COMMONDIR=../../common/
LIBCOMMON=$(COMMONDIR)/libcommon.a

ifndef TWOBJ
TWOBJ=obj
endif

ifndef DESTDIR
DESTDIR=/usr/local
endif

libdir = $(DESTDIR)/lib
includedir = $(DESTDIR)/include/warped

TWSRCS = StateQueue.cc OutputQueue.cc LogicalProcess.cc KernelMsgs.cc\
	GVTManager.cc warpedNewHandler.cc\
	AckRecord.cc TimeWarpBasicState.cc TimeWarpBasicEvent.cc \
	DynamicCancellation.cc BasicTimeWarp.cc\
	LTSFScheduler.cc LTSFInputQueue.cc\
	BuddyMemoryManager.cc SegregatedMemAlloc.cc BrentMemAlloc.cc \
	MatternGVTManager.cc GlobalMemoryManager.cc\
	TimeWarp.cc StateManager.cc \
	InfreqStateManager.cc AdaptStateManager.cc LinStateManager.cc\
	NashStateManager.cc CostFuncStateManager.cc \
	CommPhyInterface.cc CommPhyMPI.cc CommPhyTCPMPL.cc CommPhyDefault.cc\
	CommMgrInterface.cc CommMgrDefault.cc  MessageManager.cc \
	Transceiver.cc CommMgrMsgAgg.cc LogicalProcessStats.cc\
	CommPhyClumpsIRCM.cc CommPhyClumpsISCM.cc CommPhyClumpsCCM.cc \
	PthreadMutex.cc CommPhyClumpsDPC.cc UnixIPCSemaphore.cc \
	UnixIPCSharedMemory.cc CommPhyClumpsAggrISCM.cc

TWOBJS = $(TWOBJ)/StateQueue.o $(TWOBJ)/OutputQueue.o\
	$(TWOBJ)/LogicalProcess.o $(TWOBJ)/KernelMsgs.o\
	$(TWOBJ)/GVTManager.o $(TWOBJ)/warpedNewHandler.o \
	$(TWOBJ)/AckRecord.o\
	$(TWOBJ)/TimeWarpBasicState.o $(TWOBJ)/TimeWarpBasicEvent.o\
	$(TWOBJ)/BasicTimeWarp.o $(TWOBJ)/LTSFScheduler.o \
	$(TWOBJ)/LTSFInputQueue.o $(TWOBJ)/DynamicCancellation.o\
	$(TWOBJ)/BuddyMemoryManager.o $(TWOBJ)/SegregatedMemAlloc.o \
	$(TWOBJ)/MatternGVTManager.o $(TWOBJ)/BrentMemAlloc.o \
	$(TWOBJ)/GlobalMemoryManager.o\
	$(TWOBJ)/TimeWarp.o $(TWOBJ)/StateManager.o\
	$(TWOBJ)/InfreqStateManager.o\
	$(TWOBJ)/AdaptStateManager.o $(TWOBJ)/LinStateManager.o\
	$(TWOBJ)/NashStateManager.o $(TWOBJ)/CostFuncStateManager.o\
	$(TWOBJ)/CommPhyInterface.o\
	$(TWOBJ)/CommPhyMPI.o $(TWOBJ)/CommPhyTCPMPL.o\
	$(TWOBJ)/CommPhyDefault.o $(TWOBJ)/CommMgrInterface.o\
	$(TWOBJ)/CommMgrDefault.o $(TWOBJ)/MessageManager.o\
	$(TWOBJ)/Transceiver.o $(TWOBJ)/CommMgrMsgAgg.o \
	$(TWOBJ)/LogicalProcessStats.o\
	$(TWOBJ)/CommPhyClumpsIRCM.o $(TWOBJ)/CommPhyClumpsISCM.o \
	$(TWOBJ)/PthreadMutex.o $(TWOBJ)/CommPhyClumpsCCM.o \
	$(TWOBJ)/CommPhyClumpsDPC.o $(TWOBJ)/UnixIPCSemaphore.o \
	$(TWOBJ)/UnixIPCSharedMemory.o $(TWOBJ)/CommPhyClumpsAggrISCM.o 

# doc++ specific flags
HTMLPAGEFOOTER = htmlPageFooter
HTMLDIRNAME = html
DOCSRCS = AckRecord.hh AdaptStateManager.hh BasicTimeWarp.hh\
	 InfreqStateManager.hh

all: obj $(TWOBJ)/libTW.a

shared: obj $(TWOBJ)/libWarpedPar1.so

install: install-headers install-library

install-headers:
	$(MKINSTALLDIRS) $(includedir)
	for f in *.hh; do $(INSTALL) $$f $(includedir); done
	$(INSTALL) SortedListOfEvents.cc $(includedir)/
	$(INSTALL) MultiList.cc $(includedir)/
	$(INSTALL) OneShot.cc $(includedir)/
	$(MAKE) -C $(COMMONDIR) install-headers

install-library: shared
	$(MKINSTALLDIRS) $(libdir)
	$(INSTALL) $(TWOBJ)/libWarpedPar1.so $(libdir)/libWarpedPar1.so
	$(MAKE) -C $(COMMONDIR) install-library

obj:
	mkdir obj

$(LIBCOMMON):
	make -C $(COMMONDIR)

$(TWOBJ)/libWarpedPar1.so: $(TWOBJS) $(LIBCOMMON)
	$(CXX) $(CXXFLAGS) -shared $^ $(LDFLAGS) -o $@

$(TWOBJ)/libTW.a:  $(TWOBJS) $(LIBCOMMON)
	-rm -f $(TWOBJ)/libTW.a
	$(AR) cr $(TWOBJ)/libTW.a $(TWOBJS) $(COMMONDIR)/src/*.o

# Rule to make all the TimeWarp .o files
$(TWOBJ)/%.o : %.cc
	$(CXX) -c\
	$(CPPFLAGS) $(CXXFLAGS) $(CFLAGS)\
	$< -o $@

$(TWOBJ)/%.o : util/%.cc
	$(CXX) -c\
	$(CPPFLAGS) $(CXXFLAGS)\
	$< -o $@

depend: cleandep
	makedepend -p$(TWOBJ)/ -f- -- $(CPPFLAGS) $(CXXFLAGS) -- $(DEPENDINC) $(TWSRCS) > .depend

cleandep:
	-rm -f .depend

clean:
	rm -f *~ \#*\#\
	rm -rf $(TWOBJ)
	make -C $(COMMONDIR) clean

#doc++ rule to create the html manual 
doc:
	doc++ -p -v -b -B $(HTMLPAGEFOOTER) -d $(HTMLDIRNAME) -f $(DOCSRCS)

-include .depend

