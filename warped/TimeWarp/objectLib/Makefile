#-*-makefile-*-

AR = ar cr

ifndef QOBJ
QOBJ=obj
endif

QLIBSRCS = SourceObject.cc\
	ForkObject.cc\
        JoinObject.cc\
        QueueObject.cc\
        SinkObject.cc\
        ServerObject.cc\
	DelayObject.cc\
	DelayState.cc\
	ForkState.cc\
	JoinState.cc\
	QueueState.cc\
	ServerState.cc\
	SinkState.cc\
	SourceState.cc

QLIBOBJS = $(QOBJ)/SourceObject.o\
	$(QOBJ)/ForkObject.o\
	$(QOBJ)/JoinObject.o\
	$(QOBJ)/QueueObject.o\
        $(QOBJ)/SinkObject.o\
        $(QOBJ)/ServerObject.o\
        $(QOBJ)/DelayObject.o\
	$(QOBJ)/DelayState.o\
	$(QOBJ)/ForkState.o\
	$(QOBJ)/JoinState.o\
	$(QOBJ)/QueueState.o\
	$(QOBJ)/ServerState.o\
	$(QOBJ)/SinkState.o\
	$(QOBJ)/SourceState.o


all: $(QOBJ) $(QOBJ)/libQ.a 

# rule to make all the process .o files
$(QOBJ)/libQ.a : $(QLIBOBJS)
	$(AR) $(QOBJ)/libQ.a $?

$(QOBJ)/%.o: %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS)\
		$< -o $@

$(QOBJ):
	mkdir $(QOBJ)

depend:
	makedepend -p$(QOBJ)/ -- $(CPPFLAGS) $(CXXFLAGS) -- $(DEPENDINC) $(QLIBSRCS)

cleandep:
	$(TOP)/bin/cleandepend ./Makefile > ./Makefile.clean
	mv ./Makefile.clean ./Makefile

clean:	
	rm -f *~ \#*\#
	rm -f $(QOBJ)/*.o $(QOBJ)/libQ.a


it: clean depend

# DO NOT DELETE THIS LINE -- make depend depends on it.

