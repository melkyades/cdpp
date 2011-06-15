/*******************************************************************
*
*  DESCRIPTION: class Transducer2
*               The transducer calculates statistical information 
*               analizing the processing time.  
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __TRANSDUCER2_H
#define __TRANSDUCER2_H

#include <map.h>
#include "atomic.h"
#include "message.h"

class Transducer2State : public AtomicState {

public:

	typedef map< int, VTime, less<int> > JobsList ;

	long procCount ;
	long cpuLoad ;
	JobsList unsolved; 

	Transducer2State(){};
	virtual ~Transducer2State(){};

	Transducer2State& operator=(Transducer2State& thisState); //Assignment
	void copyState(Transducer2State *);
	int  getSize() const;

};

class Transducer2: public Atomic
{
public:
	Transducer2( const string &name = "Transducer2" ) ;	//Default constructor

	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const CollectMessage & );
	ModelState* allocateState() 
	{return new Transducer2State;}

private:
	const Port &arrived ;
	const Port &solved ;
	Port &responsetime;
	Port &throughput ;
	Port &cpuUsage ;   

	VTime frec ;
	VTime tUnit ;

	const VTime &frecuence() const ;
	const VTime &timeUnit() const ;

	//Shortcut to state members
	long procCount() const;
	void procCount( long );

	long cpuLoad() const;
	void cpuLoad(long);

	Transducer2State::JobsList &unsolved();

};	// class Transducer2

// ** inline ** // 
inline
	string Transducer2::className() const
{
	return "Transducer2" ;
}

inline
	const VTime &Transducer2::frecuence() const
{
	return frec ;
}


inline
	const VTime &Transducer2::timeUnit() const
{
	return tUnit ;
}

/*******************************************************************
* Shortcuts to state paramters
*********************************************************************/
inline 
long Transducer2::procCount() const {
	return ((Transducer2State*)getCurrentState())->procCount;
}

inline
void Transducer2::procCount( long p) {
	((Transducer2State*)getCurrentState())->procCount = p;
}

inline 
long Transducer2::cpuLoad() const {
	return ((Transducer2State*)getCurrentState())->cpuLoad;
}

inline
void Transducer2::cpuLoad( long l) {
	((Transducer2State*)getCurrentState())->cpuLoad = l;
}


inline
Transducer2State::JobsList &Transducer2::unsolved() {
	return ((Transducer2State*)getCurrentState())->unsolved;
}

#endif   //__TRANSDUCER2_H 
