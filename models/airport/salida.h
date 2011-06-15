/******************************************************************************
* Model: 	Airport
* Component:	Salida
* Author:	Luis De Simoni
*		Alejandro Troccoli (v2)
*
* Mail:	mailto://atroccol@dc.uba.ar
*
* DATE:	02/02/2001
********************************************************************************/

#ifndef __salida_H
#define __salida_H

#include <list>

#include "atomic.h"     	// class Atomic
#include "atomicstate.h"	// class AtomicState

class salidaState : public AtomicState {

public:

	typedef list<Value> ElementList ;
	ElementList elements ;

	salidaState(){};
	virtual ~salidaState(){};

	salidaState& operator=(salidaState& thisState); //Assignment
	void copyState(salidaState *);
	int  getSize() const;

};


class salida : public Atomic
{
public:
	salida( const string &name = "salida" );                                  //Default constructor

	virtual string className() const ;
protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const CollectMessage & );
	ModelState* allocateState() {
		return new salidaState;	
	}

private:
	const Port &in1;
	const Port &in2;
	const Port &in3;
	const Port &in4;
	Port &out;
	VTime preparationTime;

	salidaState::ElementList &elements();

};      // class salida

// ** inline ** // 
inline
	string salida::className() const
{
	return "salida" ;
}

/*******************************************************************
* Shortcuts to state paramters
*********************************************************************/
inline
salidaState::ElementList& salida::elements() {
	return ((salidaState*)getCurrentState())->elements;
}

#endif   //__salida_H
