/*******************************************************************
*
*  DESCRIPCION: Router (rutea los paquetes de acuerdo a su dirección)
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
*  FECHA: 18/10/1999
*	   05/01/2001 (v2)
*
*******************************************************************/#ifndef __ROUTER_H
#define __ROUTER_H
#include <list.h>
#include "atomic.h"

class RouterState : public AtomicState {

public:

	typedef list<Value> ElementList ;
	ElementList elements ;

	RouterState(){};
	virtual ~RouterState(){};

	RouterState& operator=(RouterState& thisState); //Assignment
	void copyState(RouterState *);
	int  getSize() const;

};


class Router : public Atomic
{
public:
	Router( const string &name = "Router" );					//Default constructor
	virtual string className() const {return "Queue" ;}

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const CollectMessage & );
	ModelState* allocateState() 
	{ return new RouterState;	}

private:
	const Port &in;
	Port &out0;
	Port &out1;
	Port &out2;
	Port &out3;
	Port &out4;
	Port &out5;
	Port &out6;
	Port &out7;
	Port &out8;
	Port &out9;
	Port &out10;
	Port &out11;
	Port &out12;
	Port &out13;
	Port &out14;
	Port &out15;

	VTime preparationTime;
	RouterState::ElementList &elements() ;

};	// class 

/*******************************************************************
* Shortcuts to state paramters
*********************************************************************/
inline
RouterState::ElementList& Router::elements() {
	return ((RouterState*)getCurrentState())->elements;
}

#endif   //__ROUTER_H
