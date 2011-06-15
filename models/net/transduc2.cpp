/*******************************************************************
*
*  DESCRIPTION: class Transducer2 (implementation)
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

/** include files **/
#include "transduc2.h"   // base class
#include "message.h"    // class ExternalMessage 
#include "parsimu.h"       // class ParallelMainSimulator
#include "mformats.h"
#include <stdio.h>

/*******************************************************************
* CLASS Transducer2State
*********************************************************************/
/*******************************************************************
* Function Name: operator =
*********************************************************************/
Transducer2State& Transducer2State::operator=(Transducer2State& thisState) {

	(AtomicState &)*this  = (AtomicState &) thisState;
	this->procCount = thisState.procCount;
	this->cpuLoad = thisState.cpuLoad;
	this->unsolved = thisState.unsolved;

	return *this;	
}

/*******************************************************************
* Function Name: copyState
*********************************************************************/
void Transducer2State::copyState(Transducer2State *rhs) {
	*this = *((Transducer2State *) rhs);	
}

/*******************************************************************
* Function Name: getSize
*********************************************************************/
int  Transducer2State::getSize() const {
	return sizeof( Transducer2State );
}

/*******************************************************************
* CLASS Transducer
*********************************************************************/

/*******************************************************************
* Function Name: Transducer2
* Description: Constructor
********************************************************************/
Transducer2::Transducer2( const string &name )
	: Atomic( name ) 
	, arrived( addInputPort( "arrived" ) )
	, solved( addInputPort( "solved" ) )
	, throughput( addOutputPort( "throughput" ) )
	, cpuUsage( addOutputPort( "cpuusage" ) )
	, responsetime(addOutputPort( "responsetime"))
{
	frec = "0:1:0:0" ;
	tUnit = "0:1:0:0" ;

	if( ParallelMainSimulator::Instance().existsParameter( description(), "frecuence" ) )
		frec = ParallelMainSimulator::Instance().getParameter( description(), "frecuence" ) ;

	if( ParallelMainSimulator::Instance().existsParameter( description(), "timeUnit" ) )
		tUnit = ParallelMainSimulator::Instance().getParameter( description(), "timeUnit" ) ;
}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &Transducer2::initFunction()
{
	procCount ( 0 ) ;
	cpuLoad ( 0 );
	unsolved().erase( unsolved().begin(), unsolved().end() ) ;
	holdIn( AtomicState::active, frecuence() ) ;
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
********************************************************************/
Model &Transducer2::externalFunction( const ExternalMessage &msg )
{
	long id;
	union request req;
	cpuLoad ( cpuLoad() + static_cast< long >( ( msg.time() - lastChange() ).asMsecs() * unsolved().size() ));

	if( msg.port() == arrived )
	{
		req = (union request) msg.value();
		id = req.r.idtask * 100 + req.r.origin;		

		if( unsolved().find( id ) != unsolved().end() )
		{
			MException e( string("Unresolved Work Id: ") + (float) id + " is duplicated." );
			e.addLocation( MEXCEPTION_LOCATION() );
			throw e;
		}

		unsolved()[ id ] = msg.time() ;
	}

	if( msg.port() == solved )
	{
		req = (union request) msg.value();
		id = req.r.idtask * 100 + req.r.destino;	

		Transducer2State::JobsList::iterator cursor( unsolved().find( id) ) ;

		if( cursor == unsolved().end() )
		{
			MException e( string("Resolved Work Id: ") + (float) id + " Not Found!" );
			e.addLocation( MEXCEPTION_LOCATION() );
			throw e;
		}

		procCount( procCount() + 1 );
		unsolved().erase( cursor ) ;
	}
	return *this ;
}

/*******************************************************************
* Function Name: internalFunction
********************************************************************/
Model &Transducer2::internalFunction( const InternalMessage &)
{
	holdIn( AtomicState::active, frecuence() );
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
********************************************************************/
Model &Transducer2::outputFunction( const CollectMessage &msg )
{
	float time( msg.time().asMsecs() / timeUnit().asMsecs() ) ;

	sendOutput( msg.time(), throughput, procCount() / time ) ;

	cpuLoad ( cpuLoad() + static_cast< long >( ( msg.time() - lastChange() ).asMsecs() * unsolved().size() )) ;

	sendOutput( msg.time(), cpuUsage  , cpuLoad() / msg.time().asMsecs() ) ;

	sendOutput( msg.time(), responsetime, (cpuLoad() / msg.time().asMsecs() ) / (procCount() / time) ); 

	return *this ;
}
