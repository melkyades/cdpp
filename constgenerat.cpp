/*******************************************************************
*
*  DESCRIPTION: class Generator
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
#include "constgenerat.h"       // base header
#include "message.h"       // class InternalMessage 
#include "parsimu.h"      // class Simulator
#include "strutil.h"       // str2Value( ... )

/*******************************************************************
* Function Name: Generator
* Description: constructor
********************************************************************/
ConstGenerator::ConstGenerator( const string &name )
	: Atomic( name )
	, out( addOutputPort( "out" ) )
{

	string time( ParallelMainSimulator::Instance().getParameter( description(), "frecuency" ));

	if( time != "" ) {
		preparationTime = time;
	}	
	else
	{
		MException e;
		e.addText( "No frecuency parameter has been found for the model " + description() );
		MTHROW ( e );
	}
}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &ConstGenerator::initFunction()
{
	holdIn( AtomicState::active, preparationTime ) ;
	return *this ;
}

/*******************************************************************
* Function Name: internalFunction
********************************************************************/
Model &ConstGenerator::internalFunction( const InternalMessage & )
{
	holdIn( AtomicState::active, preparationTime) ;
	return *this;
}

/*******************************************************************
* Function Name: outputFunction
********************************************************************/
Model &ConstGenerator::outputFunction( const CollectMessage &msg )
{
	sendOutput( msg.time(), out, (Value) 0 ) ;
	return *this ;
}

