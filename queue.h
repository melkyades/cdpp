/*******************************************************************
*
*  DESCRIPCION: Cola Gen�rica (on demand)
*
*  AUTORES:
*  	    Ing. Carlos Giorgetti
*          Iv�n A. Melgrati
*          Dra. Ana Rosa Tymoschuk
*	    v2:Alejandro Troccoli
*
*  EMAIL: mailto://cgiorget@frsf.utn.edu.ar
*         mailto://imelgrat@frsf.utn.edu.ar
*         mailto://anrotym@alpha.arcride.edu.ar
*	   mailto://atroccol@dc.uba.ar (v2)
*
*  FECHA: 15/10/1999
*         01/02/2001
*******************************************************************/
#ifndef __QUEUE_H
#define __QUEUE_H

#include <list.h>
#include "atomic.h"     	// class Atomic
#include "atomicstate.h"	//
#include "VTime.hh"



class QueueState : public AtomicState {

public:

	typedef list<Value> ElementList ;
	ElementList elements ;

	QueueState(){};
	virtual ~QueueState(){};

	QueueState& operator=(QueueState& thisState); //Assignment
	void copyState(QueueState *);
	int  getSize() const;

};

class Queue : public Atomic
{
public:
	Queue( const string &name = "Queue" );
	virtual string className() const {  return "Queue" ;}
protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const CollectMessage & );

	ModelState* allocateState() {
		return new QueueState;
	}

private:
	const Port &in;
	const Port &done;
	Port &out;

	VTime preparationTime;

	QueueState::ElementList& elements();

};	// class Queue

/*******************************************************************
* Shortcuts to state paramters
*********************************************************************/
inline
QueueState::ElementList& Queue::elements() {
	return ((QueueState*)getCurrentState())->elements;
}

#endif   //__QUEUE_H
