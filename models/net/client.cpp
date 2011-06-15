/*******************************************************************
*
*  DESCRIPCION: Client (Genera las solicitudes a los servidores)
*
*  AUTORES:
*  		Ing. Carlos Giorgetti
*          	Iv�n A. Melgrati
*          	Dra. Ana Rosa Tymoschuk
*
*	v2:	Alejandro Troccoli
*
*  EMAIL: mailto://cgiorget@frsf.utn.edu.ar
*         mailto://imelgrat@frsf.utn.edu.ar
*         mailto://anrotym@alpha.arcride.edu.ar
*         mailto://atroccol@dc.uba.ar (v2)
*
*  FECHA: 19/10/1999
*	   05/02/2001 (v2)
*******************************************************************/
#include <stdlib.h>
#include <time.h>
#include "client.h"
#include "message.h"
#include "parsimu.h"
#include "strutil.h"
#include <stdio.h>


// ** main ** //
static const unsigned long unifc = 1234567891;  // Estos son los valores de la
static const unsigned long unifa = 2564393833;  // sucesion lineal congruencial
static const unsigned long unifm = 2147483648;  // POTENCIA = 11
static unsigned long unifx0=0;                  // Semilla de nuestra suc.lineal

/*******************************************************************
* CLASS clientState
*********************************************************************/

/*******************************************************************
* Function Name: operator =
*********************************************************************/
clientState& clientState::operator=(clientState& thisState) {

	(AtomicState &)*this  = (AtomicState &) thisState;
	this->idtask = thisState.idtask;
	return *this;	
}

/*******************************************************************
* Function Name: copyState
*********************************************************************/
void clientState::copyState(clientState *rhs) {
	*this = *((clientState *) rhs);	
}

/*******************************************************************
* Function Name: getSize
*********************************************************************/
int  clientState::getSize() const {
	return sizeof( clientState );
}

/*******************************************************************
* CLASS clientState
*********************************************************************/
/*******************************************************************
* Nombre de la Funci�n: Uniforme()
* Descripci�n: Genera n�meros al azar entre 0 y 1 con distribuci�n uniforme
********************************************************************/
double Uniforme(void)   // Devuelve un numero al azar segun una
{                       // distribucion uniforme, comprendido entre
                        // 0 y 1 (Nunca alcanza 1, o sea 0<=x<1 )
	time_t tiempo;

	if (unifx0 == 0)
		unifx0 = clock()+time(&tiempo);
	else
		unifx0 = (unifa*unifx0+unifc) % unifm;

	return ((double) unifx0) / ((double) unifm);
}
/*******************************************************************
* Nombre de la Funci�n: Client::Client()
* Descripci�n: Constructor
********************************************************************/
Client::Client( const string &name ) : Atomic( name ), out( addOutputPort( "out" ) )
{
	try
	{
        // Leer datos de la distribuci�n
		dist = Distribution::create( ParallelMainSimulator::Instance().getParameter( description(), "distribution" ) );
		MASSERT( dist ) ;
		for ( register int i = 0; i < dist->varCount(); i++ )
		{
			string parameter( ParallelMainSimulator::Instance().getParameter( description(), dist->getVar( i ) ) ) ;
			dist->setVar( i, str2Value( parameter ) ) ;
		}


        // Leer valor inicial del ID de la tarea
		if( ParallelMainSimulator::Instance().existsParameter( description(), "initial" ) )
			initial = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "initial" ) );
		else
			initial = 0;

        // Leer valor del incremento del ID de la tarea
		if( ParallelMainSimulator::Instance().existsParameter( description(), "increment" ) )
			increment = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "increment" ) );
		else
			increment = 1;

        // Leer IP Address del Cliente
		if( ParallelMainSimulator::Instance().existsParameter( description(), "address" ) )
			IPAddress = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "address" ) );
		else
			IPAddress = 0;

        // Leer porcentaje de carga en el servidor 1
		if( ParallelMainSimulator::Instance().existsParameter( description(), "cargaserver1" ) )
			cargaserver1 = str2float( ParallelMainSimulator::Instance().getParameter( description(), "cargaserver1" ) );
		else
			cargaserver1 = 0.5;

	} catch( InvalidDistribution &e )
	{
		e.addText( "The model " + description() + " has distribution problems!" ) ;
		e.print(cerr);
		MTHROW( e ) ;
	} catch( MException &e )
	{
		MTHROW( e ) ;
	}
}

/*******************************************************************
* Nombre de la Funci�n: Client::initFunction()
* Descripci�n: Inicializa el modelo y lo pone en funcionamiento
********************************************************************/
Model &Client::initFunction()
{
	double f;
	time_t t;
	idtask( initial );
	pid.r.idtask = idtask();                                            	// ID inicial
	pid.r.origin = IPAddress;				    // Direcci�n IP
	srand((unsigned) time(&t));
	f= Uniforme();
	pid.r.destino =  ( f > cargaserver1 ) ? 1 : 0; // Servidor de destino
	holdIn( AtomicState::active, VTime::Zero ) ;     					 // Programar primer env�o de inmediato
	return *this ;

}

/*******************************************************************
* Nombre de la Funci�n: Client::internalFunction()
* Descripci�n: Programa el siguiente pedido a los servidores
********************************************************************/
Model &Client::internalFunction( const InternalMessage & )
{
	holdIn( AtomicState::active, VTime( static_cast< float >( fabs( distribution().get() ) ) ) ) ;
	return *this ;
}

/*******************************************************************
* Nombre de la Funci�n: Client::outputFunction()
* Descripci�n: Env�a el pedido actual y prepara el siguiente
********************************************************************/
Model &Client::outputFunction( const CollectMessage &msg )
{
	time_t t;
	double f;
	idtask( idtask() + increment );
	pid.r.idtask = idtask();                           // Siguiente ID de tarea
	pid.r.origin = IPAddress;                            // Copiar ID Address
	srand((unsigned) time(&t));
	f= Uniforme();
	pid.r.destino =  ( f > cargaserver1 ) ? 1 : 0; // Servidor de destino
	sendOutput( msg.time(), out, pid.f ) ;               // Enviar petici�n


	return *this ;
}

