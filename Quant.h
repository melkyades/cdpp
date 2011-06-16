/*******************************************************************
*
*  DESCRIPTION: Simple quantizer for DEVS models
*
*  AUTHOR: Gabriel Wainer 
*
*  EMAIL: mailto://gabrielw@dc.uba.ar
*
*  DATE: 21/7/99
*
*******************************************************************/

#ifndef __Quant_H
#define __Quant_H

/** include files **/
#include "atomic.h"  // class Atomic

/** forward declarations **/
class Distribution ;

/** declarations **/
class Quant: public Atomic
{
public:
	Quant( const std::string &name = "Quant" ) ;      // Default constructor

	~Quant();                                 // Destructor

	virtual std::string className() const
	{return "Quant";}

protected:
	Model &initFunction()
	{return *this;}

	Model &externalFunction( const ExternalMessage & );

	Model &internalFunction( const InternalMessage & );

	Model &outputFunction( const CollectMessage & );

private:
	const Port &in ;
	Port &out ;
	double quant_value ;
	double quantum ;

};      // class Quant


#endif   //__Quant_H 
