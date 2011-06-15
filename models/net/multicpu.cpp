/*******************************************************************
*
*  DESCRIPCION: MultiCPU (CPU Multitarea)
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
*  FECHA: 01/04/2000
*	   05/01/2001 (v2)
*
*******************************************************************/

#include "multicpu.h"
#include "parsimu.h"
#include "strutil.h"
#include "message.h"

/*******************************************************************
* CLASS MultiCPUState
*********************************************************************/

/*******************************************************************
* Function Name: operator =
*********************************************************************/
MultiCPUState& MultiCPUState::operator=(MultiCPUState& thisState) {

	(AtomicState &)*this  = (AtomicState &) thisState;
	this->Proc_Inicial = thisState.Proc_Inicial;
	this->Proc_Final = thisState.Proc_Final;
	return *this;	
}

/*******************************************************************
* Function Name: copyState
*********************************************************************/
void MultiCPUState::copyState(MultiCPUState *rhs) {
	*this = *((MultiCPUState *) rhs);	
}

/*******************************************************************
* Function Name: getSize
*********************************************************************/
int  MultiCPUState::getSize() const {
	return sizeof( MultiCPUState );
}

/*******************************************************************
* CLASS MultiCPU
*********************************************************************/
MultiCPU::MultiCPU (const string& name): Atomic(name) , cpuin(addInputPort( "cpuin" )),
cpuout(addOutputPort("cpuout")) , controladorain(addInputPort( "controladorain" )),
controladoraout(addOutputPort("controladoraout"))
{
	// Leer datos de las distribuciones de tiempo de servicio (inicial y final)

	dist_inicial = Distribution::create( "normal" );
	string parameter( ParallelMainSimulator::Instance().getParameter( description(), "media_inicial" ) ) ;
	dist_inicial->setVar(0, str2Value( parameter ) ) ;
	string parameter2( ParallelMainSimulator::Instance().getParameter( description(), "desviacion_inicial" ) ) ;
	dist_inicial->setVar(1, str2Value( parameter2 ) ) ;


	dist_final = Distribution::create( "normal");
	string parameter3( ParallelMainSimulator::Instance().getParameter( description(), "media_final" ) ) ;
	dist_final->setVar(0, str2Value( parameter3 ) ) ;
	string parameter4( ParallelMainSimulator::Instance().getParameter( description(), "desviacion_final" ) ) ;
	dist_final->setVar(1, str2Value( parameter4 ) ) ;
}

Model &MultiCPU::initFunction()
{
	Proc_Inicial().erase (Proc_Inicial().begin(), Proc_Inicial().end());
	Proc_Final().erase (Proc_Final().begin(), Proc_Final().end());
	return *this;
}



Model &MultiCPU::externalFunction( const ExternalMessage & msg)
{

	if(msg.port() == cpuin)
	{
		Proc_Inicial().push_back(msg.value());
	}

	if(msg.port() == controladorain)
	{
		Proc_Final().push_back(msg.value());
	}

	if(state() == AtomicState::passive)
	{	if(!Proc_Final().empty())
		{
			holdIn(AtomicState::active, dist_final->get());
		}
		else
		{	
			holdIn(AtomicState::active, dist_inicial->get());
		}
	}

	return (*this);
}


Model &MultiCPU::outputFunction( const CollectMessage & msg)
{
	union request req;
	int tmp;

	if(state() == AtomicState::active)
	{
		if(!Proc_Final().empty())
		{	
			req = (union request) Proc_Final().front();
			tmp = req.r.origin;
			req.r.origin = req.r.destino;
			req.r.destino = tmp;
			sendOutput(msg.time(), cpuout, req.f);
		}
		else
		{if(!Proc_Inicial().empty())
			{	
				req = (union request) Proc_Inicial().front();
				sendOutput(msg.time(), controladoraout, req.f);
			}	
		}
	}
	return (*this);
}


Model &MultiCPU::internalFunction( const InternalMessage & msg)
{


	if (!Proc_Final().empty())
	{
		Proc_Final().pop_front();

	}
	else
		if (!Proc_Inicial().empty())
		{
			Proc_Inicial().pop_front();
		}

	if(!Proc_Final().empty())
	{	
		holdIn(AtomicState::active, dist_final->get());
	}
	else
	{
		if(!Proc_Inicial().empty())
		{
			holdIn(AtomicState::active, dist_inicial->get());
		} 
		else
		{
			passivate();
		}
	}
	return *this;

}
