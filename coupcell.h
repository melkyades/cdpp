/*******************************************************************
*
*  DESCRIPTION: class CoupledCell
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*  Version 3: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*	   mailto://atroccol@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 06/03/1999 (v2)
*  DATE: 12/02/2001 (v3)
*
*******************************************************************/


#ifndef __COUPLED_CELL_H
#define __COUPLED_CELL_H

#include <list>

/** include files **/
#include "coupled.h"          	// base class header
#include "cellpos.h"          	// class CellPosition
#include "ltranadm.h"         	// class LocalTransAdmin
#include "pCellCoordinatorState.h" // class ParallelCellCoordinatorState 
#include "cellpartition.h"	  	// class CellPartition
#include "parsimu.h"			// class ParallelMainSimulator
#include "statevars.h"			// class StateVars


/** foward declarations **/
class ModelAdmin ;
class ParallelMainSimulator ;


#define	COUPLED_CELL_NAME	"CoupledCell"

class CoupledCell : public Coupled
{
public:
	~CoupledCell() ;

	const CellPartition &cellPartition() const 
	{ return *partition;}

	CoupledCell &dim( unsigned int Dim, unsigned int Width)
	{ tdimension.setElement( Dim, Width ); return *this; }

	CoupledCell &dim( nTupla &nt )
	{ tdimension = nt; return *this; }

	CoupledCell &borderWrapped( bool w)
	{ wrapped = w; return *this; }

	CoupledCell &neighborhood( CellPositionList& neighborList);

	const CellPositionList& neighborhood() const
	{ return neighbors; }

	CoupledCell &inertialDelay( bool i )
	{ inertial = i; return *this; }

	CoupledCell &initialCellValue( const Real &i )
	{ initialValue = i; return *this; }

        CoupledCell &initialStateVars( const StateVars &st )
        { initialVars = st; return *this; }
        
	CoupledCell &defaultDelay( const VTime &t )
	{ delay = t; return *this; }

	CoupledCell &localTransition( const LocalTransAdmin::Function &lf )
	{ localFn = lf; return *this; }

	virtual CoupledCell &createCells( const CellPositionList &neighbors, CellPartition *part, list<string> NCPorts );

	virtual CoupledCell &setCellValue( const CellPosition &, const string &, const Real & ) ;

	CoupledCell &setCellValue( const ModelId &, const string &, Value ) ;

	CoupledCell &setCellValue( const string &sCellPos, const string &, const Real & );

	CoupledCell &setCellAllValues( const ModelId &, Value ) ;

	CoupledCell &setCellAllValues( const CellPosition &, const Real & ) ;

	CoupledCell &setCellAllValues( const string &sCellPos, const Real & );

	CoupledCell &setVariablesValue( const CellPosition &, const char * );
	
	CoupledCell &setVariablesValue( const string &, const char * );

	virtual CoupledCell &setLocalTransition( const CellPosition &, const LocalTransAdmin::Function & ) ;

	string className() const
	{return COUPLED_CELL_NAME;}

	Type type() const
	{return cell;}

	string cellName( const CellPosition &pos ) const
	{ return description() + pos.print(); }

	string cellName( const string &sCellPos ) const
	{ return description() + sCellPos; }

	unsigned int dim(unsigned int pos) const
	{ return tdimension.get(pos); }

	const nTupla &dimension() const
	{return	tdimension;}

	const LocalTransAdmin::Function &localTransition() const
	{return localFn;}

	const LocalTransAdmin::Function &localTrans() const ;

	virtual void setPortInFunction( const CellPosition &cellPos, const string &sourcePort, const string &actionName );

	virtual CoupledCell& setCellMachine( const CellPosition &cellPos, const MachineId& m, const ProcId& p);

	CellPosition inverseNeighborhood( const CellPosition& center, const CellPosition& other) const;

	bool borderWrapped() const
	{return wrapped;}

	const VTime &defaultDelay() const
	{return delay;}

	const Real &initialCellValue() const
	{return initialValue;}

	const StateVars &initialStateVars() const
        {return initialVars;}
        
	bool inertialDelay() const
	{return inertial;}

	unsigned long totalProcCount() const;

	unsigned long localProcCount() const;
	
protected:

	friend class ParallelMainSimulator ;
	friend class ParallelModelAdmin;

	CoupledCell( const string &name = "CoupledCell" ) ; // default constructor

	virtual Model &addInfluence( const string &sourceName, const string &sourcePort, const string &destName, const string &destPort) ;

	virtual ParallelProcessor &createParallelProcessor()
	{ return SingleParallelProcessorAdmin::Instance().generateProcessor(this, localProc());}

	virtual void afterProcessorInitialize();


private:

	// ** instance variables ** //
	nTupla	tdimension;
	bool wrapped, inertial;
	Real initialValue;
        StateVars initialVars;
	CellPartition *partition;
	CellPositionList neighbors;
	LocalTransAdmin::Function localFn;
	VTime delay;
} ;


/** inline **/
inline 
	CoupledCell & CoupledCell::neighborhood( CellPositionList& neighborList) 
{

	neighbors = neighborList;
	return *this;
}
#endif // __COUPLED_CELL_H
