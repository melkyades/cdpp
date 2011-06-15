/*******************************************************************
*
*  DESCRIPCION: Cola Gen�rica con Stop (detiene a la cola)
*
*  AUTORES:
*  	    Ing. Carlos Giorgetti
*          Iv�n A. Melgrati
*          Dra. Ana Rosa Tymoschuk
*	    Alejandro Troccoli (v2)
*
*  EMAIL: mailto://cgiorget@frsf.utn.edu.ar
*         mailto://imelgrat@frsf.utn.edu.ar
*         mailto://anrotym@alpha.arcride.edu.ar
*	   mailto://atroccol@dc.uba.ar (v2)
*
*  FECHA: 25/10/1999
*	   05/01/2001 (v2)
*
*******************************************************************/

#include "stopq.h"
#include "message.h"
#include "parsimu.h"

/*******************************************************************
* CLASS StoppableQueueState
*********************************************************************/
/*******************************************************************
* Function Name: operator =
*********************************************************************/
StoppableQueueState& StoppableQueueState::operator=(StoppableQueueState& thisState) {

	(AtomicState &)*this  = (AtomicState &) thisState;
	this->elements = thisState.elements;
	this->stopped = thisState.stopped;
	this->timeLeft = thisState.timeLeft;
	return *this;	
}

/*******************************************************************
* Function Name: copyState
*********************************************************************/
void StoppableQueueState::copyState(StoppableQueueState *rhs) {
	*this = *((StoppableQueueState *) rhs);	
}

/*******************************************************************
* Function Name: getSize
*********************************************************************/
int  StoppableQueueState::getSize() const {
	return sizeof( StoppableQueueState );
}

/*******************************************************************
* CLASS StoppableQueue
*********************************************************************/

/*******************************************************************
* Nombre de la Funci�n: StoppableQueue::StoppableQueue()
* Descripci�n: Constructor
********************************************************************/
StoppableQueue::StoppableQueue( const string &name ) : Atomic( name ), in( addInputPort( "in" ) )
	, stop( addOutputPort( "stop" ) ) , out( addOutputPort( "out" ) ), preparationTime( 0, 0, 0, 1 )
{
	string time( ParallelMainSimulator::Instance().getParameter( description(), "preparation" ) ) ;

	if( time != "" )
		preparationTime = time ;
}

/*******************************************************************
* Nombre de la Funci�n: StoppableQueue::externalFunction()
* Descripci�n: Maneja los eventos externos (nuevas solicitudes y aviso de "stop"
********************************************************************/
Model &StoppableQueue::externalFunction( const ExternalMessage &msg )
{
	if( msg.port() == in )                             	// Si entra una nueva petici�n
	{
		elements().push_back( msg.value() ) ;             // Encolarla
		if((!stopped()) && ( elements().size() == 1 ))    // Si no est� detenida y hay una sola, enviar
			holdIn( AtomicState::active, preparationTime );
	}

	if (msg.port() == stop)
	{
		if (msg.value() != 0)
		{
			stopped ( 1 );
			if (state() == AtomicState::active)
			{
				timeLeft() = nextChange();
				passivate();
			}
		}
		else
		{
			stopped ( 0 );
			holdIn(AtomicState::active, timeLeft());
		}
	}

	return *this;
}

/*******************************************************************
* Nombre de la Funci�n: StoppableQueue::internalFunction()
* Descripci�n: Prepara el siguiente elemento a enviar si la cola no
*              est� detenida y tiene elementos para enviar
********************************************************************/
Model &StoppableQueue::internalFunction( const InternalMessage & )
{
	if ((!stopped()) && (!elements().empty()))
	{
		elements().pop_front() ;                          // Eliminar solicitud actual de cola
		if( !elements().empty() )
			holdIn( AtomicState::active, preparationTime );    // Programar siguiente env�o
		else
			passivate();
	}
	return *this;
}

/*******************************************************************
* Nombre de la Funci�n: StoppableQueue::outputFunction()
* Descripci�n: Env�a solicitud al receptor
********************************************************************/
Model &StoppableQueue::outputFunction( const CollectMessage &msg )
{
	if( !elements().empty() )   // Si la cola no est� vac�a, enviar primer elemento
		sendOutput( msg.time(), out, elements().front() ) ;
	return *this ;
}
/*******************************************************************
* Nombre de la Funci�n: StoppableQueue::initFunction()
* Descripci�n: Funci�n de Inicializaci�n
********************************************************************/

Model &StoppableQueue::initFunction()
{
	elements().erase( elements().begin(), elements().end() ) ;  // Limpiar cola
	stopped ( 0 );
	return *this ;                                        // La cola no est� detenida
}
