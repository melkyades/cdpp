/******************************************************************************
* Model: 	Airport
* Component:	Selector
* Author:	Luis De Simoni
*		Alejandro Troccoli (v2)
*
* Mail:	mailto://atroccol@dc.uba.ar
*
* DATE:	02/02/2001
********************************************************************************/

#ifndef __selctor_H
#define __selector_H


#include "atomic.h"     // class Atomic

class selector : public Atomic
{
public:
	selector( const string &name = "selector" );                                  //Default constructor
	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const CollectMessage & );

	//This model does not require a special State class
	ModelState *allocateState(){
		return new AtomicState;	
	}

private:
	const Port &in; // Port de entrada de avion
	Port &out1;    // Port de Salida en cado de accidente
	Port &out2;    //  Port de Salida 
	Port &out3;    //  Port de Salida 
	Port &out4;    //  Port de Salida 

	float id; 
	VTime Demora;

};      // class selector

// ** inline ** // 
inline
	string selector::className() const
{
	return "selector" ;
}


#endif   //__selector_H
