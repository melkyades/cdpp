/*******************************************************************
*
*  DESCRIPTION: class FlatCoupledCell
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian
*  Version 2: Daniel Rodriguez.
*  Version 3: Gabriel Wainer
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*         mailto://gabrielw@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 12/11/1999 (v2)
*
*******************************************************************/

// ** include files **//
#include <algorithm>
#include "flatcoup.h"   // header
#include "cellstate.h"  // CellState
#include "neighval.h"   // NeighborhoodValue
#include "strutil.h"	// lowerCase
#include "realfunc.h"   // calculateWithQuantum

using namespace std;

// ** private data ** //             
#define	OUT_PORT	"out"

// ** public ** //
inline long *selectPos( long *select, nTupla *dim, const CellPosition &pos, bool verifyIncludes )
{
        return &(select[ pos.calculateIndex( *dim, verifyIncludes ) ]);
}

/*******************************************************************
* Method: createCells
********************************************************************/
CoupledCell &FlatCoupledCell::createCells( const CellPositionList &n, const CellPositionList &selectList )
{
	neighbors = n;

	cellState( new CellState( *((nTupla *) &(dimension())), borderWrapped() ) );

	register long cantElems = dimension().totalElements();
	select = new long[ cantElems ];
	xList = new VirtualPortList[ cantElems ];
	yList = new VirtualPortList[ cantElems ];

	nTupla	dimen( dimension() );
	register long posi( 0 );
	// let's fill the order
	for ( CellPositionList::const_iterator cursor = selectList.begin(); cursor != selectList.end(); cursor++ )
		*(selectPos( select, &dimen, *cursor, true )) = posi++;

	///////////////////////
	// Creates the cells
	///////////////////////
	MASSERTMSG( !dimen.contains(0), "Attempt to create a FlatCoupledCell model with a dimension containing the value 0.");

	CellPosition	counter( dimen.dimension(), 0 );
	register bool	overflow = false;

	while (!overflow){
		(*cellState())[ counter ] = initialCellValue();

		if( find( selectList.begin(), selectList.end(), counter ) == selectList.end())
			*(selectPos( select, &dimen, counter, false )) = posi++;

		overflow = counter.next( dimen );
	}

	MASSERTMSG( posi == cantElems, "Invalid cell creation!" );
	return *this ;
}

/*******************************************************************
* Method: setCellValue
********************************************************************/
CoupledCell &FlatCoupledCell::setCellValue( const CellPosition &pos, const Real &val )
{
	CellPosition *cp = (CellPosition *) &pos;
	(*cellState())[ *cp ] = val ;
	return *this ;
}

/*******************************************************************
* Method: localTransition
********************************************************************/
CoupledCell &FlatCoupledCell::setLocalTransition( const CellPosition &pos, const LocalTransAdmin::Function &fn )
{
	funcZones[ pos ] = fn;
	return *this ;
}

/*******************************************************************
* Method: initFunction
********************************************************************/
Model &FlatCoupledCell::initFunction()
{
	typedef map< long, CellPosition > SelectList ;
	SelectList posList ;

	nTupla	dimen( dimension() );
	MASSERTMSG( !dimen.contains(0), "Attempt to initialize a FlatCoupledCell model with a dimension containing the value 0.");

	register bool	overflow = false;
	CellPosition	counter( dimen.dimension(), 0 );

	while (!overflow){
		posList[ *(selectPos( select, &dimen, counter, false )) ] = counter;
		overflow = counter.next( dimen );
	}

	for( SelectList::iterator cursor = posList.begin(); cursor != posList.end(); cursor ++ )
		externalFunction( VTime::Zero, cursor->second );

	return *this;
}

/*******************************************************************
* Method: internalFunction
*******************************************************************/
Model &FlatCoupledCell::internalFunction( const VTime &time )
{
	typedef map< long, CellPosition > Influenced;
	Influenced influenced;
	nTupla	dimen( dimension() );
	VirtualPortList::iterator cursor;
	CellPositionList::iterator links;
	CellPosition aux;
	register long posCell;

	while( !eventList.empty() && eventList.front().time == time )
	{
		NextEvent &nextEvent( eventList.front() ); 

		(*cellState())[ nextEvent.pos ] = nextEvent.value;

		/*************************************************************/
		// let's analize the output 
		// Find the "OUT" port
		posCell = nextEvent.pos.calculateIndex( dimension() );
		for (cursor = yList[posCell].begin(); cursor != yList[posCell].end(); cursor++)
			if (cursor->first == OUT_PORT)
				sendOutput( time, *(cursor->second), nextEvent.value.value() );
		/*************************************************************/

		// let's save the external messages to the local links
		for (links = neighbors.begin(); links != neighbors.end(); links++ )
		{
			aux = nextEvent.pos;
			aux -= *links;		// REVERSE NEIGHBOR LIST
						// For NORMAL NEIGHBOR LIST do +=

			if( cellState()->includes( aux ) )
			{
				cellState()->calcRealPos( aux );

				// inserting ordered by select
				influenced[ *(selectPos( select, &dimen, aux, false )) ] = aux;
			}
		}
		eventList.pop_front();
	}

	// let's send the external messages
	for (Influenced::iterator cursor = influenced.begin(); cursor != influenced.end(); cursor++ )
		externalFunction( time, cursor->second );

	return *this;
}

/*******************************************************************
* Function Name: addInfluence
* Description: agrega una influencia entre dos ports
********************************************************************/
Model &FlatCoupledCell::addInfluence( const string &sourceName, const string &sourcePort, const string &destName, const string &destPort ) 
{
	MASSERTMSG( sourceName==description() || destName==description(), "Invalid Link!" );

	if( sourceName==description() )
	{
		Port &port( port( sourcePort ) );

		xList[ CellPosition(destName).calculateIndex( dimension() )].insert( VirtualPortList::value_type( lowerCase(destPort), &port ) );

		// Now adds the default function in the port function list
		setPortInFunction( CellPosition(destName), destPort, DEFAULT_FUNCTION_InPort );

		// Now set the last value arrived to the port with a undefined
		setLastValuePortIn( CellPosition(destName), destPort, Real::tundef );
	} else
	{
		/************************************************************/
		// Now, adds the output ports to the YList

		Port &port( port( destPort ) );

		yList[ CellPosition(sourceName).calculateIndex( dimension() )].insert( VirtualPortList::value_type( lowerCase(sourcePort), &port ) );
		/************************************************************/
	}
	return *this ;
}

/*******************************************************************
* Function Name: setPortInFunction
* Description: agrega o establece una function para un puerto especifico de una celda
********************************************************************/
void FlatCoupledCell::setPortInFunction( const CellPosition &cellPos, const string &portIn, const string &functionName ) 
{
	FlatPortInFunction::iterator	cursor = getPortFunction().begin();
	register bool	salgo = false;

	// search for the (cellPos, portIn)
	while ( !salgo && cursor != getPortFunction().end() )
		if ( cursor->first.cellPos() == cellPos  &&  cursor->first.inPort() == portIn )
			salgo = true;
		else
			cursor++;

	// if not exists
	if (cursor == getPortFunction().end() )
	{
		CellDescription	*cellDesc = new CellDescription;
		cellDesc->cellPos( cellPos );
		cellDesc->inPort( portIn );

		getPortFunction()[ *cellDesc ] = functionName;
	}
	else	// the entry exists
	{
		CellDescription	cellDesc;
		cellDesc.cellPos( cellPos );
		cellDesc.inPort( portIn );

		getPortFunction()[ cellDesc ] = functionName;
	}
}

/*******************************************************************
* Function Name: getPortInFunction
* Description: obtiene el nombre de la function para un puerto especifico de una celda
********************************************************************/
const string &FlatCoupledCell::getPortInFunction( const CellPosition &cellPos, const string &portIn ) 
{
	register bool	salgo = false;

	// search for the (cellPos(), portIn)
	FlatPortInFunction::iterator	cursor = getPortFunction().begin();
	while (!salgo && cursor != getPortFunction().end())
		if ( cursor->first.cellPos() == cellPos  &&  cursor->first.inPort() == portIn )
			salgo = true;
		else
			cursor++;

	MASSERTMSG( cursor != getPortFunction().end(), "Invalid reference to the PortIn function: " + portIn);

	CellDescription	cellDesc;
	cellDesc.cellPos( cellPos );
	cellDesc.inPort( portIn );

	return getPortFunction()[ cellDesc ];
}

/*******************************************************************
* Function Name: setLastValuePortIn
* Description: agrega o establece el ultimo valor ingresado por un puerto especifico de una celda
********************************************************************/
void FlatCoupledCell::setLastValuePortIn( const CellPosition &cellPos, const string &portIn, const Real &value )
{
	FlatPortValues::iterator	cursor = getPortValues().begin();
	register bool	salgo = false;

	// search for the (cellPos, portIn)
	while (!salgo && cursor != getPortValues().end())
		if ( cursor->first.cellPos() == cellPos  &&  cursor->first.inPort() == portIn )
			salgo = true;
		else
			cursor++;

	// if not exists
	if (cursor == getPortValues().end() )
	{
		CellDescription	*cellDesc = new CellDescription;
		cellDesc->cellPos( cellPos );
		cellDesc->inPort( portIn );

		portValues[ *cellDesc ] = value;
	}
	else	// the entry exists
	{
		CellDescription	cellDesc;
		cellDesc.cellPos( cellPos );
		cellDesc.inPort( portIn );

		portValues[ cellDesc ] = value;
	}
}

/*******************************************************************
* Function Name: getInputPortValues
* Description: obtiene a partir de la lista de valores de los ports
*		de todas las celdas, una lista con los valores de los
*		ports de una celda especifica
********************************************************************/
void FlatCoupledCell::getInputPortValues( PortValues *pv, const CellPosition &cellPos, const string &portIn )
{
	// search for the cellPos
	for (FlatPortValues::const_iterator cursor = getPortValues().begin(); cursor != getPortValues().end(); cursor++)
		if ( cursor->first.cellPos() == cellPos )
			(*pv)[ cursor->first.inPort() ] = cursor->second;
}

/*******************************************************************
* Function Name: getOutputPorts
* Description: obtiene una lista de los puertos de salida de la celda
*		en cuestion
********************************************************************/
void FlatCoupledCell::getOutputPorts( VirtualPortList **vpl, const CellPosition &cellPos )
{
	*vpl = &(yList[ cellPos.calculateIndex( dimension() ) ]);

//	register long posCell = cellPos.calculateIndex( dimension() );
//	for (VirtualPortList::const_iterator cursor = yList[posCell].begin(); cursor != yList[posCell].end(); cursor++)
//		vpl->insert( VirtualPortList::value_type( cursor->first, cursor->second ) );
}

/*******************************************************************
* Method: externalFunction
********************************************************************/
Model &FlatCoupledCell::externalFunction( const VTime &time, const CellPosition &pos, bool external, Real mtv, const string &portIn )
{
	VTime delay( defaultDelay() );
	VTime actualTime( time );
	Real tv( mtv );

	if( !external )
	{
		LocalTransAdmin::Function idFn( localTransition() );

		// Busco
		FunctionZones::iterator cursor = funcZones.begin();
		while ( cursor->first != pos && cursor != funcZones.end() )
			cursor++;

		if( cursor != funcZones.end() )
			idFn = cursor->second;

		NeighborhoodValue *nVal = new NeighborhoodValue(*cellState(), neighbors, pos);

		VirtualPortList *vpl;

		getOutputPorts( &vpl, pos );
		tv = SingleLocalTransAdmin::Instance().evaluate(idFn, *nVal, NULL, delay, actualTime, vpl, this);

		delete nVal;
	}
	else 	// the message comes from an IN Port
	{
		string functionName = getPortInFunction(pos, portIn);

		// first we set the new port value in the list of FlatPortValues
		setLastValuePortIn( pos, portIn, mtv );

		if (functionName != DEFAULT_FUNCTION_InPort)
		{
			// Obtengo el vecindario
			NeighborhoodValue *nVal = new NeighborhoodValue(*cellState(), neighbors, pos);

			// Obtengo los valores de todos los puertos de entrada de la celda en cuestion
			PortValues	*portInValues = new PortValues;
			getInputPortValues( portInValues, pos, portIn );

			VirtualPortList *vpl;
			getOutputPorts( &vpl, pos );
			tv = SingleLocalTransAdmin::Instance().evaluate( functionName, *nVal, portInValues, delay, actualTime, vpl, this, portIn );

			delete portInValues;
			delete nVal;
		}
	}
        /////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////
        //if (UseQuantum().Active())
        //        tv = valueWithQuantum(tv, Real(UseQuantum().Value()));
        /////////////////////////////////////////////////////////////////////
 
	NextEventList::iterator lastFound( nextEventList().end() );
	nTupla	dimen( dimension() );

	// Replaced all time + delay with nextTime
	VTime	nextTime( time + delay );

	NextEventList::iterator cursor = nextEventList().begin();
	for (; ( cursor->time < nextTime || ( cursor->time == nextTime && *(selectPos(select, &dimen, cursor->pos, false)) < *(selectPos(select, &dimen, pos, false)) ) ) && cursor != nextEventList().end() ; cursor++ )
		if (cursor->pos == pos)
			lastFound = cursor;

	CellPosition *cp = (CellPosition *) &pos;
	Real ltv( lastFound == this->nextEventList().end() ? (*cellState())[ *cp ] : lastFound->value );


        if(  (  UseQuantum().Active()  &&
                (   valueWithQuantum( tv, Real(UseQuantum().Value()) ) !=
                    valueWithQuantum( ltv, Real(UseQuantum().Value()) )
                 )
              )
            || (!UseQuantum().Active() && ltv != tv )   )

//        if( tv != ltv )
	{
		NextEvent next;
		next.time = nextTime;
		next.pos = pos;
		next.value = tv;
		cursor = nextEventList().insert( cursor, next );
		cursor++;

		if (inertialDelay() && lastFound != nextEventList().end())
			nextEventList().erase( lastFound );

		// ESTO ESTA MAL. POR ESO FUE COMENTADO
		//else if (!inertialDelay())	// DEMORA DE TRANSPORTE
		//	for ( ; cursor != nextEventList().end(); cursor++)
		//		if (cursor->pos == pos)
		//			cursor = nextEventList().erase(cursor);
	}
	return *this;
}
