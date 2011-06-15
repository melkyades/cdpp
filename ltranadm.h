/*******************************************************************
*
*  DESCRIPTION: class LocalTransAdmin, SingleLocalTransAdmin, InvalidTransId
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 07/02/1999 (v2)
*
*******************************************************************/

#ifndef __LOCAL_TRANSITION_ADMIN_H
#define __LOCAL_TRANSITION_ADMIN_H

/** include files **/
#include <map>

/** my include files **/
#include "cellpos.h"       // struct NeighborPosition
#include "portlist.h"      // struct PortList
#include "real.h"	
#include "VTime.hh"
#include "model.h"

/** foward declarations **/
class NeighborhoodValue ;
class VTime ;
class SpecNode ;

/** declarations **/
typedef pair <string, Real> PortValue;

class SingleLocalTransAdmin ;

class LocalTransAdmin
{
public:
	typedef string Function;
	static const Function InvalidFn ;

	~LocalTransAdmin() ;

	list<PortValue> evaluate( const string &, const NeighborhoodValue &, PortValues *, VTime &delay , VTime &actualtime, VirtualPortList *outPorts, Model *actualCell, string portSource = "" ) ;
		// NOTA: Actual time solo es usada para el caso en que se requiera
		// 	obtener el tiempo actual de simulacion (con la funcion 'time').
		// NOTA: PortValues contendra NULL si se evaluara una funcion de transicion
		//	local sin considerar el valor de los puertos. Si es distinto a NULL
		//	entonces, se esta evaluando una funcion de transicion local para el
		//	caso en que arribo un mensage a traves de un puerto IN.
		// NOTA: outPorts es una lista de los puertos de salida de la celda que llamo a
		//	la funcion Evaluate.
		//	Solo es usada en caso de que durante la evaluacion se encuentre una
		//	referencia a la funcion SEND la cual debe enviar un valor por un puerto
		//	de la celda (usando el metodo sendOutput de la misma).
		// NOTA: actualCell es el modelo (celda atomica o flatCoupled) que se usara
		//	para llamar a sendOutput al evaluarse SEND

	LocalTransAdmin &registerTransition( const LocalTransAdmin::Function &transName, istream &transText, bool printParserInfo, bool parseForPortIn, const string &elseFunction = "" ) ;

	const Real &cellValue( const NeighborPosition &, const string PortName ) ;

	const NeighborhoodValue &neighborhood() const ;
	const Real &portValue(const string name) ;
	const VTime &actualTime() const ;
	const string &functionName() const ;
	const string &portSource() const ;
	const VirtualPortList &outputPorts() const ;
	const Model *actualModel() const;

	bool isRegistered( const Function &transName ) const ;

private:

	friend class SingleLocalTransAdmin ;

	LocalTransAdmin() ;

	typedef map< Function, SpecNode*, less< Function> > LocalTransDB ;

	LocalTransDB transDB ;

	NeighborhoodValue const *symbolTable ;
	PortValues *portTable;
	VTime ActualTime;
	CellPosition	cellpos;	// This is for show extra information in case of error
	string		functionname;	// This is for extra info too.
	string		portsource;	// This is for the command 'thisPort' of PortRefNode::evaluate
	VirtualPortList	*outPortList;	// This is for the command 'send' of SendPortNode::evaluate
	Model		*evalModel;	// This is for the command 'send' of SendPortNode::evaluate

} ; // LocalTransAdmin

class InvalidTransId : public MException
{} ; // InvalidTransId 


class SingleLocalTransAdmin
{
public:
	static LocalTransAdmin &Instance() ;

private:
	static LocalTransAdmin *instance ;

} ; // SingleLocalTransAdmin

/** inline **/
inline
	LocalTransAdmin &SingleLocalTransAdmin::Instance()
{
	if( !instance )
		instance = new LocalTransAdmin() ;

	return *instance ;
}

inline
	const NeighborhoodValue &LocalTransAdmin::neighborhood() const
{
	return *symbolTable ;
}

inline
	const VTime &LocalTransAdmin::actualTime() const
{
	return ActualTime ;
}

inline
	const string &LocalTransAdmin::functionName() const
{
	return functionname;
}

inline
	const string &LocalTransAdmin::portSource() const
{
	return portsource;
}

inline
	const VirtualPortList &LocalTransAdmin::outputPorts() const
{
	return *outPortList;
}

inline
	const Model *LocalTransAdmin::actualModel() const
{
	return evalModel;
}

#endif // __LOCAL_TRANSITION_ADMIN_H
