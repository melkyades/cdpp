/*******************************************************************
*
*  DESCRIPCION: Router (rutea los paquetes de acuerdo a su dirección)
*
*  AUTORES:
*  	    Ing. Carlos Giorgetti
*          Iván A. Melgrati
*          Dra. Ana Rosa Tymoschuk
*	    Alejandro Troccoli (v2)
*
*  EMAIL: mailto://cgiorget@frsf.utn.edu.ar
*         mailto://imelgrat@frsf.utn.edu.ar
*         mailto://anrotym@alpha.arcride.edu.ar
*	   mailto://atroccol@dc.uba.ar (v2)
*
*  FECHA: 18/10/1999
*	   05/01/2001 (v2)
*
*******************************************************************/
#include <string>
#include "router.h"
#include "message.h"
#include "parsimu.h"
#include "mformats.h"

/*******************************************************************
* CLASS RouterState
*********************************************************************/
/*******************************************************************
* Function Name: operator =
*********************************************************************/
RouterState& RouterState::operator=(RouterState& thisState) {

	(AtomicState &)*this  = (AtomicState &) thisState;
	this->elements = thisState.elements;

	return *this;	
}

/*******************************************************************
* Function Name: copyState
*********************************************************************/
void RouterState::copyState(RouterState *rhs) {
	*this = *((RouterState *) rhs);	
}

/*******************************************************************
* Function Name: getSize
*********************************************************************/
int  RouterState::getSize() const {
	return sizeof( RouterState );
}

/*******************************************************************
* CLASS Router
*********************************************************************/

/*******************************************************************
* Nombre de la Función: Router::Router()
* Descripción: Constructor (Inicializa el modelo)
********************************************************************/
Router::Router( const string &name ): Atomic( name )
	, in(addInputPort( "in" )),
out0(addOutputPort("out0")), out1(addOutputPort("out1")), out2(addOutputPort("out2")),
out3(addOutputPort("out3")), out4(addOutputPort("out4")), out5(addOutputPort("out5")),
out6(addOutputPort("out6")), out7(addOutputPort("out7")), out8(addOutputPort("out8")),
out9(addOutputPort("out9")), out10(addOutputPort("out10")), out11(addOutputPort("out11")),
out12(addOutputPort("out12")), out13(addOutputPort("out13")), out14(addOutputPort("out14")),
out15(addOutputPort("out15")), preparationTime( 0, 0, 10, 0 )
{
	string time( ParallelMainSimulator::Instance().getParameter( description(), "preparation" ) ) ;

	if( time != "" )
		preparationTime = time ;
}



/*******************************************************************
* Nombre de la Función: Router::initFunction()
* Descripción: Resetea la lista
********************************************************************/
Model &Router::initFunction()
{
	elements().erase( elements().begin(), elements().end() ) ;
	return *this ;
}


/*******************************************************************
* Nombre de la Función: Router::externalFunction()
* Descripción: Recibe y encola los paquetes recibidos
********************************************************************/
Model &Router::externalFunction( const ExternalMessage &msg )
{
	if( msg.port() == in )
	{
		elements().push_back( msg.value() ) ;
		if( elements().size() == 1 )
			holdIn( AtomicState::active, preparationTime );
	}

	return *this;
}

/*******************************************************************
* Nombre de la Función: Router::internalFunction()
* Descripción:  Prepara el siguiente paquete a enviar (o se pasiva si no hay ninguno)
********************************************************************/
Model &Router::internalFunction( const InternalMessage & )
{
	elements().pop_front();
	if (elements().empty())
		passivate();
	else
		holdIn( AtomicState::active, preparationTime );

	return *this ;
}


/*******************************************************************
* Nombre de la Función: Router::outputFunction()
* Descripción:  Rutea los paquetes a las bocas (ports) correspondientes
********************************************************************/
Model &Router::outputFunction( const CollectMessage &msg )
{
	union request re;

	if( !elements().empty() )
	{
		re.f = (double) elements().front();
		switch (re.r.destino)
		{
			case 0:
				sendOutput( msg.time(), out0, elements().front() ) ;
				break;
			case 1:
				sendOutput( msg.time(), out1, elements().front() ) ;
				break;
			case 2:
				sendOutput( msg.time(), out2, elements().front() ) ;
				break;
			case 3:
				sendOutput( msg.time(), out3, elements().front() ) ;
				break;
			case 4:
				sendOutput( msg.time(), out4, elements().front() ) ;
				break;
			case 5:
				sendOutput( msg.time(), out5, elements().front() ) ;
				break;
			case 6:
				sendOutput( msg.time(), out6, elements().front() ) ;
				break;
			case 7:
				sendOutput( msg.time(), out7, elements().front() ) ;
				break;
			case 8:
				sendOutput( msg.time(), out8, elements().front() ) ;
				break;
			case 9:
				sendOutput( msg.time(), out9, elements().front() ) ;
				break;
			case 10:
				sendOutput( msg.time(), out10, elements().front() ) ;
				break;
			case 11:
				sendOutput( msg.time(), out11, elements().front() ) ;
				break;
			case 12:
				sendOutput( msg.time(), out12, elements().front() ) ;
				break;
			case 13:
				sendOutput( msg.time(), out13, elements().front() ) ;
				break;
			case 14:
				sendOutput( msg.time(), out14, elements().front() ) ;
				break;
			case 15:
				sendOutput( msg.time(), out15, elements().front() ) ;
				break;
			default:
				break;
		}   // Switch
	}     // If
	return *this ;
}
