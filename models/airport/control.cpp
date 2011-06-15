/******************************************************************************
* Model: 	Airport
* Component:	Control
* Author:	Luis De Simoni
*		Alejandro Troccoli (v2)
*
* Mail:	mailto://atroccol@dc.uba.ar
*
* DATE:	02/02/2001
********************************************************************************/

/** include files **/
#include <string>

/** my include files **/
#include "control.h"      // class control
#include "message.h"    // class ExternalMessage, InternalMessage
#include "parsimu.h"   // MainSimulator::Instance().getParameter( ... )

/** public functions **/

/*******************************************************************
* Function Name: control
* Description: 
********************************************************************/
control::control( const string &name )
	: Atomic( name )
	, in_a( this->addInputPort( "in_a" ) )
	, in_d( this->addInputPort( "in_d" ) )
	, done_a( this->addOutputPort( "done_a" ) )
	, done_d( this->addOutputPort( "done_d" ) )
	, aterriza( this->addOutputPort( "aterriza" ) )
	, despega( this->addOutputPort( "despega" ) )
	, stop_d( this->addOutputPort( "stop_d" ) )
	, stop_a( this->addOutputPort( "stop_a" ) )

{
}



/*******************************************************************
* Function Name: initFunction

* Precondition: El tiempo del proximo evento interno es ...
********************************************************************/
Model &control::initFunction()
{
	tiempo_aterrizaje = "00:01:00:00";
	avion_numero=0 ;
	return *this ;
}


/*******************************************************************
* Function Name: externalFunction
* Description:
********************************************************************/
Model &control::externalFunction( const ExternalMessage &msg )
{
	if( msg.port() == in_a )  //Ingresa un avion para aterrizar
	{

		avion_numero = msg.value()  ;
		this->sendOutput( msg.time(), stop_a , 1) ;
		this->sendOutput( msg.time(), stop_d , 1) ;
		this->sendOutput( msg.time(), aterriza , avion_numero) ;
		this->holdIn( AtomicState::active, tiempo_aterrizaje );
	}


	if( msg.port() == in_d )   //Ingresa un avion para despegar
	{

		avion_numero = msg.value()  ;
		this->sendOutput( msg.time(), stop_a , 1) ;
		this->sendOutput( msg.time(), stop_d , 1) ;
		this->sendOutput( msg.time(), despega , avion_numero) ;
		this->holdIn( AtomicState::active, tiempo_aterrizaje );
	}



	return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description:
********************************************************************/
Model &control::internalFunction( const InternalMessage & )
{
	this->passivate();
	return *this ;
}


/*******************************************************************
* Function Name: outputFunction
* Description:
********************************************************************/
Model &control::outputFunction( const CollectMessage &msg )

{
	this->sendOutput( msg.time(), done_a , 1) ;
	this->sendOutput( msg.time(), done_d , 1) ;
	return *this ;
}
