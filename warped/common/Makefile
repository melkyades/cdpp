include ../../Makefile.common

CPPFLAGS = $(DEFINES_CPP) $(INCLUDES_CPP)

ifndef DESTDIR
DESTDIR=/usr/local
endif

.cc.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -g  $< -o $@


includedir=$(DESTDIR)/include/warped

CPPFLAGS+=-Iinclude

DIRS=src
SRCS=$(foreach DIR, $(DIRS), $(wildcard $(DIR)/*.cc))
OBJS=$(patsubst %.cc,%.o, $(SRCS))

all: libcommon.a

VTime.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -g  ./src/VTime.cc  -o ./src/$@
	
libcommon.a: $(OBJS)
	rm -f $@
	$(AR) qvc $@ $^

depend: cleandep
	makedepend -f- -- $(CPPFLAGS) $(CXXFLAGS) -- $(DEPENDINC) $(SRCS) > .depend

cleandep:
	-rm -f .depend

install-headers:
	../$(MKINSTALLDIRS) $(includedir)
	for f in include/*.hh; do $(INSTALL) $$f $(includedir); done
	for f in include/*.cc; do $(INSTALL) $$f $(includedir); done

install-library:

clean:
	-rm -f src/*.o libcommon.a

-include .depend
