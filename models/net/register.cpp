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

#include "modeladm.h" 
#include "mainsimu.h"
#include "queue.h"      // class Queue
#include "generat.h"    // class Generator
#include "multicpu.h"        // class CPU
#include "transduc.h"   // class Transducer
#include "controla.h"   // class Controladora
#include "router.h"		// class Router
#include "client.h"     // class Client
#include "stopq.h"		// class StoppableQueue
#include "disco.h"      // class Disco
void MainSimulator::registerNewAtomics()
{
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Disco>() , "Disco" ) ;
    	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<StoppableQueue>() , "StoppableQueue" ) ;
    	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Client>() , "Client" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Router>() , "Router" ) ;
    	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Controladora>() , "Controladora" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Queue>() , "Queue" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Generator>() , "Generator" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<MultiCPU>() , "MultiCPU" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Transducer>() , "Transducer" ) ;

}
