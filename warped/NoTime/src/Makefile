#-*-makefile-*-
NTOBJ=obj

include ../../../Makefile.common


CPPFLAGS += $(INCLUDES_CPP) $(DEFINES_CPP)

SRCS = BasicEvent.cc BasicNoTime.cc BasicState.cc CommManager.cc \
	InputQueue.cc KernelMsgs.cc LogicalProcess.cc NoTime.cc \
	NoTimeObject.cc FileQueue.cc DebugStream.cc Interactive.cc

OBJS = $(NTOBJ)/BasicEvent.o $(NTOBJ)/BasicNoTime.o $(NTOBJ)/BasicState.o \
	$(NTOBJ)/CommManager.o $(NTOBJ)/InputQueue.o $(NTOBJ)/KernelMsgs.o \
	$(NTOBJ)/LogicalProcess.o $(NTOBJ)/NoTime.o $(NTOBJ)/NoTimeObject.o \
	$(NTOBJ)/FileQueue.o $(NTOBJ)/DebugStream.o $(NTOBJ)/Interactive.o \

all: obj $(NTOBJ)/libNoTime.a

obj:
	-mkdir $(NTOBJ)

../../common/src/VTime.o:
	make -C ../../common VTime.o

$(NTOBJ)/libNoTime.a: $(OBJS) ../../common/src/VTime.o
	-rm -f $(NTOBJ)/libNoTime.a
	$(AR) cr $(NTOBJ)/libNoTime.a $^ ../../common/src/VTime.o

$(NTOBJ)/%.o : %.cc
	$(CXX) -c \
	$(CPPFLAGS) $(CXXFLAGS) \
	$< -o $@

depend:
	makedepend -p$(NTOBJ)/ -- $(CPPFLAGS) $(CXXFLAGS) -- $(DEPENDINC) $(SRCS) 

cleandep:
	../bin/cleandepend ./Makefile > ./Makefile.clean
	mv ./Makefile.clean ./Makefile

clean:
	rm -f *~ \#*\#\
	rm -f $(NTOBJ)/*

# DO NOT DELETE THIS LINE -- make depend depends on it.

obj/BasicEvent.o: BasicEvent.hh NoTime.hh /usr/include/stdlib.h
obj/BasicEvent.o: /usr/include/features.h /usr/include/sys/cdefs.h
#obj/BasicEvent.o: /usr/include/gnu/stubs.h
#obj/BasicEvent.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
obj/BasicEvent.o: SimulationTime.hh DefaultVTime.hh
#obj/BasicEvent.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
#obj/BasicEvent.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
#obj/BasicEvent.o: /home/alejandro/tesis/cd++/dev/warped/common/include/VTime.hh
obj/BasicEvent.o: /usr/include/string.h
obj/BasicNoTime.o: BasicNoTime.hh ObjectRecord.hh NoTime.hh
obj/BasicNoTime.o: /usr/include/stdlib.h /usr/include/features.h
#obj/BasicNoTime.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
#obj/BasicNoTime.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
obj/BasicNoTime.o: DefaultVTime.hh
#obj/BasicNoTime.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
#obj/BasicNoTime.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
#obj/BasicNoTime.o: /home/alejandro/tesis/cd++/dev/warped/common/include/VTime.hh
obj/BasicState.o: BasicState.hh SimulationTime.hh DefaultVTime.hh NoTime.hh
obj/BasicState.o: /usr/include/stdlib.h /usr/include/features.h
#obj/BasicState.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
#obj/BasicState.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
#obj/BasicState.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
#obj/BasicState.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
#obj/BasicState.o: /home/alejandro/tesis/cd++/dev/warped/common/include/VTime.hh
obj/CommManager.o: CommManager.hh BasicNoTime.hh ObjectRecord.hh NoTime.hh
obj/CommManager.o: /usr/include/stdlib.h /usr/include/features.h
#obj/CommManager.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
#obj/CommManager.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
obj/CommManager.o: DefaultVTime.hh
#obj/CommManager.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
#obj/CommManager.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
#obj/CommManager.o: /home/alejandro/tesis/cd++/dev/warped/common/include/VTime.hh
obj/CommManager.o: KernelMsgs.hh BasicEvent.hh SimulationTime.hh
obj/CommManager.o: LogicalProcess.hh InputQueue.hh Interactive.hh
obj/CommManager.o: /usr/include/string.h DebugStream.hh NoTimeObject.hh
#obj/CommManager.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/new.h
#obj/CommManager.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/new
#obj/CommManager.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/exception
obj/CommManager.o: BasicState.hh
obj/InputQueue.o: InputQueue.hh SimulationTime.hh DefaultVTime.hh NoTime.hh
obj/InputQueue.o: /usr/include/stdlib.h /usr/include/features.h
#obj/InputQueue.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
#obj/InputQueue.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
#obj/InputQueue.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
#obj/InputQueue.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
#obj/InputQueue.o: /home/alejandro/tesis/cd++/dev/warped/common/include/VTime.hh
obj/InputQueue.o: BasicEvent.hh
obj/KernelMsgs.o: KernelMsgs.hh BasicEvent.hh NoTime.hh /usr/include/stdlib.h
obj/KernelMsgs.o: /usr/include/features.h /usr/include/sys/cdefs.h
#obj/KernelMsgs.o: /usr/include/gnu/stubs.h
#obj/KernelMsgs.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
obj/KernelMsgs.o: SimulationTime.hh DefaultVTime.hh
#obj/KernelMsgs.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
#obj/KernelMsgs.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
#obj/KernelMsgs.o: /home/alejandro/tesis/cd++/dev/warped/common/include/VTime.hh
obj/LogicalProcess.o: LogicalProcess.hh BasicEvent.hh NoTime.hh
obj/LogicalProcess.o: /usr/include/stdlib.h /usr/include/features.h
#obj/LogicalProcess.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
#obj/LogicalProcess.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
obj/LogicalProcess.o: SimulationTime.hh DefaultVTime.hh
#obj/LogicalProcess.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
#obj/LogicalProcess.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
#obj/LogicalProcess.o: /home/alejandro/tesis/cd++/dev/warped/common/include/VTime.hh
obj/LogicalProcess.o: CommManager.hh BasicNoTime.hh ObjectRecord.hh
obj/LogicalProcess.o: KernelMsgs.hh InputQueue.hh Interactive.hh
#obj/LogicalProcess.o: /usr/include/math.h /usr/include/bits/huge_val.h
#obj/LogicalProcess.o: /usr/include/bits/mathdef.h
#obj/LogicalProcess.o: /usr/include/bits/mathcalls.h /usr/include/string.h
obj/NoTime.o: NoTime.hh /usr/include/stdlib.h /usr/include/features.h
#obj/NoTime.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
#obj/NoTime.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
obj/NoTimeObject.o: LogicalProcess.hh BasicEvent.hh NoTime.hh
obj/NoTimeObject.o: /usr/include/stdlib.h /usr/include/features.h
#obj/NoTimeObject.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
#obj/NoTimeObject.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
obj/NoTimeObject.o: SimulationTime.hh DefaultVTime.hh
#obj/NoTimeObject.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
#obj/NoTimeObject.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
#obj/NoTimeObject.o: /home/alejandro/tesis/cd++/dev/warped/common/include/VTime.hh
obj/NoTimeObject.o: CommManager.hh BasicNoTime.hh ObjectRecord.hh
obj/NoTimeObject.o: KernelMsgs.hh InputQueue.hh Interactive.hh
obj/NoTimeObject.o: NoTimeObject.hh
#obj/NoTimeObject.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/new.h
#obj/NoTimeObject.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/new
#obj/NoTimeObject.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/exception
obj/NoTimeObject.o: BasicState.hh
obj/FileQueue.o: FileQueue.hh /usr/include/stdlib.h /usr/include/features.h
#obj/FileQueue.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
#obj/FileQueue.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
obj/FileQueue.o: FileData.hh DefaultVTime.hh NoTime.hh
#obj/FileQueue.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
#obj/FileQueue.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
#obj/FileQueue.o: /home/alejandro/tesis/cd++/dev/warped/common/include/VTime.hh
obj/DebugStream.o: LogicalProcess.hh BasicEvent.hh NoTime.hh
obj/DebugStream.o: /usr/include/stdlib.h /usr/include/features.h
#obj/DebugStream.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
#obj/DebugStream.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
obj/DebugStream.o: SimulationTime.hh DefaultVTime.hh
#obj/DebugStream.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
#obj/DebugStream.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
#obj/DebugStream.o: /home/alejandro/tesis/cd++/dev/warped/common/include/VTime.hh
obj/DebugStream.o: CommManager.hh BasicNoTime.hh ObjectRecord.hh
obj/DebugStream.o: KernelMsgs.hh InputQueue.hh Interactive.hh DebugStream.hh
obj/DebugStream.o: NoTimeObject.hh
#obj/DebugStream.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/new.h
#obj/DebugStream.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/new
#obj/DebugStream.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/exception
obj/DebugStream.o: BasicState.hh /usr/include/stdio.h
#obj/DebugStream.o: /usr/include/bits/types.h /usr/include/libio.h
#obj/DebugStream.o: /usr/include/_G_config.h /usr/include/wchar.h
#obj/DebugStream.o: /usr/include/bits/wchar.h /usr/include/gconv.h
#obj/DebugStream.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stdarg.h
#obj/DebugStream.o: /usr/include/bits/stdio_lim.h
#obj/DebugStream.o: /usr/include/bits/sys_errlist.h
obj/Interactive.o: DebugStream.hh NoTimeObject.hh
#obj/Interactive.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/new.h
#obj/Interactive.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/new
#obj/Interactive.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/stddef.h
#obj/Interactive.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/exception
obj/Interactive.o: BasicNoTime.hh ObjectRecord.hh NoTime.hh
obj/Interactive.o: /usr/include/stdlib.h /usr/include/features.h
#obj/Interactive.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
obj/Interactive.o: DefaultVTime.hh
#obj/Interactive.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/limits.h
#obj/Interactive.o: /home/alejandro/tesis/gcc/usr/lib/gcc-lib/i386-redhat-linux/2.96/include/syslimits.h
#obj/Interactive.o: /home/alejandro/tesis/cd++/dev/warped/common/include/VTime.hh
obj/Interactive.o: BasicState.hh SimulationTime.hh Interactive.hh
obj/Interactive.o: KernelMsgs.hh BasicEvent.hh

