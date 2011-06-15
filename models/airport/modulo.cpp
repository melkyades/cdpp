/******************************************************************************
* Model: 	Airport
* Component:	Modulo
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
#include "modulo.h"      // class modulo
#include "message.h"    // class ExternalMessage, InternalMessage
#include "parsimu.h"   // MainSimulator::Instance().getParameter( ... )

/** public functions **/
/*******************************************************************
* CLASS moduloState
*********************************************************************/
/*******************************************************************
* Function Name: operator =
*********************************************************************/
moduloState& moduloState::operator=(moduloState& thisState) {

	(AtomicState &)*this  = (AtomicState &) thisState;
	this->accion = thisState.accion;

	return *this;	
}

/*******************************************************************
* Function Name: copyState
*********************************************************************/
void moduloState::copyState(moduloState *rhs) {
	*this = *((moduloState *) rhs);	
}

/*******************************************************************
* Function Name: getSize
*********************************************************************/
int  moduloState::getSize() const {
	return sizeof( moduloState );
}

/*******************************************************************
* CLASS modulo
*********************************************************************/

/*******************************************************************
* Function Name: modulo
* Description: 
********************************************************************/
modulo::modulo( const string &name )
	: Atomic( name )
	, aterriza( this->addInputPort( "aterriza" ) )
	, despega( this->addInputPort( "despega" ) )
	, salida_a( this->addOutputPort( "salida_a" ) )	
	, salida_d( this->addOutputPort( "salida_d" ) )



{
}



/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &modulo::initFunction()
{
	avion_numero=0 ;
	return *this ;
	Pista_en_uso="00:01:00:00";
}


/*******************************************************************
* Function Name: externalFunction
* Description:
********************************************************************/
Model &modulo::externalFunction( const ExternalMessage &msg )
{
	if( msg.port() == aterriza )
	{
		accion ("aterriza");
		avion_numero = msg.value()  ;
		this->holdIn( AtomicState::active, Pista_en_uso );
	}

	if( msg.port() == despega )
	{
		accion ("despega");
		avion_numero = msg.value()  ;
		this->holdIn( AtomicState::active, Pista_en_uso );
	}



	return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description:
********************************************************************/
Model &modulo::internalFunction( const InternalMessage & )
{
	this->passivate();
	return *this ;
}


/*******************************************************************
* Function Name: outputFunction
* Description:
********************************************************************/
Model &modulo::outputFunction( const CollectMessage &msg )

{
	if (accion() == "despega")
		this->sendOutput( msg.time(), salida_d , avion_numero) ;

	if (accion() == "aterriza")
		this->sendOutput( msg.time(), salida_a , avion_numero) ;


	return *this ;
}
