/*******************************************************************
*
*  DESCRIPCION: Cola Genérica con Stop (detiene a la cola)
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
*  FECHA: 16/10/1999
*	   05/01/2001 (v2)
*
*******************************************************************/
#ifndef __STOPQ_H
#define __STOPQ_H

#include <list.h>
#include "atomic.h"


class StoppableQueueState : public AtomicState {

public:

	typedef list<Value> ElementList ;
	ElementList elements ;
	VTime timeLeft;
	int stopped;

	StoppableQueueState(){};
	virtual ~StoppableQueueState(){};

	StoppableQueueState& operator=(StoppableQueueState& thisState); //Assignment
	void copyState(StoppableQueueState *);
	int  getSize() const;

};

class StoppableQueue : public Atomic
{
public:
	StoppableQueue( const string &name = "StoppableQueue" );	//Default constructor
	virtual string className() const {  return "StoppableQueue" ;}
protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const CollectMessage & );
	ModelState* allocateState()
	{return new StoppableQueueState;}

private:
	const Port &stop;
	const Port &in;
	Port &out;
	VTime preparationTime;

	StoppableQueueState::ElementList &elements();
	VTime& timeLeft();
	void stopped( int );
	int stopped() const;

};	// class StoppableQueue

/*******************************************************************
* Shortcuts to state paramters
*********************************************************************/
inline
StoppableQueueState::ElementList& StoppableQueue::elements() {
	return ((StoppableQueueState*)getCurrentState())->elements;
}

inline
VTime& StoppableQueue::timeLeft() {
	return ((StoppableQueueState*)getCurrentState())->timeLeft;
}

inline
void StoppableQueue::stopped (int s) {
	((StoppableQueueState*)getCurrentState())->stopped = s;
}

inline
int StoppableQueue::stopped () const {
	return ((StoppableQueueState*)getCurrentState())->stopped;
}

#endif   //__QUEUE_H
