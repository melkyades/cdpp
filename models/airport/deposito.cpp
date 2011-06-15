
/** include files **/
#include <string>

/** my include files **/
#include "deposito.h"      // class deposito
#include "message.h"    // class ExternalMessage, InternalMessage
#include "parsimu.h"   // ParallelMainSimulator::Instance().getParameter( ... )

/*******************************************************************
* CLASS depositoState
*********************************************************************/
/*******************************************************************
* Function Name: operator =
*********************************************************************/
depositoState& depositoState::operator=(depositoState& thisState) {

	(AtomicState &)*this  = (AtomicState &) thisState;
	this->elements = thisState.elements;

	return *this;	
}

/*******************************************************************
* Function Name: copyState
*********************************************************************/
void depositoState::copyState(depositoState *rhs) {
	*this = *((depositoState *) rhs);	
}

/*******************************************************************
* Function Name: getSize
*********************************************************************/
int  depositoState::getSize() const {
	return sizeof( depositoState );
}

/*******************************************************************
* CLASS deposito
*********************************************************************/
/** public functions **/

/*******************************************************************
* Function Name: deposito
* Description: 
********************************************************************/
deposito::deposito( const string &name )
	: Atomic( name )
	, in( this->addInputPort( "in" ) )
	, out( this->addOutputPort( "out" ) )
	, preparationTime( 0, 0, 0, 0 )
{
}



/*******************************************************************
* Function Name: initFunction
* Precondition: El tiempo del proximo evento interno es Infinito
********************************************************************/
Model &deposito::initFunction()
{

	elements().erase( elements().begin(), elements().end() ) ;
	return *this ;
}


/*******************************************************************
* Function Name: externalFunction
* Description: 
********************************************************************/
Model &deposito::externalFunction( const ExternalMessage &msg )
{
	if( msg.port() == in )
	{
		elements().push_back( msg.value() ) ;
		this->holdIn( AtomicState::active, preparationTime);
	}


	return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description:
********************************************************************/
Model &deposito::internalFunction( const InternalMessage & )
{
	this->passivate();
	return *this ;
}


/*******************************************************************
* Function Name: outputFunction
* Description:
********************************************************************/
Model &deposito::outputFunction( const CollectMessage &msg )
{

	if( elements().size() > 0 )
	{ 
		this->sendOutput( msg.time(), out, elements().front() ) ;
		elements().pop_front() ;

	}
	return *this ;
}
