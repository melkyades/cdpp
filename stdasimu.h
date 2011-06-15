/*******************************************************************
*
*  DESCRIPTION: class StandAloneMainSimulator
*
*  AUTHOR:    Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 30/8/2000
*
*******************************************************************/

#ifndef __STANDALONESIMULATOR_H
#define __STANDALONESIMULATOR_H

/** include files **/
#include <iostream.h>
#include "root.h"          // EventList 
#include "ini.h"           // class Ini
#include "loader.h"        // class SimLoader 
#include "ltranadm.h"      // class LocalTransAdmin
#include "parsimu.h"	   // class ParallelMainSimulator



/** declarations **/
class StandAloneMainSimulator : public ParallelMainSimulator
{
public:
	ParallelMainSimulator &run() ;
	
protected:
	friend class Coupled ;
	friend class Atomic ;
	
	// ** Methods ** //
	ParallelMainSimulator &loadModels( istream&, bool printParserInfo ) ;
	ParallelMainSimulator &loadExternalEvents( istream& ) ;
//	ParallelMainSimulator &log( ostream & ) ;
//	ParallelMainSimulator &output( ostream & ) ;
	ParallelMainSimulator &loadModel( Coupled &, Ini &, bool ) ;
	ParallelMainSimulator &loadPorts( Coupled &, Ini & ) ;
	ParallelMainSimulator &loadComponents( Coupled &, Ini &, bool ) ;
	ParallelMainSimulator &loadLinks( Coupled &, Ini & ) ;
	ParallelMainSimulator &loadCells( CoupledCell &, Ini &, bool ) ;
	ParallelMainSimulator &loadInitialCellValues( CoupledCell &, Ini & ) ;
	ParallelMainSimulator &loadInitialCellValuesFromFile( CoupledCell &parent, const string &fileName );
	ParallelMainSimulator &loadInitialCellValuesFromMapFile( CoupledCell &parent, const string &fileName );
	ParallelMainSimulator &loadDefaultTransitions( CoupledCell &, Ini &, bool ) ;
	ParallelMainSimulator &loadPortInTransitions( CoupledCell &, Ini &, bool ) ;
	ParallelMainSimulator &loadLocalZones( CoupledCell &, Ini &, bool ) ;
	
	ParallelMainSimulator &registerTransition(const LocalTransAdmin::Function &, Ini &, bool );
	ParallelMainSimulator &registerTransitionPortIn(const LocalTransAdmin::Function &, Ini &, bool, const string & );
	
	void showEvents( const EventList &events, ostream &out = cout ) ;
	
	
};	// class StandAloneMainSimulator

#endif   //__StandAloneMainSimulator_H
