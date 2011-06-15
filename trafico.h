/*******************************************************************
*
*  DESCRIPTION: class Trafico
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __TRAFICO_H
#define __TRAFICO_H

/** include files **/
#include <list>
#include "atomic.h"     // class Atomic

/** forward declarations **/

/** declarations **/
class Trafico : public Atomic
{
public:
	Trafico( const string &name = "Trafico" );	// Default constructor
	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
	const Port &in;
	const Port &east;
	const Port &west;
	const Port &north;
	const Port &south;   
	const VTime demoringTime;

};	// class Trafico

// ** inline ** // 
inline
string Trafico::className() const
{
	return "Trafico" ;
}

#endif   //__TRAFICO_H 
