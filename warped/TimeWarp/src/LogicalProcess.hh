//-*-c++-*-
#ifndef LOGICAL_PROCESS_HH
#define LOGICAL_PROCESS_HH
// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.

// BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY 
// FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT 
// PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, 
// EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE 
// PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME 
// THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION. 

// IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING 
// WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR 
// REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR 
// DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL 
// DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM 
// (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED 
// INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF 
// THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH HOLDER 
// OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 

//
// $Id: LogicalProcess.hh,v 1.12 1999/10/24 23:26:02 ramanan Exp $
//
//---------------------------------------------------------------------------
#include "BasicEvent.hh"
#include "FossilManagerInclude.hh"
#include "CommMgrInterface.hh"
#include "DefaultVTime.hh"
#include "config.hh"
#include "Stopwatch.hh"
#include "TimeWarp.hh"
#include "LTSFScheduler.hh"
#ifdef MESSAGE_AGGREGATION
#include "CommMgrMsgAgg.hh"
#else
#include "CommMgrDefault.hh"
#endif

#ifdef STATS
/************************************************************************/
// This turns on TimeWarp and associated statistics collection
// and reporting
#include "TimeWarpObjectStats.hh"
#include "LogicalProcessStats.hh"
#endif

extern "C" {
	long lrand48();
	void srand48(long int);
}

class BasicTimeWarp;
class ObjectRecord;
class TimeWarp;

class LogicalProcess {
	friend class BasicTimeWarp;

public:
  // Arguments are: Total number of Objects, number of objects in this LP,
  // number of LPs, and time upto which to simulate.
	LogicalProcess(int, int, int);

	~LogicalProcess();

#ifdef DISPLAY_SIM_CONFIG     
	void printConfigInfo();
#endif

	int getNumObjects();
	int getTotalNumberOfObjects() const;

	int getLPid(){
		return id;
	}


	int MAX_GVT_COUNT;
	bool simulationFinished;

	void simulate(VTime simUntil = PINFINITY );  // Simulate until time

	void calculateGVT(); //Will force the LP to calculate the GVT after the current simulation cycle.

	ObjectRecord* simArray;

#ifdef STATS
  // New statistics collection class
	logicalProcessStats lpStats;
	simulationObjectStats* simObjArray;
	void printSimulationObjectStats(ostream* statsFile);
#endif

	void allRegistered( void ); // called by application to tell the kernel 
                              // when all of the simulation objects in this
                              // LP have registered
	void registerObject( BasicTimeWarp* handle );

#ifdef STATS
	ofstream commitStatsFile ;
#endif
	ofstream outFile;
#ifdef STATEDEBUG
	ofstream outFile2;
#endif

	FOSSIL_MANAGER gVT;   // basically our GVT manager 
	int cycleCount;

#ifdef MESSAGE_AGGREGATION

	inline void  incrementAgeOfAggregatedMessage() {
		comm.incrementAgeOfMessage();
	}

	inline void  setRollingBackFlagInMessageManager() {
		comm.setRollingBackFlagInMessageManager();
	}

	inline void  resetRollingBackFlagInMessageManager() {
		comm.resetRollingBackFlagInMessageManager();
	}
	inline void flushOutputMessages() {
		comm.flush();
	}
#endif

  // Time upto which to simulate.  The definition of this variable is in
  // main.cc
	static const VTime SIMUNTIL;

private:
	bool initialized;     // flag that gets set if we've been initialized

	int numRegistered;
	int numObjects;       // number of objects on this LP
	int totalObjects;     // total number of sim objects
	int numLPs;           // the number of LPs
	int id;               // my LP id

	bool getGVT;		//Flag used to force the LP to calculate the GVT

	COMMMANAGER  comm;     // our walkie talkie to the other LPs

	LTSFScheduler scheduler;  
	Stopwatch stopWatch;

#if defined(INFREQ_POLLING)
	ifstream infreqInfile ;
	int pollMax ;
	float pollDelta ;
	int pollStatic ;
	void infreqConfigCheck();
#ifdef INFREQ_TRACE
	ofstream infreqTraceFile ;
#endif
#endif /* defined(INFREQ_POLLING) && !defined(MESSAGE_AGGREGATION) */

#ifdef MCS_TRACE
	ofstream mcsTraceFile ;
#endif

};

#endif // LOGICAL_PROCESS_HH

