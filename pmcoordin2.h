/*******************************************************************
*
*  DESCRIPTION: class ParallelMasterCoordinator
*
*  AUTHOR:    Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 24/01/2001
*
*******************************************************************/

#ifndef __PMCOORDINATOR2_H
#define __PMCOORDINATOR2_H

/** include files **/
#include <list>
#include "pcoordin.h"          // class Processor
#include "pMCoordinatorState.h" // class ParallelCoordinatorState


/** forward declarations **/
class Coupled ;

/** declarations **/
class ParallelMCoordinator2: public ParallelCoordinator 
{
public:

	//Functions that are required by Warped
	void initialize();
	BasicState* allocateState();


	ParallelMCoordinator2( Coupled * ); //constructor

	//The following commented methods will not be redefined
	//ParallelProcessor &receive(  BasicOutputMessage * ) ;
	//ParallelProcessor &receive(  BasicExternalMessage * ) ;

	ParallelProcessor &receive( const InternalMessage & ) ;	
	ParallelProcessor &receive( const InitMessage & ) ;	
	ParallelProcessor &receive( const DoneMessage & ) ;
	ParallelProcessor &receive( const CollectMessage & );





protected:

	ParallelMCoordinator2(const ParallelMCoordinator2 & ) ;	// Copy constructor
	ParallelMCoordinator2& operator=(const ParallelMCoordinator2 & )  ;	 // Assignment operator
	int operator==(const ParallelMCoordinator2 & ) const ;	// Equality operator

	ParallelCoordinator& sortOutputMessage  ( const BasicOutputMessage & );
	ParallelCoordinator& sortExternalMessage( const BasicExternalMessage & );
	virtual VTime calculateNextChange(const VTime& time) const;

	//Shortcuts for state members
	ParallelCoordinatorState::DependantList &slaves() const;

};	// class ParallelMCoordinator2


/** Inline functions **/
/*******************************************************************
* Function Name: Shortcut to state variables
********************************************************************/

inline
ParallelCoordinatorState::DependantList& ParallelMCoordinator2::slaves() const{
	return *((ParallelMCoordinatorState *)state->current)->slaves;
}

#endif   //__PMCOORDINATOR2_H 
