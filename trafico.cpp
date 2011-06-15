/*******************************************************************
*
*  DESCRIPTION: 
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/


/** include files **/
#include <time.h>
#include "trafico.h"    // class Trafico
#include "message.h"    // class ExternalMessage, InternalMessage

/*******************************************************************
* Function Name: Trafico
********************************************************************/
Trafico::Trafico( const string &name )
: Atomic( name )
, in( addInputPort( "in" ) )
, east( addOutputPort( "east" ) )
, west( addOutputPort( "west" ) )
, north( addOutputPort( "north" ) )
, south( addOutputPort( "south" ) )
, demoringTime( 0, 0, 1, 0 )
{}

/*******************************************************************
* Function Name: initFunction
* Precondition: El tiempo del proximo evento interno es Infinito
********************************************************************/ 
Model &Trafico::initFunction()
{
	time_t t;
	time(&t);
	#ifdef __unix__
		srandom( t );
	#else
		srand( t );
	#endif
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description: si estaba en modo activo (ya habia un vehiculo
*              transitando la manzana) ==> chocan y termina!!!
********************************************************************/
Model &Trafico::externalFunction( const ExternalMessage & )
{
	if( state() == AtomicState::passive )
		holdIn( AtomicState::active, demoringTime );
	else
		passivate();

	return *this;
}

/*******************************************************************
* Function Name: internalFunction
********************************************************************/
Model &Trafico::internalFunction( const InternalMessage & )
{
	passivate();
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description: se elige la direccion por la que saldra el vehiculo
********************************************************************/
Model &Trafico::outputFunction( const InternalMessage &msg )
{
	#ifdef __unix__
		switch( random () % 4 )
	#else
		switch( rand () % 4 )
	#endif
	{
		case 0:  sendOutput( msg.time(), north, 0 ) ;
			break;
		case 1:  sendOutput( msg.time(), east, 0 ) ;
			break;
		case 2:  sendOutput( msg.time(), south, 0 ) ;
			break;
		case 3:  sendOutput( msg.time(), west, 0 ) ;
			break;
	}
	return *this ;
}
