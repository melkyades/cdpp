/*******************************************************************
*
*  DESCRIPCION: clase DISCO (procesa pedidos a disco de un tamaño deter-
*               minado (pasado como mensaje)
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
#include <math.h>        // fabs( ... )
#include "disco.h"           // base header
#include "message.h"       // InternalMessage ....
#include "parsimu.h"      // class ParallelMainSimulator
#include "strutil.h"       // str2float( ... )

/*******************************************************************
* CLASS DiscoState
*********************************************************************/

/*******************************************************************
* Function Name: operator =
*********************************************************************/
DiscoState& DiscoState::operator=(DiscoState& thisState) {

	(AtomicState &)*this  = (AtomicState &) thisState;
	this->requerimiento.f = thisState.requerimiento.f;
	return *this;	
}

/*******************************************************************
* Function Name: copyState
*********************************************************************/
void DiscoState::copyState(DiscoState *rhs) {
	*this = *((DiscoState *) rhs);	
}

/*******************************************************************
* Function Name: getSize
*********************************************************************/
int  DiscoState::getSize() const {
	return sizeof( DiscoState );
}

/*******************************************************************
* CLASS Disco
*********************************************************************/
/*******************************************************************
* Nombre de la Función: Disco::Disco
* Descripción: Constructor
********************************************************************/
Disco::Disco( const string &name ): Atomic( name ), in( addInputPort( "in" ) ), out( addOutputPort( "out" ) )
{
	try
	{
		dist = Distribution::create( "poisson" );    // Inicializar la Distribución del servicio

		MASSERT( dist );
		requerimiento().f = 0.0;                         // Limpiar el requerimiento

	} catch( InvalidDistribution &e )
	{
		e.addText( "El modelo" + description() + " tiene problemas en la definición de la distribución!" ) ;
		e.print(cerr);
		MTHROW( e ) ;
	} catch( MException &e )
	{
		MTHROW( e ) ;
	}
	if( ParallelMainSimulator::Instance().existsParameter( description(), "velocidad" ) )
		velocidad = str2float( ParallelMainSimulator::Instance().getParameter( description(), "velocidad" ) );
	else
		velocidad = 0.009;			//  Tiempo para leer 1 KB = 9ms (default)

}

/*******************************************************************
* Nombre de la Función: Disco::externalFunction
* Descripción: El Disco recibe una petición y se ocupa.
*              Si llega otra petición mientras está ocupado, desaloja la anterior
********************************************************************/
Model &Disco::externalFunction( const ExternalMessage &msg )
{
	requerimiento() = static_cast<union request>( msg.value() ) ;
	float size = (float)requerimiento().r.tamanio * velocidad;
	dist->setVar(0, size);                       // Valor medio del tiempo de servicio
	holdIn( AtomicState::active, VTime( static_cast<float>( fabs( distribution().get() ) ) ) ) ;    // Ocupar el disco
	return *this ;
}

/*******************************************************************
* Nombre de la Función: Disco::internalFunction
* Descripción: Cuando termina su trabajo (tr. interna) el disco se pone en estado pasivo
********************************************************************/
Model &Disco::internalFunction( const InternalMessage & )
{
	passivate();
	return *this;
}

/*******************************************************************
* Nombre de la Función: Disco::outputFunction
* Descripción: Envía el trabajo (terminado) al puerto out
********************************************************************/
Model &Disco::outputFunction( const CollectMessage &msg )
{
	sendOutput( msg.time(), out, requerimiento().f );
	return *this;
}

