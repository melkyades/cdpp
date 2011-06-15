
/** include files **/
#include <string>

/** my include files **/
#include "cola.h"      // class cola
#include "message.h"    // class ExternalMessage, InternalMessage
#include "parsimu.h"   // MainSimulator::Instance().getParameter( ... )

/*******************************************************************
* CLASS colaState
*********************************************************************/
/*******************************************************************
* Function Name: operator =
*********************************************************************/
colaState& colaState::operator=(colaState& thisState) {

	(AtomicState &)*this  = (AtomicState &) thisState;
	this->ocupado = thisState.ocupado;
	this->elements = thisState.elements;

	return *this;	
}

/*******************************************************************
* Function Name: copyState
*********************************************************************/
void colaState::copyState(colaState *rhs) {
	*this = *((colaState *) rhs);	
}

/*******************************************************************
* Function Name: getSize
*********************************************************************/
int  colaState::getSize() const {
	return sizeof( colaState );
}

/*******************************************************************
* CLASS cola
*********************************************************************/
/** public functions **/

/*******************************************************************
* Function Name: cola
* Description: 
********************************************************************/
cola::cola( const string &name )
	: Atomic( name )
	, in( this->addInputPort( "in" ) )
	, stop( this->addInputPort( "stop" ) )
	, done( this->addInputPort( "done" ) )
	, out( this->addOutputPort( "out" ) )
	, preparationTime( 0, 0, 0, 0 )
{
	string time( ParallelMainSimulator::Instance().getParameter( this->description(), "preparation" ) ) ;

	if( time != "" )
		preparationTime = time ;
}



/*******************************************************************
* Function Name: initFunction

********************************************************************/
Model &cola::initFunction()
{
	ocupado (0);
	elements().erase( elements().begin(), elements().end() ) ;
	return *this ;
}


/*******************************************************************
* Function Name: externalFunction
* Description: 
********************************************************************/
Model &cola::externalFunction( const ExternalMessage &msg )
{
	if( msg.port() == in )
	{
		elements().push_back( msg.value() ) ;
		this->holdIn( AtomicState::active, preparationTime );
	}

	if( msg.port() == done )
	{
		ocupado (0);
		this->holdIn( AtomicState::active, preparationTime );
	}

	if( msg.port() == stop )
	{ 
		ocupado (1);
		this->passivate();
	}

	return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description: 
********************************************************************/
Model &cola::internalFunction( const InternalMessage & )
{
	this->passivate();
	return *this ;
}


/*******************************************************************
* Function Name: outputFunction
* Description: 
********************************************************************/
Model &cola::outputFunction( const CollectMessage &msg )
{

	if( (elements().size() > 0)  && ocupado()==0 )
	{ 
		this->sendOutput( msg.time(), out, elements().front() ) ;
		elements().pop_front() ;
	}
	return *this ;
}
