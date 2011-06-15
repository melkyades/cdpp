/******************************************************************************
* Model: 	Airport
* Component:	Cola
* Author:	Luis De Simoni
*		Alejandro Troccoli (v2)
*
* Mail:	mailto://atroccol@dc.uba.ar
*
* DATE:	02/02/2001
********************************************************************************/
#ifndef __cola_H
#define __cola_H

#include <list>

#include "atomic.h"     	// class Atomic
#include "atomicstate.h"	// class AtomicState

class colaState : public AtomicState {
public: 
	typedef list<Value> ElementList ;
	ElementList elements ;
	int ocupado;

	colaState(){};
	virtual ~colaState(){};

	colaState& operator=(colaState& thisState); //Assignment
	void copyState(colaState *);
	int  getSize() const;

};

class cola : public Atomic
{
public:
	cola( const string &name = "cola" );                                  //Default constructor

	virtual string className() const ;
protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const CollectMessage & );


	ModelState* allocateState() {
		return new colaState;	
	}
private:
	const Port &in;
	const Port &stop;
	const Port &done;
	Port &out;
	VTime preparationTime;

	int ocupado() const;
	void ocupado( int );
	colaState::ElementList &elements();

};      // class cola

// ** inline ** // 
inline
	string cola::className() const
{
	return "cola" ;
}

/*******************************************************************
* Shortcuts to state paramters
*********************************************************************/
inline
int cola::ocupado() const {
	return ((colaState*)getCurrentState())->ocupado;	
}

inline 
void cola::ocupado(int o) {
	((colaState*)getCurrentState())->ocupado = o;
}

inline
colaState::ElementList &cola::elements() {
	return ((colaState*)getCurrentState())->elements;
}

#endif   //__cola_H
