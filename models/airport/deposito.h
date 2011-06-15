
#ifndef __deposito_H
#define __deposito_H

#include <list>

#include "atomic.h"     // class Atomic

class depositoState : public AtomicState {
public: 
	typedef list<Value> ElementList ;
	ElementList elements ;

	depositoState(){};
	virtual ~depositoState(){};

	depositoState& operator=(depositoState& thisState); //Assignment
	void copyState(depositoState *);
	int  getSize() const;

};

class deposito : public Atomic
{
public:
	deposito( const string &name = "deposito" );                                  //Default constructor

	virtual string className() const ;
protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const CollectMessage & );

	ModelState* allocateState() 
	{return new depositoState;}
private:
	const Port &in;
	Port &out;
	VTime preparationTime;

	depositoState::ElementList &elements();
};      // class deposito

// ** inline ** // 
inline
	string deposito::className() const
{
	return "deposito" ;
}

/*******************************************************************
* Shortcuts to state paramters
*********************************************************************/
inline
depositoState::ElementList &deposito::elements() {
	return ((depositoState*)getCurrentState())->elements;
}

#endif   //__deposito_H
