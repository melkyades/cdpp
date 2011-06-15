/******************************************************************************
* Model: 	Airport
* Component:	Selector
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
#include "selector.h"      // class selector
#include "message.h"    // class ExternalMessage, InternalMessage
#include "parsimu.h"   // MainSimulator::Instance().getParameter( ... )

/** public functions **/

/*******************************************************************
* Function Name: selector
* Description: 
********************************************************************/

selector::selector( const string &name )
	: Atomic( name )
	, in( this->addInputPort( "in" ) )
	, out1( this->addOutputPort( "out1" ) )
	, out2( this->addOutputPort( "out2" ) )
	, out3( this->addOutputPort( "out3" ) )
	, out4( this->addOutputPort( "out4"  ) )

{
}



/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &selector::initFunction()
{
	Demora = "00:00:30:00" ;
	return *this ;
}


/*******************************************************************
* Function Name: externalFunction
* Description:
********************************************************************/
Model &selector::externalFunction( const ExternalMessage &msg )
{
	if( msg.port() == in )
	{
		id = msg.value()  ;
		this->holdIn( AtomicState::active, Demora );
	}


	return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description:
********************************************************************/
Model &selector::internalFunction( const InternalMessage & )
{
	this->passivate();
	return *this ;
}


/*******************************************************************
* Function Name: outputFunction
* Description:
********************************************************************/
Model &selector::outputFunction( const CollectMessage &msg )

{
	if (id <= 254)
		this->sendOutput( msg.time(), out1 , id);

	if (id > 254 && id <=499)
		this->sendOutput( msg.time(), out2 , id) ;

	if (id > 499 && id <=754)
		this->sendOutput( msg.time(), out3 , id) ;

	if (id > 754 && id <=1000)
		this->sendOutput( msg.time(), out4 , id) ;


	return *this ;

}
