.SUFFIXES : .cpp .o .c .y

include Makefile.common

#Basic models
export EXAMPLESOBJS=queue.o main.o generat.o cpu.o transduc.o constgenerat.o distri.o com.o linpack.o register.o 


SIMOBJS=neighval.o	\
tokit.o			\
macroexp.o		\
evaldeb.o		\
zone.o			\
message.o		\
partitionzone.o		\
cellpartition.o		\
except.o		\
strutil.o		\
randlib.o		\
ntupla.o		\
gram.o			\
undefd.o		\
atomicstate.o		\
atomic.o		\
coupled.o		\
msgbag.o		\
model.o			\
pmodeladm.o		\
real.o			\
realfunc.o		\
realprec.o		\
impresion.o		\
log.o			\
port.o			\
proot.o			\
pRootState.o		\
ini.o			\
stdaload.o		\
pmessage.o		\
pprocess.o		\
pProcessorState.o	\
pprocadm.o		\
psimulat.o		\
pSimulatorState.o	\
pcoordin.o		\
pmcoordin.o		\
pmcoordin2.o		\
pscoordin.o		\
pscoordin2.o		\
pCoordinatorState.o	\
pMCoordinatorState.o	\
portlist.o		\
atomcell.o		\
tdcell.o		\
idcell.o		\
ltranadm.o		\
coupcell.o		\
synnode.o		\
tbool.o			\
parser.o		\
parsimu.o		\
pini.o			\
pprocadm.o		\
statevars.o		\
netload.o		\
bsdchann.o
# if we are compiling for Windows 95, comment the last two lines

UTILOBJS = ntupla.o cellstate.o strutil.o ini.o $(COMMON)/src/VTime.o \
	real.o impresion.o tbool.o realprec.o except.o macroexp.o tokit.o
 
DRAWOBJS= drawlog.o logparser.o	 $(UTILOBJS)

LOGBUFFEROBJS= logbuffer.o logparser.o $(UTILOBJS)

PARLOGOBJS= parlog.o logparser.o pini.o $(UTILOBJS)


LIBNAME=simu
LIBS += -lsimu
ALLOBJS=${EXAMPLESOBJS} ${SIMOBJS} 
INIOBJS=initest.o ini.o strutil.o
PINIOBJS=pinitest.o pini.o strutil.o
ALLSRCS=${ALLOBJS:.o=.cpp} gram.y 

all: libs cd++  drawlog makerand toMap toCDPP logbuffer parlog

libs: libsimu.a

cd++: ${ALLOBJS} libsimu.a $(TWOBJ)/$(TWLIB) $(MODELLIBS)
	${CPP} ${LDFLAGS} -o $@  ${EXAMPLESOBJS} ${LIBS} ${PLIBS}

$(TWOBJ)/$(TWLIB):
	$(MAKE) -j6 -l5 -C $(TWDIR)

initest: ${INIOBJS}  
	${CPP} ${LDFLAGS} -o $@ ${INIOBJS}

pinitest: ${PINIOBJS}
	${CPP} ${LDFLAGS} -o $@ ${PINIOBJS}

drawlog: ${DRAWOBJS}
	g++ ${CPPFLAGS} ${LDFLAGS} -o $@ ${DRAWOBJS}

logbuffer: ${LOGBUFFEROBJS}
	g++ ${CPPLAGS} ${LDFLAGS} -o $@ ${LOGBUFFEROBJS}

parlog: ${PARLOGOBJS}
	g++ ${CPPFLAGS }${LDFLAGS} -o $@ ${PARLOGOBJS}
	
makerand: makerand.o $(UTILOBJS)
	g++ ${CPPFLAGS} ${LDFLAGS} -o $@ makerand.o ${UTILOBJS}

toMap: toMap.o $(UTILOBJS)
	g++ ${CPPFLAGS} ${LDFLAGS} -o $@ toMap.o ${UTILOBJS}

toCDPP: toCDPP.o $(UTILOBJS)
	g++ ${CPPFLAGS} ${LDFLAGS} -o $@ toCDPP.o ${UTILOBJS}

randEvent: randEvent.o $(UTILOBJS)
	g++ ${CPPFLAGS} ${LDFLAGS} -o $@ randEvent.o ${EXAMPLESOBJS} ${UTILOBJS}

exptest: synnode.o
	g++ ${LDFLAGS} -o $@ synnode.o
	
parser: parser.o gram.o
	g++ ${LDFLAGS} -o $@ parser.o gram.o

libsimu.a: ${SIMOBJS}
	${AR} crs lib${LIBNAME}.a ${SIMOBJS}

clean:
	- rm -f *.o *.a simu core drawlog initest exptest parser makerand toMap parlog logbuffer pcd cd++ toCDPP simu.tar.gz *~
	make -C ./warped/TimeWarp/src clean
	make -C ./warped/NoTime/src clean
	make -C ./warped/common clean
	make -C ./models/airport clean
	make -C ./models/net clean

depend:
	makedepend -Y ${ALLSRCS}
	make -C ./warped/TimeWarp/src depend
	make -C ./warped/NoTime/src depend
	make -C ./warped/common depend
	make -C ./models/airport depend
	make -C ./models/net depend

backup:
	tar -cvzf simu.tar.gz *.cpp *.h *.c *.y Makefile* makefile* *.ma *.ev *.vpj *.bat *.txt *.val *.inc *.map 


##############
MODELS:

./models/libnet.a:
	make -C ./models/net

./models/libairport.a:
	make -C ./models/airport
	
########################
# Without Optimization
generat.o: generat.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<

queue.o: queue.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<

cpu.o: cpu.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<

transduc.o: transduc.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<

trafico.o: trafico.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<

register.o: register.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<

msgadm.o: msgadm.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<

root.o: root.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<

parser.o: parser.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<

main.o: main.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<

drawlog.o: drawlog.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<

toMap.o: toMap.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<

toCDPP.o: toCDPP.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<

makerand.o: makerand.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<

mainsimu.o: mainsimu.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<

stdasimu.o: stdasimu.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<

parsimu.o: parsimu.cpp
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<


# Uncomment these lines only for Windows
#macroexp.o: macroexp.cpp
#	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<
#
#flatcoup.o: flatcoup.cpp
#	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} $<
########################

.cpp.o:
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} ${OPTCPPFLAGS} $<

.c.o:
	${CC} -c ${INCLUDES_C} ${DEFINES_C}  ${DEBUGFLAGS} ${CFLAGS} ${OPTCPPFLAGS} $<

.y.o:
	bison -d -v -o gram.c gram.y
	${CPP} -c ${INCLUDES_CPP} ${DEFINES_CPP} ${DEBUGFLAGS} ${CPPFLAGS} ${OPTCPPFLAGS} ${@:.o=.c} 
	rm $*.c
	
# DO NOT DELETE

queue.o: queue.h atomic.h model.h machineid.h port.h modelid.h pmodeladm.h
queue.o: strutil.h real.h undefd.h impresion.h tbool.h except.h stringp.h
queue.o: value.h evaldeb.h cellpos.h ntupla.h portlist.h pprocadm.h
queue.o: pprocess.h pProcessorState.h msgbag.h modelstate.h atomicstate.h
queue.o: pSimulatorState.h message.h parsimu.h proot.h event.h eventlist.h
queue.o: prootmodel.h pRootState.h ini.h tokit.h pini.h loader.h ltranadm.h
queue.o: log.h process.h
main.o: strutil.h real.h undefd.h impresion.h tbool.h except.h stringp.h
main.o: value.h evaldeb.h stdaload.h loader.h macroexp.h netload.h parsimu.h
main.o: proot.h event.h port.h modelid.h pmodeladm.h cellpos.h ntupla.h
main.o: eventlist.h prootmodel.h model.h machineid.h portlist.h pprocadm.h
main.o: pprocess.h pProcessorState.h msgbag.h modelstate.h pRootState.h ini.h
main.o: tokit.h pini.h ltranadm.h log.h
generat.o: generat.h atomic.h model.h machineid.h port.h modelid.h
generat.o: pmodeladm.h strutil.h real.h undefd.h impresion.h tbool.h except.h
generat.o: stringp.h value.h evaldeb.h cellpos.h ntupla.h portlist.h
generat.o: pprocadm.h pprocess.h pProcessorState.h msgbag.h modelstate.h
generat.o: atomicstate.h pSimulatorState.h message.h parsimu.h proot.h
generat.o: event.h eventlist.h prootmodel.h pRootState.h ini.h tokit.h pini.h
generat.o: loader.h ltranadm.h log.h process.h distri.h
cpu.o: cpu.h atomic.h model.h machineid.h port.h modelid.h pmodeladm.h
cpu.o: strutil.h real.h undefd.h impresion.h tbool.h except.h stringp.h
cpu.o: value.h evaldeb.h cellpos.h ntupla.h portlist.h pprocadm.h pprocess.h
cpu.o: pProcessorState.h msgbag.h modelstate.h atomicstate.h
cpu.o: pSimulatorState.h message.h parsimu.h proot.h event.h eventlist.h
cpu.o: prootmodel.h pRootState.h ini.h tokit.h pini.h loader.h ltranadm.h
cpu.o: log.h process.h distri.h
transduc.o: transduc.h atomic.h model.h machineid.h port.h modelid.h
transduc.o: pmodeladm.h strutil.h real.h undefd.h impresion.h tbool.h
transduc.o: except.h stringp.h value.h evaldeb.h cellpos.h ntupla.h
transduc.o: portlist.h pprocadm.h pprocess.h pProcessorState.h msgbag.h
transduc.o: modelstate.h atomicstate.h pSimulatorState.h message.h parsimu.h
transduc.o: proot.h event.h eventlist.h prootmodel.h pRootState.h ini.h
transduc.o: tokit.h pini.h loader.h ltranadm.h log.h process.h
constgenerat.o: constgenerat.h atomic.h model.h machineid.h port.h modelid.h
constgenerat.o: pmodeladm.h strutil.h real.h undefd.h impresion.h tbool.h
constgenerat.o: except.h stringp.h value.h evaldeb.h cellpos.h ntupla.h
constgenerat.o: portlist.h pprocadm.h pprocess.h pProcessorState.h msgbag.h
constgenerat.o: modelstate.h atomicstate.h pSimulatorState.h message.h
constgenerat.o: parsimu.h proot.h event.h eventlist.h prootmodel.h
constgenerat.o: pRootState.h ini.h tokit.h pini.h loader.h ltranadm.h log.h
constgenerat.o: process.h
distri.o: distri.h except.h stringp.h strutil.h real.h undefd.h impresion.h
distri.o: tbool.h value.h evaldeb.h randlib.h
register.o: pmodeladm.h strutil.h real.h undefd.h impresion.h tbool.h
register.o: except.h stringp.h value.h evaldeb.h modelid.h cellpos.h ntupla.h
register.o: parsimu.h proot.h event.h port.h eventlist.h prootmodel.h model.h
register.o: machineid.h portlist.h pprocadm.h pprocess.h pProcessorState.h
register.o: msgbag.h modelstate.h pRootState.h ini.h tokit.h pini.h loader.h
register.o: ltranadm.h log.h queue.h atomic.h atomicstate.h pSimulatorState.h
register.o: message.h process.h generat.h cpu.h transduc.h constgenerat.h
neighval.o: neighval.h except.h stringp.h real.h undefd.h impresion.h tbool.h
neighval.o: value.h evaldeb.h cellpos.h ntupla.h coupcell.h coupled.h model.h
neighval.o: machineid.h port.h modelid.h pmodeladm.h strutil.h portlist.h
neighval.o: pprocadm.h pprocess.h pProcessorState.h msgbag.h modelstate.h
neighval.o: ltranadm.h pCellCoordinatorState.h pCoordinatorState.h
neighval.o: cellpartition.h partitionzone.h zone.h parsimu.h proot.h event.h
neighval.o: eventlist.h prootmodel.h pRootState.h ini.h tokit.h pini.h
neighval.o: loader.h log.h statevars.h cellstate.h atomcell.h atomic.h
neighval.o: atomicstate.h pSimulatorState.h message.h process.h
tokit.o: tokit.h
macroexp.o: macroexp.h except.h stringp.h strutil.h real.h undefd.h
macroexp.o: impresion.h tbool.h value.h evaldeb.h
evaldeb.o: evaldeb.h
zone.o: zone.h cellpos.h ntupla.h except.h stringp.h
message.o: message.h model.h machineid.h port.h modelid.h pmodeladm.h
message.o: strutil.h real.h undefd.h impresion.h tbool.h except.h stringp.h
message.o: value.h evaldeb.h cellpos.h ntupla.h portlist.h pprocadm.h
message.o: pprocess.h pProcessorState.h msgbag.h modelstate.h parsimu.h
message.o: proot.h event.h eventlist.h prootmodel.h pRootState.h ini.h
message.o: tokit.h pini.h loader.h ltranadm.h log.h process.h
partitionzone.o: partitionzone.h zone.h cellpos.h ntupla.h except.h stringp.h
cellpartition.o: cellpartition.h partitionzone.h zone.h cellpos.h ntupla.h
cellpartition.o: except.h stringp.h
except.o: except.h stringp.h macroexp.h
strutil.o: strutil.h real.h undefd.h impresion.h tbool.h except.h stringp.h
strutil.o: value.h evaldeb.h
ntupla.o: ntupla.h except.h stringp.h strutil.h real.h undefd.h impresion.h
ntupla.o: tbool.h value.h evaldeb.h
undefd.o: undefd.h
atomicstate.o: atomicstate.h modelstate.h
atomic.o: atomic.h model.h machineid.h port.h modelid.h pmodeladm.h strutil.h
atomic.o: real.h undefd.h impresion.h tbool.h except.h stringp.h value.h
atomic.o: evaldeb.h cellpos.h ntupla.h portlist.h pprocadm.h pprocess.h
atomic.o: pProcessorState.h msgbag.h modelstate.h atomicstate.h
atomic.o: pSimulatorState.h message.h parsimu.h proot.h event.h eventlist.h
atomic.o: prootmodel.h pRootState.h ini.h tokit.h pini.h loader.h ltranadm.h
atomic.o: log.h process.h
coupled.o: coupled.h model.h machineid.h port.h modelid.h pmodeladm.h
coupled.o: strutil.h real.h undefd.h impresion.h tbool.h except.h stringp.h
coupled.o: value.h evaldeb.h cellpos.h ntupla.h portlist.h pprocadm.h
coupled.o: pprocess.h pProcessorState.h msgbag.h modelstate.h
msgbag.o: msgbag.h message.h model.h machineid.h port.h modelid.h pmodeladm.h
msgbag.o: strutil.h real.h undefd.h impresion.h tbool.h except.h stringp.h
msgbag.o: value.h evaldeb.h cellpos.h ntupla.h portlist.h pprocadm.h
msgbag.o: pprocess.h pProcessorState.h modelstate.h parsimu.h proot.h event.h
msgbag.o: eventlist.h prootmodel.h pRootState.h ini.h tokit.h pini.h loader.h
msgbag.o: ltranadm.h log.h process.h
model.o: parsimu.h proot.h event.h real.h undefd.h impresion.h tbool.h
model.o: except.h stringp.h value.h evaldeb.h port.h modelid.h pmodeladm.h
model.o: strutil.h cellpos.h ntupla.h eventlist.h prootmodel.h model.h
model.o: machineid.h portlist.h pprocadm.h pprocess.h pProcessorState.h
model.o: msgbag.h modelstate.h pRootState.h ini.h tokit.h pini.h loader.h
model.o: ltranadm.h log.h
pmodeladm.o: pmodeladm.h strutil.h real.h undefd.h impresion.h tbool.h
pmodeladm.o: except.h stringp.h value.h evaldeb.h modelid.h cellpos.h
pmodeladm.o: ntupla.h parsimu.h proot.h event.h port.h eventlist.h
pmodeladm.o: prootmodel.h model.h machineid.h portlist.h pprocadm.h
pmodeladm.o: pprocess.h pProcessorState.h msgbag.h modelstate.h pRootState.h
pmodeladm.o: ini.h tokit.h pini.h loader.h ltranadm.h log.h idcell.h
pmodeladm.o: atomcell.h atomic.h atomicstate.h pSimulatorState.h message.h
pmodeladm.o: process.h neighval.h coupcell.h coupled.h
pmodeladm.o: pCellCoordinatorState.h pCoordinatorState.h cellpartition.h
pmodeladm.o: partitionzone.h zone.h statevars.h tdcell.h flatcoup.h
real.o: real.h undefd.h impresion.h tbool.h except.h stringp.h value.h
real.o: evaldeb.h realprec.h mathincl.h
realfunc.o: realfunc.h real.h undefd.h impresion.h tbool.h except.h stringp.h
realfunc.o: value.h evaldeb.h mathincl.h randlib.h
realprec.o: realprec.h value.h
impresion.o: impresion.h
log.o: log.h
port.o: port.h modelid.h pmodeladm.h strutil.h real.h undefd.h impresion.h
port.o: tbool.h except.h stringp.h value.h evaldeb.h cellpos.h ntupla.h
proot.o: proot.h event.h real.h undefd.h impresion.h tbool.h except.h
proot.o: stringp.h value.h evaldeb.h port.h modelid.h pmodeladm.h strutil.h
proot.o: cellpos.h ntupla.h eventlist.h prootmodel.h model.h machineid.h
proot.o: portlist.h pprocadm.h pprocess.h pProcessorState.h msgbag.h
proot.o: modelstate.h pRootState.h message.h parsimu.h ini.h tokit.h pini.h
proot.o: loader.h ltranadm.h log.h process.h coupled.h
pRootState.o: pRootState.h pProcessorState.h eventlist.h event.h real.h
pRootState.o: undefd.h impresion.h tbool.h except.h stringp.h value.h
pRootState.o: evaldeb.h port.h modelid.h pmodeladm.h strutil.h cellpos.h
pRootState.o: ntupla.h
ini.o: ini.h except.h stringp.h tokit.h strutil.h real.h undefd.h impresion.h
ini.o: tbool.h value.h evaldeb.h prnutil.h
stdaload.o: stdaload.h loader.h evaldeb.h macroexp.h except.h stringp.h
stdaload.o: realprec.h value.h impresion.h log.h randlib.h
pmessage.o: pmessage.h message.h model.h machineid.h port.h modelid.h
pmessage.o: pmodeladm.h strutil.h real.h undefd.h impresion.h tbool.h
pmessage.o: except.h stringp.h value.h evaldeb.h cellpos.h ntupla.h
pmessage.o: portlist.h pprocadm.h pprocess.h pProcessorState.h msgbag.h
pmessage.o: modelstate.h parsimu.h proot.h event.h eventlist.h prootmodel.h
pmessage.o: pRootState.h ini.h tokit.h pini.h loader.h ltranadm.h log.h
pmessage.o: process.h
pprocess.o: strutil.h real.h undefd.h impresion.h tbool.h except.h stringp.h
pprocess.o: value.h evaldeb.h pprocess.h pProcessorState.h modelid.h msgbag.h
pprocess.o: pmessage.h message.h model.h machineid.h port.h pmodeladm.h
pprocess.o: cellpos.h ntupla.h portlist.h pprocadm.h modelstate.h parsimu.h
pprocess.o: proot.h event.h eventlist.h prootmodel.h pRootState.h ini.h
pprocess.o: tokit.h pini.h loader.h ltranadm.h log.h process.h
pProcessorState.o: pProcessorState.h
pprocadm.o: pprocadm.h pprocess.h pProcessorState.h modelid.h except.h
pprocadm.o: stringp.h value.h msgbag.h model.h machineid.h port.h pmodeladm.h
pprocadm.o: strutil.h real.h undefd.h impresion.h tbool.h evaldeb.h cellpos.h
pprocadm.o: ntupla.h portlist.h modelstate.h atomic.h atomicstate.h
pprocadm.o: pSimulatorState.h message.h parsimu.h proot.h event.h eventlist.h
pprocadm.o: prootmodel.h pRootState.h ini.h tokit.h pini.h loader.h
pprocadm.o: ltranadm.h log.h process.h flatcoup.h coupcell.h coupled.h
pprocadm.o: pCellCoordinatorState.h pCoordinatorState.h cellpartition.h
pprocadm.o: partitionzone.h zone.h statevars.h psimulat.h pcoordin.h
pprocadm.o: pmcoordin.h pMCoordinatorState.h pscoordin.h
psimulat.o: psimulat.h pprocess.h pProcessorState.h modelid.h except.h
psimulat.o: stringp.h value.h msgbag.h pSimulatorState.h atomicstate.h
psimulat.o: modelstate.h atomic.h model.h machineid.h port.h pmodeladm.h
psimulat.o: strutil.h real.h undefd.h impresion.h tbool.h evaldeb.h cellpos.h
psimulat.o: ntupla.h portlist.h pprocadm.h message.h parsimu.h proot.h
psimulat.o: event.h eventlist.h prootmodel.h pRootState.h ini.h tokit.h
psimulat.o: pini.h loader.h ltranadm.h log.h process.h pmessage.h
pSimulatorState.o: pSimulatorState.h pProcessorState.h atomicstate.h
pSimulatorState.o: modelstate.h
pcoordin.o: pcoordin.h pprocess.h pProcessorState.h modelid.h except.h
pcoordin.o: stringp.h value.h msgbag.h pCoordinatorState.h pprocadm.h
pcoordin.o: coupled.h model.h machineid.h port.h pmodeladm.h strutil.h real.h
pcoordin.o: undefd.h impresion.h tbool.h evaldeb.h cellpos.h ntupla.h
pcoordin.o: portlist.h modelstate.h pmessage.h message.h parsimu.h proot.h
pcoordin.o: event.h eventlist.h prootmodel.h pRootState.h ini.h tokit.h
pcoordin.o: pini.h loader.h ltranadm.h log.h process.h
pmcoordin.o: pmcoordin.h pcoordin.h pprocess.h pProcessorState.h modelid.h
pmcoordin.o: except.h stringp.h value.h msgbag.h pCoordinatorState.h
pmcoordin.o: pprocadm.h pMCoordinatorState.h coupled.h model.h machineid.h
pmcoordin.o: port.h pmodeladm.h strutil.h real.h undefd.h impresion.h tbool.h
pmcoordin.o: evaldeb.h cellpos.h ntupla.h portlist.h modelstate.h pmessage.h
pmcoordin.o: message.h parsimu.h proot.h event.h eventlist.h prootmodel.h
pmcoordin.o: pRootState.h ini.h tokit.h pini.h loader.h ltranadm.h log.h
pmcoordin.o: process.h
pmcoordin2.o: pmcoordin2.h pcoordin.h pprocess.h pProcessorState.h modelid.h
pmcoordin2.o: except.h stringp.h value.h msgbag.h pCoordinatorState.h
pmcoordin2.o: pprocadm.h pMCoordinatorState.h coupled.h model.h machineid.h
pmcoordin2.o: port.h pmodeladm.h strutil.h real.h undefd.h impresion.h
pmcoordin2.o: tbool.h evaldeb.h cellpos.h ntupla.h portlist.h modelstate.h
pmcoordin2.o: pmessage.h message.h parsimu.h proot.h event.h eventlist.h
pmcoordin2.o: prootmodel.h pRootState.h ini.h tokit.h pini.h loader.h
pmcoordin2.o: ltranadm.h log.h process.h
pscoordin.o: pscoordin.h pcoordin.h pprocess.h pProcessorState.h modelid.h
pscoordin.o: except.h stringp.h value.h msgbag.h pCoordinatorState.h
pscoordin.o: pprocadm.h coupled.h model.h machineid.h port.h pmodeladm.h
pscoordin.o: strutil.h real.h undefd.h impresion.h tbool.h evaldeb.h
pscoordin.o: cellpos.h ntupla.h portlist.h modelstate.h pmessage.h message.h
pscoordin.o: parsimu.h proot.h event.h eventlist.h prootmodel.h pRootState.h
pscoordin.o: ini.h tokit.h pini.h loader.h ltranadm.h log.h process.h
pscoordin2.o: pscoordin2.h pcoordin.h pprocess.h pProcessorState.h modelid.h
pscoordin2.o: except.h stringp.h value.h msgbag.h pCoordinatorState.h
pscoordin2.o: pprocadm.h coupled.h model.h machineid.h port.h pmodeladm.h
pscoordin2.o: strutil.h real.h undefd.h impresion.h tbool.h evaldeb.h
pscoordin2.o: cellpos.h ntupla.h portlist.h modelstate.h pmessage.h message.h
pscoordin2.o: parsimu.h proot.h event.h eventlist.h prootmodel.h pRootState.h
pscoordin2.o: ini.h tokit.h pini.h loader.h ltranadm.h log.h process.h
pCoordinatorState.o: pCoordinatorState.h pProcessorState.h pprocadm.h
pCoordinatorState.o: pprocess.h modelid.h except.h stringp.h value.h msgbag.h
pMCoordinatorState.o: pMCoordinatorState.h pCoordinatorState.h
pMCoordinatorState.o: pProcessorState.h pprocadm.h pprocess.h modelid.h
pMCoordinatorState.o: except.h stringp.h value.h msgbag.h
portlist.o: portlist.h real.h undefd.h impresion.h tbool.h except.h stringp.h
portlist.o: value.h evaldeb.h port.h modelid.h pmodeladm.h strutil.h
portlist.o: cellpos.h ntupla.h
atomcell.o: atomcell.h portlist.h real.h undefd.h impresion.h tbool.h
atomcell.o: except.h stringp.h value.h evaldeb.h port.h modelid.h pmodeladm.h
atomcell.o: strutil.h cellpos.h ntupla.h atomic.h model.h machineid.h
atomcell.o: pprocadm.h pprocess.h pProcessorState.h msgbag.h modelstate.h
atomcell.o: atomicstate.h pSimulatorState.h message.h parsimu.h proot.h
atomcell.o: event.h eventlist.h prootmodel.h pRootState.h ini.h tokit.h
atomcell.o: pini.h loader.h ltranadm.h log.h process.h neighval.h coupcell.h
atomcell.o: coupled.h pCellCoordinatorState.h pCoordinatorState.h
atomcell.o: cellpartition.h partitionzone.h zone.h statevars.h
tdcell.o: tdcell.h atomcell.h portlist.h real.h undefd.h impresion.h tbool.h
tdcell.o: except.h stringp.h value.h evaldeb.h port.h modelid.h pmodeladm.h
tdcell.o: strutil.h cellpos.h ntupla.h atomic.h model.h machineid.h
tdcell.o: pprocadm.h pprocess.h pProcessorState.h msgbag.h modelstate.h
tdcell.o: atomicstate.h pSimulatorState.h message.h parsimu.h proot.h event.h
tdcell.o: eventlist.h prootmodel.h pRootState.h ini.h tokit.h pini.h loader.h
tdcell.o: ltranadm.h log.h process.h neighval.h coupcell.h coupled.h
tdcell.o: pCellCoordinatorState.h pCoordinatorState.h cellpartition.h
tdcell.o: partitionzone.h zone.h statevars.h realfunc.h
idcell.o: idcell.h atomcell.h portlist.h real.h undefd.h impresion.h tbool.h
idcell.o: except.h stringp.h value.h evaldeb.h port.h modelid.h pmodeladm.h
idcell.o: strutil.h cellpos.h ntupla.h atomic.h model.h machineid.h
idcell.o: pprocadm.h pprocess.h pProcessorState.h msgbag.h modelstate.h
idcell.o: atomicstate.h pSimulatorState.h message.h parsimu.h proot.h event.h
idcell.o: eventlist.h prootmodel.h pRootState.h ini.h tokit.h pini.h loader.h
idcell.o: ltranadm.h log.h process.h neighval.h coupcell.h coupled.h
idcell.o: pCellCoordinatorState.h pCoordinatorState.h cellpartition.h
idcell.o: partitionzone.h zone.h statevars.h realfunc.h
ltranadm.o: ltranadm.h cellpos.h ntupla.h except.h stringp.h portlist.h
ltranadm.o: real.h undefd.h impresion.h tbool.h value.h evaldeb.h port.h
ltranadm.o: modelid.h pmodeladm.h strutil.h model.h machineid.h pprocadm.h
ltranadm.o: pprocess.h pProcessorState.h msgbag.h modelstate.h parser.h
ltranadm.o: tokit.h neighval.h coupcell.h coupled.h pCellCoordinatorState.h
ltranadm.o: pCoordinatorState.h cellpartition.h partitionzone.h zone.h
ltranadm.o: parsimu.h proot.h event.h eventlist.h prootmodel.h pRootState.h
ltranadm.o: ini.h pini.h loader.h log.h statevars.h synnode.h realfunc.h
ltranadm.o: atomcell.h atomic.h atomicstate.h pSimulatorState.h message.h
ltranadm.o: process.h
coupcell.o: coupcell.h coupled.h model.h machineid.h port.h modelid.h
coupcell.o: pmodeladm.h strutil.h real.h undefd.h impresion.h tbool.h
coupcell.o: except.h stringp.h value.h evaldeb.h cellpos.h ntupla.h
coupcell.o: portlist.h pprocadm.h pprocess.h pProcessorState.h msgbag.h
coupcell.o: modelstate.h ltranadm.h pCellCoordinatorState.h
coupcell.o: pCoordinatorState.h cellpartition.h partitionzone.h zone.h
coupcell.o: parsimu.h proot.h event.h eventlist.h prootmodel.h pRootState.h
coupcell.o: ini.h tokit.h pini.h loader.h log.h statevars.h atomcell.h
coupcell.o: atomic.h atomicstate.h pSimulatorState.h message.h process.h
coupcell.o: neighval.h idcell.h tdcell.h
synnode.o: synnode.h tbool.h except.h stringp.h real.h undefd.h impresion.h
synnode.o: value.h evaldeb.h realfunc.h ltranadm.h cellpos.h ntupla.h
synnode.o: portlist.h port.h modelid.h pmodeladm.h strutil.h model.h
synnode.o: machineid.h pprocadm.h pprocess.h pProcessorState.h msgbag.h
synnode.o: modelstate.h neighval.h coupcell.h coupled.h
synnode.o: pCellCoordinatorState.h pCoordinatorState.h cellpartition.h
synnode.o: partitionzone.h zone.h parsimu.h proot.h event.h eventlist.h
synnode.o: prootmodel.h pRootState.h ini.h tokit.h pini.h loader.h log.h
synnode.o: statevars.h atomcell.h atomic.h atomicstate.h pSimulatorState.h
synnode.o: message.h process.h
tbool.o: tbool.h except.h stringp.h real.h undefd.h impresion.h value.h
tbool.o: evaldeb.h
parser.o: parser.h except.h stringp.h tokit.h synnode.h tbool.h real.h
parser.o: undefd.h impresion.h value.h evaldeb.h realfunc.h ltranadm.h
parser.o: cellpos.h ntupla.h portlist.h port.h modelid.h pmodeladm.h
parser.o: strutil.h model.h machineid.h pprocadm.h pprocess.h
parser.o: pProcessorState.h msgbag.h modelstate.h gram.h mathincl.h
parsimu.o: parsimu.h proot.h event.h real.h undefd.h impresion.h tbool.h
parsimu.o: except.h stringp.h value.h evaldeb.h port.h modelid.h pmodeladm.h
parsimu.o: strutil.h cellpos.h ntupla.h eventlist.h prootmodel.h model.h
parsimu.o: machineid.h portlist.h pprocadm.h pprocess.h pProcessorState.h
parsimu.o: msgbag.h modelstate.h pRootState.h ini.h tokit.h pini.h loader.h
parsimu.o: ltranadm.h log.h stdaload.h macroexp.h coupled.h zone.h
parsimu.o: cellpartition.h partitionzone.h flatcoup.h coupcell.h
parsimu.o: pCellCoordinatorState.h pCoordinatorState.h statevars.h tdcell.h
parsimu.o: atomcell.h atomic.h atomicstate.h pSimulatorState.h message.h
parsimu.o: process.h neighval.h idcell.h
pini.o: pini.h machineid.h except.h stringp.h tokit.h strutil.h real.h
pini.o: undefd.h impresion.h tbool.h value.h evaldeb.h prnutil.h
pprocadm.o: pprocadm.h pprocess.h pProcessorState.h modelid.h except.h
pprocadm.o: stringp.h value.h msgbag.h model.h machineid.h port.h pmodeladm.h
pprocadm.o: strutil.h real.h undefd.h impresion.h tbool.h evaldeb.h cellpos.h
pprocadm.o: ntupla.h portlist.h modelstate.h atomic.h atomicstate.h
pprocadm.o: pSimulatorState.h message.h parsimu.h proot.h event.h eventlist.h
pprocadm.o: prootmodel.h pRootState.h ini.h tokit.h pini.h loader.h
pprocadm.o: ltranadm.h log.h process.h flatcoup.h coupcell.h coupled.h
pprocadm.o: pCellCoordinatorState.h pCoordinatorState.h cellpartition.h
pprocadm.o: partitionzone.h zone.h statevars.h psimulat.h pcoordin.h
pprocadm.o: pmcoordin.h pMCoordinatorState.h pscoordin.h
statevars.o: real.h undefd.h impresion.h tbool.h except.h stringp.h value.h
statevars.o: evaldeb.h statevars.h
netload.o: netload.h loader.h evaldeb.h bsdchann.h commchan.h except.h
netload.o: stringp.h
bsdchann.o: bsdchann.h commchan.h except.h stringp.h
gram.o: synnode.h tbool.h except.h stringp.h real.h undefd.h impresion.h
gram.o: value.h evaldeb.h realfunc.h ltranadm.h cellpos.h ntupla.h portlist.h
gram.o: port.h modelid.h pmodeladm.h strutil.h model.h machineid.h pprocadm.h
gram.o: pprocess.h pProcessorState.h msgbag.h modelstate.h parser.h tokit.h
