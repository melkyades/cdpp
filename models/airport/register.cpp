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
#include "cpu.h"        // class CPU
#include "transduc.h"   // class Transducer
#include "trafico.h"    // class Trafico
#include "modulo.h"     // class Pista Aterrizaje
#include "selector.h"   // class Selector hangar
#include "control.h"    // class Torre de control
#include "cola.h"       // class Cola de aviones en Maniobra de atterizaje despegue
#include "deposito.h"   //Deposito de aviones dentro del Hangar
#include "salida.h"     //Une los depositos con la salida de aviones del Hangar

void MainSimulator::registerNewAtomics()
{
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Queue>() , "Queue" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Generator>() , "Generator" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<CPU>() , "CPU" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Transducer>() , "Transducer" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Trafico>() , "Trafico" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<modulo>() , "modulo" ) ;
        SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<selector>() , "selector" ) ;
        SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<control>() , "control" ) ;
        SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<cola>() , "cola" ) ;
        SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<deposito>() , "deposito" ) ;
        SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<salida>() , "salida" ) ;
}
