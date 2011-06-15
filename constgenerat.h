/*******************************************************************
*
*  DESCRIPTION: class ConstantGenerator
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __CONSTGENERATOR_H
#define __CONSTGENERATOR_H

/** include files **/
#include "atomic.h"     // class Atomic
#include "except.h"     // class InvalidMessageException

/** declarations **/
class ConstGenerator : public Atomic
{
public:
	ConstGenerator( const string &name = "ConstGenerator" );				  // Default constructor

	virtual string className() const
		{return "ConstGenerator";}

protected:
	Model &initFunction() ;

	Model &externalFunction( const ExternalMessage & )
			{throw InvalidMessageException();}

	Model &internalFunction( const InternalMessage & );

	Model &outputFunction( const CollectMessage & );

private:
	Port &out ;
	VTime preparationTime;
			
};	// class ConstGenerator


#endif   //__CONSTGENERATOR_H 
