/*******************************************************************
*
*  DESCRIPTION: class Congestion
*
*  AUTHOR: Andrea Díaz && Verónica Vazquez
*
*  EMAIL: rodiaz@dc.uba.ar		
*  	  vvazqu@dc.uba.ar	
*
*  DATE: 24/01/2000
*
*******************************************************************/

/** include files **/
#include "congest.h"       // base header
#include "message.h"       // class InternalMessage 
#include "parsimu.h"      // class Simulator
#include "distri.h"        // class Distribution 
#include "strutil.h"       // str2Value( ... )

/*******************************************************************
* Function Name: Congestion
* Description: constructor
********************************************************************/
Congestion::Congestion( const string &name)
	:Atomic(name)
	, IngresaAuto(this->addInputPort("IngresaAuto"))
	, SaleAuto(this->addInputPort("SaleAuto"))
	, stop(this->addInputPort("stop"))
	, peso(this->addOutputPort("peso"))
{
}
//******************************************************************
Model &Congestion::initFunction()
{
  cant_autos = 0;
  return *this;
}
//******************************************************************
Model &Congestion::externalFunction(const ExternalMessage &msg)
{
  if (msg.port() == IngresaAuto )
  {
     cant_autos++;
     this->passivate();
  }
  if (msg.port() == SaleAuto )
  {
     cant_autos--;
     this->passivate();
  }
  if (msg.port() == stop )
  {
    	this->passivate();
  }

  return *this;
}
//******************************************************************
Model &Congestion::outputFunction( const InternalMessage &msg) 
{
	this->sendOutput(msg.time(), peso, cant_autos );
}
//******************************************************************
Model &Congestion::internalFunction (const InternalMessage &msg)
{
	return *this;
}
//******************************************************************
