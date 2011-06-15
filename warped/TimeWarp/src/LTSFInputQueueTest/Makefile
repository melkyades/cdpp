#-*-makefile-*-
AR = ar cr
TWOBJ = /home/cyoung/warped/src/obj
CXXFLAGS = -g -Wall -O2
DEPENDINC = -I/local/packages/gnu/lib/g++-include
MPIDIR = /vast/dmartin/download/mpich.ch_p4
LDFLAGS = -L$(MPIDIR)/lib/solaris/ch_p4
LOADLIBS = -lmpi -lsocket -lnsl -lm
CPPFLAGS = -I$(TWDIR) -I$(MPIDIR)/include -DMPI -DFINALSTATS -DCOMMITSTATS

TWSRCS = BasicEvent.cc Container.cc LTSFInputQueue.cc StateQueue.cc\
	BasicState.cc


TWOBJS = $(TWOBJ)/BasicEvent.o $(TWOBJ)/Container.o \
	$(TWOBJ)/LTSFInputQueue.o $(TWOBJ)/StateQueue.o\
	$(TWOBJ)/BasicState.o


all: main

main: main.cc $(TWOBJ)/libTW.a
	$(CXX) -g $^\
	$(LDFLAGS) $(LOADLIBES)\
	-o $@

obj:
	mkdir obj

$(TWOBJ)/libTW.a: $(TWOBJS)
	-rm -f $(TWOBJ)/libTW.a
	$(AR) $(TWOBJ)/libTW.a $^

$(TWOBJ)/Container.o : Container.cc
	$(CXX) -c \
	$(CPPFLAGS) $(CXXFLAGS)\
	$< -o $@

# Rule to make all the TimeWarp .o files
$(TWOBJ)/%.o : %.cc
	$(CXX) -c\
	$(CPPFLAGS) $(CXXFLAGS)\
	$< -o $@

$(TWOBJ)/%.o : util/%.cc
	$(CXX) -c\
	$(CPPFLAGS) $(CXXFLAGS)\
	$< -o $@

depend:
	makedepend -p$(TWOBJ)/ -- $(CPPFLAGS) $(CXXFLAGS) -- $(DEPENDINC) $(TWSRCS) 

cleandep:
	../bin/cleandepend ./Makefile > ./Makefile.clean
	mv ./Makefile.clean ./Makefile

clean:
	rm -f *~ \#*\#\
	rm -rf $(TWOBJ)

# DO NOT DELETE THIS LINE -- make depend depends on it.

/home/cyoung/warped/src/obj/BasicEvent.o: BasicEvent.hh /usr/include/stddef.h
/home/cyoung/warped/src/obj/BasicEvent.o: warped.hh
/home/cyoung/warped/src/obj/BasicEvent.o: /local/packages/gnu/lib/g++-include/iostream.h
/home/cyoung/warped/src/obj/BasicEvent.o: /local/packages/gnu/lib/g++-include/streambuf.h
/home/cyoung/warped/src/obj/BasicEvent.o: /local/packages/gnu/lib/g++-include/libio.h
/home/cyoung/warped/src/obj/BasicEvent.o: /local/packages/gnu/lib/g++-include/_G_config.h
/home/cyoung/warped/src/obj/BasicEvent.o: /local/packages/gnu/lib/g++-include/strstream.h
/home/cyoung/warped/src/obj/BasicEvent.o: /local/packages/gnu/lib/g++-include/strfile.h
/home/cyoung/warped/src/obj/BasicEvent.o: /local/packages/gnu/lib/g++-include/string.h
/home/cyoung/warped/src/obj/BasicEvent.o: /local/packages/gnu/lib/g++-include/cstring
/home/cyoung/warped/src/obj/BasicEvent.o: /local/packages/gnu/lib/g++-include/std/cstring.h
/home/cyoung/warped/src/obj/BasicEvent.o: /usr/include/values.h
/home/cyoung/warped/src/obj/BasicEvent.o: /usr/include/ctype.h
/home/cyoung/warped/src/obj/BasicEvent.o: /usr/include/sys/feature_tests.h
/home/cyoung/warped/src/obj/BasicEvent.o: config.hh DefaultVTime.hh
/home/cyoung/warped/src/obj/Container.o: Container.hh warped.hh
/home/cyoung/warped/src/obj/Container.o: /local/packages/gnu/lib/g++-include/iostream.h
/home/cyoung/warped/src/obj/Container.o: /local/packages/gnu/lib/g++-include/streambuf.h
/home/cyoung/warped/src/obj/Container.o: /local/packages/gnu/lib/g++-include/libio.h
/home/cyoung/warped/src/obj/Container.o: /local/packages/gnu/lib/g++-include/_G_config.h
/home/cyoung/warped/src/obj/Container.o: /local/packages/gnu/lib/g++-include/strstream.h
/home/cyoung/warped/src/obj/Container.o: /local/packages/gnu/lib/g++-include/strfile.h
/home/cyoung/warped/src/obj/Container.o: /local/packages/gnu/lib/g++-include/string.h
/home/cyoung/warped/src/obj/Container.o: /local/packages/gnu/lib/g++-include/cstring
/home/cyoung/warped/src/obj/Container.o: /local/packages/gnu/lib/g++-include/std/cstring.h
/home/cyoung/warped/src/obj/Container.o: /usr/include/values.h
/home/cyoung/warped/src/obj/Container.o: /usr/include/ctype.h
/home/cyoung/warped/src/obj/Container.o: /usr/include/sys/feature_tests.h
/home/cyoung/warped/src/obj/Container.o: config.hh DefaultVTime.hh
/home/cyoung/warped/src/obj/LTSFInputQueue.o: LTSFInputQueue.hh config.hh
/home/cyoung/warped/src/obj/LTSFInputQueue.o: /local/packages/gnu/lib/g++-include/fstream.h
/home/cyoung/warped/src/obj/LTSFInputQueue.o: /local/packages/gnu/lib/g++-include/iostream.h
/home/cyoung/warped/src/obj/LTSFInputQueue.o: /local/packages/gnu/lib/g++-include/streambuf.h
/home/cyoung/warped/src/obj/LTSFInputQueue.o: /local/packages/gnu/lib/g++-include/libio.h
/home/cyoung/warped/src/obj/LTSFInputQueue.o: /local/packages/gnu/lib/g++-include/_G_config.h
/home/cyoung/warped/src/obj/LTSFInputQueue.o: BasicEvent.hh
/home/cyoung/warped/src/obj/LTSFInputQueue.o: /usr/include/stddef.h warped.hh
/home/cyoung/warped/src/obj/LTSFInputQueue.o: /local/packages/gnu/lib/g++-include/strstream.h
/home/cyoung/warped/src/obj/LTSFInputQueue.o: /local/packages/gnu/lib/g++-include/strfile.h
/home/cyoung/warped/src/obj/LTSFInputQueue.o: /local/packages/gnu/lib/g++-include/string.h
/home/cyoung/warped/src/obj/LTSFInputQueue.o: /local/packages/gnu/lib/g++-include/cstring
/home/cyoung/warped/src/obj/LTSFInputQueue.o: /local/packages/gnu/lib/g++-include/std/cstring.h
/home/cyoung/warped/src/obj/LTSFInputQueue.o: /usr/include/values.h
/home/cyoung/warped/src/obj/LTSFInputQueue.o: /usr/include/ctype.h
/home/cyoung/warped/src/obj/LTSFInputQueue.o: /usr/include/sys/feature_tests.h
/home/cyoung/warped/src/obj/LTSFInputQueue.o: DefaultVTime.hh MultiList.hh
/home/cyoung/warped/src/obj/LTSFInputQueue.o: SortedList.hh Container.hh
/home/cyoung/warped/src/obj/LTSFInputQueue.o: SortedList.cc MultiList.cc
/home/cyoung/warped/src/obj/StateQueue.o: StateQueue.hh warped.hh
/home/cyoung/warped/src/obj/StateQueue.o: /local/packages/gnu/lib/g++-include/iostream.h
/home/cyoung/warped/src/obj/StateQueue.o: /local/packages/gnu/lib/g++-include/streambuf.h
/home/cyoung/warped/src/obj/StateQueue.o: /local/packages/gnu/lib/g++-include/libio.h
/home/cyoung/warped/src/obj/StateQueue.o: /local/packages/gnu/lib/g++-include/_G_config.h
/home/cyoung/warped/src/obj/StateQueue.o: /local/packages/gnu/lib/g++-include/strstream.h
/home/cyoung/warped/src/obj/StateQueue.o: /local/packages/gnu/lib/g++-include/strfile.h
/home/cyoung/warped/src/obj/StateQueue.o: /local/packages/gnu/lib/g++-include/string.h
/home/cyoung/warped/src/obj/StateQueue.o: /local/packages/gnu/lib/g++-include/cstring
/home/cyoung/warped/src/obj/StateQueue.o: /local/packages/gnu/lib/g++-include/std/cstring.h
/home/cyoung/warped/src/obj/StateQueue.o: /usr/include/values.h
/home/cyoung/warped/src/obj/StateQueue.o: /usr/include/ctype.h
/home/cyoung/warped/src/obj/StateQueue.o: /usr/include/sys/feature_tests.h
/home/cyoung/warped/src/obj/StateQueue.o: config.hh DefaultVTime.hh
/home/cyoung/warped/src/obj/StateQueue.o: BasicState.hh SortedList.hh
/home/cyoung/warped/src/obj/StateQueue.o: Container.hh SortedList.cc
/home/cyoung/warped/src/obj/BasicState.o: BasicState.hh warped.hh
/home/cyoung/warped/src/obj/BasicState.o: /local/packages/gnu/lib/g++-include/iostream.h
/home/cyoung/warped/src/obj/BasicState.o: /local/packages/gnu/lib/g++-include/streambuf.h
/home/cyoung/warped/src/obj/BasicState.o: /local/packages/gnu/lib/g++-include/libio.h
/home/cyoung/warped/src/obj/BasicState.o: /local/packages/gnu/lib/g++-include/_G_config.h
/home/cyoung/warped/src/obj/BasicState.o: /local/packages/gnu/lib/g++-include/strstream.h
/home/cyoung/warped/src/obj/BasicState.o: /local/packages/gnu/lib/g++-include/strfile.h
/home/cyoung/warped/src/obj/BasicState.o: /local/packages/gnu/lib/g++-include/string.h
/home/cyoung/warped/src/obj/BasicState.o: /local/packages/gnu/lib/g++-include/cstring
/home/cyoung/warped/src/obj/BasicState.o: /local/packages/gnu/lib/g++-include/std/cstring.h
/home/cyoung/warped/src/obj/BasicState.o: /usr/include/values.h
/home/cyoung/warped/src/obj/BasicState.o: /usr/include/ctype.h
/home/cyoung/warped/src/obj/BasicState.o: /usr/include/sys/feature_tests.h
/home/cyoung/warped/src/obj/BasicState.o: config.hh DefaultVTime.hh
