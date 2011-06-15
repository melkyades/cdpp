/*******************************************************************
*
*  DESCRIPTION: class AtomicCell (Abstract)
*
*  AUTHOR:    Amir Barylko, Jorge Beyoglonian
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 17/09/1999 (v2)
*
*******************************************************************/

#ifndef __ATOMIC_CELL_H
#define __ATOMIC_CELL_H

#include <map>


/** include files **/
#include "portlist.h"         // for the PortList definition
#include "atomic.h"           // base class
#include "neighval.h"         // NeighborhoodValue
#include "ltranadm.h"         // LocalTransAdmin
#include "cellpos.h"          // class CellPos and NeighborPosition
#include "real.h"             // The value of the cell
#include "statevars.h"        // The state variables

/** forward declarations **/
class CoupledCell ;
class ParallelMainSimulator ;

#define	ATOMIC_CELL_NAME		"AtomicCell"

/** declarations **/
class AtomicCellState: public AtomicState
{

public:
	Real			Quantum;
	PortValues		inValues;	// record the last value that arrives in each port
	NeighborhoodValue	neighborhood;
        StateVars		variables;

	AtomicCellState(){};
	virtual ~AtomicCellState(){};

	AtomicCellState& operator=(AtomicCellState& thisState); //Assignment
	void copyState(AtomicCellState *);
	int  getSize() const;

};

class AtomicCell: public Atomic
{
public:
	static const string cellClassName;
	static const string outPort;
	static const string neighborChangePort;
	static const string NCInPrefix;
	static const string NCOutPrefix;

	~AtomicCell();

	virtual string className() const
	{ return ATOMIC_CELL_NAME; }

	const CellPosition& cellPosition() const 
	{ return cellPos; }

	AtomicCell &localFunction( const LocalTransAdmin::Function &lf )
	{ localFn = lf; return *this; }

	bool addInPort( string portName );
		// Agrega un port de entrada. Si ya existia devuelve FALSE,
		// sino devuelve TRUE.

	bool addOutPort( string portName );
		// Agrega un port de salida. Si ya existia devuelve FALSE,
		// sino devuelve TRUE.

	void createNCPorts( list<string> &portNames);
		// Crea los puertos NC.

	void getOutPorts(VirtualPortList *vpl);
		// Devuelve una lista de los puertos de salida

	const Real &value(const string &port) const
	{ return neighborhood().get(port); }

        Real &variable(const string &name)
        { return variables().get(name); }

        Real &variable(const string &name, Real &value)
        { return variables().set(name, value); }

	//Port & port( const string & );

	//Port & port( const PortId & );

protected:
	friend class CoupledCell;   	  // value
	friend class SendNCPortNode;
	friend class ParallelSCellCoordinator;
	friend class ParallelMCellCoordinator;
	friend class ParallelMainSimulator; // neighborhood( NeighborhoodValue * )
	friend class LocalTransAdmin; // evaluate
	friend class TransportDelayCell; // calculateInPort()

	AtomicCell( const CellPosition& pos, const string & = cellClassName, const LocalTransAdmin::Function &id = LocalTransAdmin::InvalidFn) ; 

	Model &outputFunction( const CollectMessage & );

	virtual AtomicCell &localTransitionConfluent( const MessageBag& );

	virtual void initializeCell();

	ModelState* allocateState() 
	{ return new AtomicCellState;}

	const LocalTransAdmin::Function &localFunction() const
	{ return localFn; }

	void setPortInFunction( const string portName, const string functionName );

	void setPortValue( const string portName, const Real portValue );

	const NeighborhoodValue &neighborhood() const
	{ return ((AtomicCellState*)getCurrentState())->neighborhood; }

	NeighborhoodValue &neighborhood()
	{ return ((AtomicCellState*)getCurrentState())->neighborhood; }

	// Puertos OUT
	const Port &outputPort(const string &portName) const;

	// Puertos NEIGHBORCHANGE
	const Port &neighborPort(const string &portName) const;
	
	PortList &inputPort()		// Lista de puertos IN (Dinamica)
	{ return in; }

	PortInFunction &inputPortFunction()
	{ return inFunction; }

	PortValues &inputPortValues()
	{ return ((AtomicCellState*)(getCurrentState()))->inValues; }

	PortList &outPortList()		// Lista de Puertos OUT
	{ return output; }		// (Dinamica)

	PortList &inNCPortList()	// Lista de Puertos IN de NC
	{ return Xports; }

	PortList &outNCPortList()	// Lista de Puertos OUT de NC
	{ return Yports; }
	
	list<string> &NCPorts()		// Lista de nombres de puertos NC
	{ return NCPortNames; }
	
	bool isInNCPort( const string portName ) const
	{ return (getNCPortByName(Xports, portName) != NULL); }

	bool isOutNCPort( const string portName ) const
	{ return (getNCPortByName(Yports, portName) != NULL); }

        StateVars &variables()
        { return (dynamic_cast<AtomicCellState*>(getCurrentState()))->variables; }

	AtomicCell &value( const Real &val, const string &port )
	{ neighborhood().set(port, val); return *this; }

	AtomicCell &setAllNCPortsValues( const Real &val );

        AtomicCell &variables( const StateVars &vars )
        { (dynamic_cast<AtomicCellState*>(getCurrentState()))->variables = vars;
          return *this; }
        
	Real QuantumValue() const;

	void QuantumValue( const Real& );

	string calculateInPort(string &portName);
	string calculateOutPort(string &portName);
	
private:
	bool addInputNCPort( string portName );
	bool addOutputNCPort( string portName );
	const Port *getNCPortByName(const PortList &pl, const string portName) const;
	
	LocalTransAdmin::Function localFn ;

	// Atomic model in and out ports
	PortList        Xports;		// In
	PortList        Yports;		// Out
	list<string>    NCPortNames;    // NC port names

	// Coupled model in and out ports
	PortList 	in;		// record the In Ports (Dynamic)
	PortInFunction  inFunction;	// record the function to use when a message come from an In Port

	PortList 	output;		// record the Out Ports (Dynamic)
	CellPosition	cellPos;
	
} ; // AtomicCell


/** inline **/
inline
Real AtomicCell::QuantumValue() const {
	return ((AtomicCellState*)getCurrentState())->Quantum;
}

inline
void AtomicCell::QuantumValue( const Real & r){
	((AtomicCellState*)getCurrentState())->Quantum = r;	
}
#endif // __ATOMIC_CELL_H

