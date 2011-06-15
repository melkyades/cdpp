/*******************************************************************
*
*  DESCRIPTION: class FlatCoupledCell
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 24/04/1999 (v2)
*
*******************************************************************/

#ifndef __FLAT_CELL_COUPLED_H
#define __FLAT_CELL_COUPLED_H

/** include files **/
#include <list>
#include <map>
#include <iostream>
#include "coupcell.h"   // class CoupledCell
#include "real.h"	// The state of a cell
#include "cellpos.h"    // class CellPosition
#include "VTime.hh"       // class VTime
#include "port.h"       // class Port
#include "portlist.h"
#include "impresion.h"

/** foward declarations **/
class FlatCoordinator ;

#define	FLAT_COUPLED_CELL_NAME	"FlatCoupledCell"

/** declarations **/
class FlatCoupledCell: public CoupledCell
{
public:
	~FlatCoupledCell()
	{ delete select; delete xList; delete yList; }

	CoupledCell &createCells( const CellPositionList &neighbors, const CellPositionList &selectList ) ;

	CoupledCell &setCellValue( const CellPosition &, const Real & ) ;

	CoupledCell &setLocalTransition( const CellPosition &, const LocalTransAdmin::Function & ) ;

	void setPortInFunction( const CellPosition &cellPos, const string &sourcePort, const string &actionName );

	Model &initFunction();

	Model &externalFunction( const VTime&, const CellPosition&, bool = false, Real mtv = Real::tundef, const string &portIn = "" );

	Model &internalFunction( const VTime& );

	string className() const
	{return FLAT_COUPLED_CELL_NAME;}

protected:
	Model &addInfluence( const string &sourceName, const string &sourcePort, const string &destName, const string &destPort) ;

	void setLastValuePortIn( const CellPosition &cellPos, const string &portIn, const Real &value);

private:

	friend class ParallelModelAdmin;
	friend class ParallelMainSimulator ;
	friend class FlatCoordinator ;

	// ** Types ** //
	struct NextEvent
	{
		VTime time ;
		CellPosition pos ;
		Real value ;
	} ; // NextEvent

	friend ostream &operator <<( ostream &os, NextEvent &next ) ;

	typedef list< NextEvent > NextEventList ;
	typedef map< CellPosition, LocalTransAdmin::Function > FunctionZones ;

	struct ElementList
	{
		CellPosition	cellPosition;
		string		port;
	}; // ElementList

	// ** Instance variables ** //
	NextEventList eventList ;
	FunctionZones funcZones ;
	CellPositionList neighbors ;

	long *select ;

	VirtualPortList *xList;
	VirtualPortList *yList;

	FlatPortValues		portValues;
	FlatPortInFunction	portFunction;

	// ** Methods ** //
	FlatPortValues &getPortValues()
	{return portValues;}

	FlatPortInFunction &getPortFunction()
	{return portFunction;}

	const string &getPortInFunction( const CellPosition &cellPos, const string &sourcePort );

	void getInputPortValues( PortValues *pv, const CellPosition &cellPos, const string &portIn );

	void getOutputPorts( VirtualPortList **vpl, const CellPosition &cellPos );

	FlatCoupledCell( const string &name )
		: CoupledCell( name )
	{}

	VirtualPortList *externalList()
	{return xList;}

	NextEventList &nextEventList()
	{return eventList;}

	FlatCoupledCell &insertByTime( const VTime &t, const CellPosition &pos, const Real &v ) ;

} ; // FlatCoupledCell

/** inline **/
inline
	ostream &operator <<( ostream &os, FlatCoupledCell::NextEvent &next )
{
	os << next.time.asString() << " |" << next.pos << "| " << next.value.asString(Impresion::Default.Width(), Impresion::Default.Precision() ) ;
	return os ;
}

#endif // __FLAT_CELL_COUPLED_H
