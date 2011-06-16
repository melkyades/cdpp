/*******************************************************************
*
*  DESCRIPTION: class Generator
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*	v2: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*	   mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*        01/02/2001
*
*******************************************************************/

#ifndef __GENERATOR_H
#define __GENERATOR_H

/** include files **/
#include "atomic.h"     	// class Atomic
#include "atomicstate.h"	// class AtomicState
#include "except.h"     	// class InvalidMessageException

/** forward declarations **/
class Distribution ;

/** declarations **/
class GeneratorState : public AtomicState {

public:

	//Process id
	int pid;

	GeneratorState(){};
	virtual ~GeneratorState(){};

	GeneratorState& operator=(GeneratorState& thisState); //Assignment
	void copyState(GeneratorState *);
	int  getSize() const;

};


class Generator : public Atomic
{
public:
	Generator( const std::string &name = "Generator" );				  // Default constructor

	~Generator();

	virtual std::string className() const
	{return "Generator";}

protected:
	Model &initFunction() ;

	Model &externalFunction( const ExternalMessage & )
	{throw InvalidMessageException();}

	Model &internalFunction( const InternalMessage & );

	Model &outputFunction( const CollectMessage & );

	ModelState* allocateState() {
		return new GeneratorState;	
	}

private:

	int initial, increment;
	Port &out ;
	Distribution *dist ;

	Distribution &distribution()
	{return *dist;}

	//Shortcut to state members
	const int pid() const;
	void pid( int );

};	// class Generator


/*******************************************************************
* Shortcuts to state paramters
*********************************************************************/
inline 
const int Generator::pid() const {
	return ((GeneratorState*)getCurrentState())->pid;
}

inline
void Generator::pid( int p) {
	((GeneratorState*)getCurrentState())->pid = p;
}

#endif   //__GENERATOR_H 
