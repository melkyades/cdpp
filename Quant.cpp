/*******************************************************************
*
*  DESCRIPTION: Simple quantizer
*
*  AUTHOR: Gabriel Wainer 
*
*  EMAIL: mailto://gabrielw@dc.uba.ar
*
*  DATE: 21/7/99
*
*******************************************************************/

/** include files **/
#include <math.h>        // fabs( ... )
#include "Quant.h"           // base header
#include "message.h"       // InternalMessage ....
#include "parsimu.h"      // class ParallelMainSimulator
#include "strutil.h"       // str2float( ... )
#include "realfunc.h"      // value with quantum

/*******************************************************************
* Function Name: Quant
* Description: constructor
********************************************************************/
Quant::Quant( const string &name )
	: Atomic( name )
	, in( addInputPort( "in" ) )
	, out( addOutputPort( "out" ) )
{
	quantum = UseQuantum().Value();
}

/*******************************************************************
* Function Name: externalFunction
* Description: the Quant receives one job
********************************************************************/
Model &Quant::externalFunction( const ExternalMessage &msg )
{
	if (quantum  != 0)  {
		double ss;
		modf(msg.value()/quantum, &ss);
             // Dejo en ss la part entera de v/q
		quant_value = ss * quantum;
	}
	else
		quant_value = msg.value();

	holdIn( AtomicState::active, VTime( static_cast<float>( 0 ) ) ) ;
	return *this ;
}

/*******************************************************************
* Function Name: internalFunction
********************************************************************/
Model &Quant::internalFunction( const InternalMessage & )
{
	passivate();
	return *this;
}

/*******************************************************************
* Function Name: outputFunction
********************************************************************/
Model &Quant::outputFunction( const CollectMessage &msg )
{
	sendOutput( msg.time(), out, quant_value );
	return *this;
}

Quant::~Quant()
{
}
