/*******************************************************************
*
*  DESCRIPTION: PortList
*
*  AUTHOR:    Daniel Rodriguez.
*
*  EMAIL:     mailto://drodrigu@dc.uba.ar
*
*  DATE: 03/01/1999
*
*******************************************************************/

#ifndef __PORTLIST_H
#define __PORTLIST_H

/** include files **/
#include <map>
#include <string>
#include "real.h"
#include "port.h"
#include "cellpos.h"

/** forward declarations **/

#define	DEFAULT_FUNCTION_InPort		""

typedef map< string, string, less< string > > PortInFunction;
typedef map< string, Real, less< string > >   PortValues;
typedef map< PortId, Port *, less< PortId > > PortList;

typedef multimap< const string, const Port * > VirtualPortList;


const Port *getPort( VirtualPortList *pl, string name);
		// Devuelve el puerto identificado por el nombre  indicado.


/////////////////////////////////////////////////////////////////////////////
// THIS ARE THE STRUCTS FOR FlatCoupledCell
/////////////////////////////////////////////////////////////////////////////

class CellDescription
{
public:
	CellDescription() { }
	CellDescription( CellPosition &cp, string &ip );

	const CellPosition &cellPos() const { return cell_pos; }
	void cellPos( const CellPosition &cp ) { cell_pos = cp; }
	
	const string &inPort() const { return in_port; }
	void inPort( const string &ip ) { in_port = ip; }

private:
	CellPosition	cell_pos ;
	string		in_port ;
} ;


inline
CellDescription::CellDescription( CellPosition &cp, string &ip )
: cell_pos( cp )
, in_port( ip )
{
}

 
inline
bool less<CellDescription>::operator ()(const CellDescription &a, const CellDescription &b) const
{
	return (a.cellPos() < b.cellPos()) || (a.cellPos() == b.cellPos() && a.inPort() < b.inPort() );
}


typedef map< CellDescription, string, less< CellDescription > > FlatPortInFunction;
	// Nota: < <cellPos, inPort>, functionName> >
	
typedef map< CellDescription, Real, less< CellDescription > >   FlatPortValues;
	// Nota: < <cellPos, inPort>, lastValue> >

#endif // __PORTLIST_H
