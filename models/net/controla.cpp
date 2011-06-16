/*******************************************************************
*
*  DESCRIPCION: clase CONTROLADORA (Rutea los pedidos a los Discos
*                                   y a la CPU)
*
*  AUTORES: Ing. Carlos Giorgetti
*           Iván Melgrati
*           Dra. Ana Rosa Tymoschuk
*
*  EMAIL: mailto://cgiorget@frsf.utn.edu.ar
*         mailto://imelgrat@frsf.utn.edu.ar
*         mailto://anrotym@alpha.arcride.edu.ar
*
*  FECHA: 09/10/1999
*
*******************************************************************/


/** include files **/
#include "controla.h"       // base header
#include "message.h"       // InternalMessage ....
#include "parsimu.h"      // class ParallelMainSimulator
#include "strutil.h"       // str2float( ... )
#include <cstdlib>      // random() y randomize()
#include <ctime>
#include <stdio.h>

/*******************************************************************
* CLASS ControladoraState
*********************************************************************/

/*******************************************************************
* Function Name: operator =
*********************************************************************/
ControladoraState& ControladoraState::operator=(ControladoraState& thisState) {

	(AtomicState &)*this  = (AtomicState &) thisState;
	this->fase = thisState.fase;
	this->requerimiento.f = thisState.requerimiento.f;
	return *this;	
}

/*******************************************************************
* Function Name: copyState
*********************************************************************/
void ControladoraState::copyState(ControladoraState *rhs) {
	*this = *((ControladoraState *) rhs);	
}

/*******************************************************************
* Function Name: getSize
*********************************************************************/
int  ControladoraState::getSize() const {
	return sizeof( ControladoraState );
}

/*******************************************************************
* CLASS Controladora
*********************************************************************/

/*******************************************************************
* Nombre de la Función: Controladora::Controladora
* Descripción: Constructor
********************************************************************/
Controladora::Controladora( const string &name ): Atomic( name ), in( addInputPort( "in" ) ), out( addOutputPort( "out" ) )
	,discoin( addInputPort( "discoin" ) ), discoout1( addOutputPort( "discoout1" ) ), discoout2( addOutputPort( "discoout2" ) )
	, discoout3( addOutputPort( "discoout3" ) ), discoout4( addOutputPort( "discoout4" ) ), discoout5( addOutputPort( "discoout5" ) )
	, discoout6( addOutputPort( "discoout6" ) ), discoout7( addOutputPort( "discoout7" ) ), discoout8( addOutputPort( "discoout8" ) )
{
	fase ( pasiva );                           // Iniciar en Estado Pasivo
	if( ParallelMainSimulator::Instance().existsParameter( description(), "discos" ) )     // Obtener la cantidad de discos (del Script .MA)
		cant = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "discos" ) );
	else
		cant = 1;

	requerimiento().f = 0.0;					 // Limpiar el requerimiento actual

	dist = Distribution::create( ParallelMainSimulator::Instance().getParameter( description(), "distribution" ) );
	for ( register int i = 0; i < dist->varCount(); i++ )
	{
		string parameter( ParallelMainSimulator::Instance().getParameter( description(), dist->getVar( i ) ) ) ;
		dist->setVar( i, str2Value( parameter ) ) ;
	}
}

/*******************************************************************
* Nombre de la Función: Controladora::externalFunction
* Descripción: Se reciben peticiones desde la CPU. Elige un disco al azar y rutea la petición
*              Si retorna una petición, se envía a la CPU
********************************************************************/
Model &Controladora::externalFunction( const ExternalMessage &msg )
{
	if(msg.port() == in)
	{
		fase ( senddisk );
		requerimiento().f = (double) msg.value();  // Copiar datos de req.
		holdIn(AtomicState::active, VTime::Zero);                 //Enviar al disco inmediatamente
	}
	else
		if (msg.port() == discoin)
		{
			fase ( sendCPU );
			requerimiento().f =  (double) msg.value() ;  // Copiar datos de req.
			holdIn(AtomicState::active, VTime::Zero);                 //Enviar a la CPU inmediatamente
		}

	return *this ;
}

/*******************************************************************
* Nombre de la Función: Controladora::internalFunction
* Descripción: Se pone en estado pasivo, esperando un nuevo pedido
********************************************************************/
Model &Controladora::internalFunction( const InternalMessage & )
{
	fase ( pasiva );
	passivate();
	return *this;
}

/*******************************************************************
* Nombre de la Función: Controladora::outputFunction
* Descripción: Envía el trabajo a los discos o a la CPU
********************************************************************/
Model &Controladora::outputFunction( const CollectMessage &msg )
{
	unsigned int disk;
	time_t t;
	if (fase() == sendCPU)
		sendOutput( msg.time(), out, requerimiento().f );
	else
	{
		srand((unsigned) time(&t));
		requerimiento().r.tamanio = (unsigned int) static_cast<float>( fabs( distribution().get() ) );

		disk = (unsigned int) static_cast<float>( fabs( distribution().get() ) ) % cant;

		switch(disk)
		{
			case 0:
				sendOutput( msg.time(), discoout1, requerimiento().f );
				break;
			case 1:
				sendOutput( msg.time(), discoout2, requerimiento().f );
				break;
			case 2:
				sendOutput( msg.time(), discoout3, requerimiento().f );
				break;
			case 3:
				sendOutput( msg.time(), discoout4, requerimiento().f );
				break;
			case 4:
				sendOutput( msg.time(), discoout5, requerimiento().f );
				break;
			case 5:
				sendOutput( msg.time(), discoout6, requerimiento().f );
				break;
			case 6:
				sendOutput( msg.time(), discoout7, requerimiento().f );
				break;
			case 7:
				sendOutput( msg.time(), discoout8, requerimiento().f );
				break;
			default:
				break;
		}
	}

	return *this;
}

