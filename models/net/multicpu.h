/*******************************************************************
*
*  DESCRIPCION: MultiCPU (CPU Multitarea)
*
*  AUTORES:
*  	    Ing. Carlos Giorgetti
*          Iván A. Melgrati
*          Dra. Ana Rosa Tymoschuk
*	    Alejandro Troccoli (v2)
*
*  EMAIL: mailto://cgiorget@frsf.utn.edu.ar
*         mailto://imelgrat@frsf.utn.edu.ar
*         mailto://anrotym@alpha.arcride.edu.ar
*	   mailto://atroccol@dc.uba.ar (v2)
*
*  FECHA: 01/04/2000
*	   05/01/2001 (v2)
*
*******************************************************************/

#ifndef __MULTICPU_H
#define __MULTICPU_H
#include "atomic.h"
#include "mformats.h"
#include "VTime.hh"
#include <time.h>
#include "distri.h"
#include <list.h>




class MultiCPUState:public AtomicState {
public:	
	typedef list<Value> Cola ;
	Cola Proc_Inicial, Proc_Final;                      // Colas de Ejecución

	MultiCPUState(){};
	virtual ~MultiCPUState(){};

	MultiCPUState& operator=(MultiCPUState& thisState); //Assignment
	void copyState(MultiCPUState *);
	int  getSize() const;

};

class MultiCPU: public Atomic
{

public:
	MultiCPU( const string &name = "MultiCPU" ) ;	 // Constructor

	~MultiCPU(void){delete dist_inicial;
		delete dist_final;
	}		   						// Destructor

	virtual string className() const  {return "MultiCPU";}

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const CollectMessage & );
	ModelState* allocateState() {
		return new MultiCPUState;	
	}

private:
	const Port &cpuin;
	const Port &controladorain;
	Port &cpuout ;
	Port &controladoraout;

	Distribution *dist_inicial, *dist_final ;		// Distribución de probabilidad de tiempo de servicio
	MultiCPUState::Cola &Proc_Inicial();
	MultiCPUState::Cola &Proc_Final();

};	// class MultiCPU

inline
MultiCPUState::Cola& MultiCPU::Proc_Inicial() {
	return ((MultiCPUState*) getCurrentState())->Proc_Inicial;
}

inline
MultiCPUState::Cola& MultiCPU::Proc_Final() {
	return ((MultiCPUState*) getCurrentState())->Proc_Final;
}

#endif   //__MultiCPU_H
