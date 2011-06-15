/*******************************************************************
*
*  DESCRIPCION: Cola Gen�rica (on demand)
*
*  AUTORES:
*  	    Ing. Carlos Giorgetti
*          Iv�n A. Melgrati
*          Dra. Ana Rosa Tymoschuk
*	v2:Alejandro Troccoli
*
*  EMAIL: mailto://cgiorget@frsf.utn.edu.ar
*         mailto://imelgrat@frsf.utn.edu.ar
*         mailto://anrotym@alpha.arcride.edu.ar
*	   mailto://atroccol@dc.uba.ar (v2)
*
*  FECHA: 15/10/1999
*         01/02/2001
*******************************************************************/

#include <string.h>
#include "queue.h"
#include "message.h"
#include "parsimu.h"

/*******************************************************************
* CLASS QueueState
*********************************************************************/
/*******************************************************************
* Function Name: operator =
*********************************************************************/
QueueState& QueueState::operator=(QueueState& thisState) {

	(AtomicState &)*this  = (AtomicState &) thisState;
	this->elements = thisState.elements;

	return *this;	
}

/*******************************************************************
* Function Name: copyState
*********************************************************************/
void QueueState::copyState(QueueState *rhs) {
	*this = *((QueueState *) rhs);	
}

/*******************************************************************
* Function Name: getSize
*********************************************************************/
int  QueueState::getSize() const {
	return sizeof( QueueState );
}

/*******************************************************************
* CLASS Queue
*********************************************************************/

/*******************************************************************
* Nombre de la Funci�n: Queue::Queue()
* Descripci�n: Constructor
********************************************************************/
Queue::Queue( const string &name ) : Atomic( name ), in( addInputPort( "in" ) )
	, done( addInputPort( "done" ) ) , out( addOutputPort( "out" ) )
	, preparationTime( 0, 0, 0, 1 )
{
	string time( ParallelMainSimulator::Instance().getParameter( description(), "preparation" ) ) ;

	if( time != "" )
		preparationTime = time ;
}


/*******************************************************************
* Nombre de la Funci�n: Queue::initFunction()
* Descripci�n: Funci�n de Inicializaci�n
********************************************************************/

Model &Queue::initFunction()
{
	elements().erase( elements().begin(), elements().end() ) ;
	return *this ;
}


/*******************************************************************
* Nombre de la Funci�n: Queue::externalFunction()
* Descripci�n: Maneja los eventos externos (nuevas solicitudes y aviso de "listo"
********************************************************************/

Model &Queue::externalFunction( const ExternalMessage &msg )
{
#ifdef DEVS_DELAY_EXTERNAL
	string a;
	for (int i = 1; i < 100000; i++ ) {
		if ( a.length() > 100 )
			a = "";
		else
			a += i;
	}
#endif	

	if( msg.port() == in )                             	// Si entra una nueva petici�n
	{
		elements().push_back( msg.value() ) ;             // Encolarla
		if( elements().size() == 1 )                      // Si no hay otra, preparla para env�o
			holdIn( AtomicState::active, preparationTime );
	}

	if( msg.port() == done )                            // Si notifican condici�n "listo"
	{
		elements().pop_front() ;                          // Eliminar solicitud actual de cola
		if( !elements().empty() )
			holdIn( AtomicState::active, preparationTime );
			// Programar siguiente env�o
	}

	return *this;
}

/*******************************************************************
* Nombre de la Funci�n: Queue::internalFunction()
* Descripci�n: Pone el modelo en estado pasivo (esperando un "Done" o algo para enviar)
********************************************************************/
Model &Queue::internalFunction( const InternalMessage & )
{
#ifdef DEVS_DELAY_INTERNAL
	string a;
	for (int i = 1; i < 100000; i++ ) {
		if ( a.length() > 100 )
			a = "";
		else
			a += i;
	}
#endif
	passivate();
	return *this ;
}


/*******************************************************************
* Nombre de la Funci�n: Queue::outputFunction()
* Descripci�n: Env�a solicitud al receptor
********************************************************************/
Model &Queue::outputFunction( const CollectMessage &msg )
{
	if( !elements().empty() )   // Si la cola no est� vac�a, enviar primer elemento
		sendOutput( msg.time(), out, elements().front() ) ;
	return *this ;
}
