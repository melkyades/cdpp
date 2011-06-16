/** include files **/
#include <string>

/** my include files **/
#include "modulo.h"      // class modulo
#include "message.h"    // class ExternalMessage, InternalMessage
#include "parsimu.h"   // ParallelMainSimulator::Instance().getParameter( ... )

using namespace std;

/** public functions **/

/*******************************************************************
* Function Name: modulo
* Description: 
********************************************************************/
modulo::modulo( const string &name )
: Atomic( name )
, recibe_avion( this->addInputPort( "recibe_avion" ) )
, emergencia( this->addOutputPort( "emergencia" ) )
, sigue_avion( this->addOutputPort( "sigue_avion" ) )
, salida_hangar( this->addOutputPort( "salida_hangar" ) )
, en_uso( this->addOutputPort( "en_uso" ) )

{
        string time( ParallelMainSimulator::Instance().getParameter( this->description(), "preparation" ) ) ;

	if( time != "" )
                preparationTime = time ;
}



/*******************************************************************
* Function Name: initFunction

* Precondition: El tiempo del proximo evento interno es ...
********************************************************************/
Model &modulo::initFunction()
{
	   avion_numero=0 ;
	   velocidad=0
	   return *this ;
}


/*******************************************************************
* Function Name: externalFunction
* Description:
********************************************************************/
Model &modulo::externalFunction( const ExternalMessage &msg )
{
		if( msg.port() == recibe_avion )
	{

				avion_numero = msg.value()  ;
				velocidad = floor(msg.value())     ;//Toma su parte entera
				avion_numero = avion_numero - velocidad ; //El Id del vuelo queda en avion_numero

				this->holdIn( active, preparationTime );
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
Model &modulo::outputFunction( const InternalMessage &msg )

{
		this->sendOutput( msg.time(), sigue_avion , (velocidad + avion_numero)) ;
                return *this ;
}
