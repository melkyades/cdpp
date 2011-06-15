/*******************************************************************
*
*  DESCRIPTION: class ParallelSlaveCoordinator
*
*  AUTHOR:    Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 24/01/2001
*
*******************************************************************/

#ifndef __PSCOORDINATOR2_H
#define __PSCOORDINATOR2_H

/** include files **/
#include <list>
#include "pcoordin.h"          // class Processor
#include "pCoordinatorState.h" // class ParallelCoordinatorState


/** forward declarations **/
class Coupled ;

/** declarations **/
class ParallelSCoordinator2: public ParallelCoordinator
{
public:

	//Functions that are required by Warped
	void initialize();

	ParallelSCoordinator2( Coupled * ); //constructor

	//The following commented methods will not be redefined.
	//BasicState* allocateState();

	//ParallelProcessor &receive( BasicExternalMessage * ) ;
	//ParallelProcessor &receive( BasicOutputMessage * ) ;

	ParallelProcessor &receive( const InternalMessage & ) ;	
	ParallelProcessor &receive( const InitMessage & ) ;
	ParallelProcessor &receive( const DoneMessage & ) ;
	ParallelProcessor &receive( const CollectMessage & );
	ParallelProcessor &receive( const OutputSyncMessage & );




protected:

	ParallelSCoordinator2(const ParallelSCoordinator2 & ) ;	// Copy constructor
	ParallelSCoordinator2& operator=(const ParallelSCoordinator2 & )  ;	 // Assignment operator
	int operator==(const ParallelSCoordinator2 & ) const ;	// Equality operator

	ParallelCoordinator& sortOutputMessage  ( const BasicOutputMessage & );
	ParallelCoordinator& sortExternalMessage( const BasicExternalMessage & );
	virtual VTime calculateNextChange(const VTime& time) const;

private:

	void sendSlaveSync();

	int slaveSyncCount;	//Number of OutSynMessages pending.
	ProcSet slaves;		//Other slave processors
	bool collectPhase;

};	// class Coordinator

#endif   //__PCOORDINATOR2_H 
