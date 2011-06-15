include Makefile.common

export CXXFLAGS=-Werror

all:
	$(MAKE) -C NoTime all
	$(MAKE) -C Sequential all
	$(MAKE) -C TimeWarp all


depend:
	$(MAKE) -C NoTime depend
	$(MAKE) -C Sequential depend
	$(MAKE) -C TimeWarp depend

clean:
	$(MAKE) -C NoTime clean
	$(MAKE) -C Sequential clean
	$(MAKE) -C TimeWarp clean

distclean:
	rm -f *~ config.log config.cache config.status\
	global_config.h $(MAKE)file.common
	$(MAKE) -C NoTime distclean
	$(MAKE) -C Sequential distclean
	$(MAKE) -C TimeWarp distclean

install:
	$(MAKE) -C NoTime install
	$(MAKE) -C Sequential install
	$(MAKE) -C TimeWarp install



