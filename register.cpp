/*******************************************************************
*
*  DESCRIPTION: Simulator::registerNewAtomics()
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#include "pmodeladm.h" 
#include "parsimu.h"
#include "queue.h"      // class Queue
#include "generat.h"    // class Generator
#include "cpu.h"        // class CPU
#include "transduc.h"   // class Transducer
#include "constgenerat.h" //class ConstGenerator

#ifdef DEVS_AIRPORT
#include "modulo.h"
#include "selector.h"
#include "control.h"
#include "cola.h"
#include "deposito.h"
#include "salida.h"
#endif

#ifdef DEVS_NET
#include "client.h"
#include "controla.h"
#include "disco.h"
#include "multicpu.h"
#include "router.h"
#include "stopq.h"
#include "transduc2.h"
#endif

using namespace std;

void ParallelMainSimulator::registerNewAtomics()
{
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<Queue>() , "Queue" ) ;
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<Generator>() , "Generator" ) ;
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<ConstGenerator>() , "ConstGenerator" ) ;
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<CPU>() , "CPU" ) ;
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<Transducer>() , "Transducer" ) ;
	
#ifdef DEVS_AIRPORT
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<modulo>() , "modulo" ) ;
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<selector>() , "selector" ) ;
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<control>() , "control" ) ;
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<cola>() , "cola" ) ;
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<deposito>() , "deposito" ) ;
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<salida>() , "salida" ) ;
#endif
	
#ifdef DEVS_NET
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<Client>() , "Client" ) ;
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<Controladora>() , "Controladora" ) ;
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<Disco>() , "Disco" ) ;
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<MultiCPU>() , "MultiCPU" ) ;
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<Router>() , "Router" ) ;
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<StoppableQueue>() , "StoppableQueue" ) ;
	SingleParallelModelAdm::Instance().registerAtomic( NewAtomicFunction<Transducer2>() , "Transducer2" ) ;
#endif	
	
	
}
