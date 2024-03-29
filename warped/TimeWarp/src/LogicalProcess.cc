//-*-c++-*-
#ifndef LOGICAL_PROCESS_CC
#define LOGICAL_PROCESS_CC
// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.
//
// BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY 
// FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT 
// PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, 
// EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE 
// PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME 
// THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION. 
//
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
//
// $Id: LogicalProcess.cc,v 1.26 1999/10/24 23:26:01 ramanan Exp $
//
//---------------------------------------------------------------------------
#include "LogicalProcess.hh"
#include "SimulationTime.hh"
#include <math.h>
#include <iomanip>
#ifdef STATS
#include <stdio.h>
#endif

class CommPhyInterface;
extern CommPhyInterface *commLib;

LogicalProcess::LogicalProcess(int totalNum, int myNum, int lpNum)
#ifdef STATS
: lpStats(myNum), comm(lpNum, commLib), scheduler(myNum) {
#else
: comm(lpNum, commLib), scheduler(myNum) {
#endif 
	int i;
	totalObjects = totalNum;
	numObjects = myNum;
	numLPs = lpNum;
	simulationFinished = false;
	initialized = false;
	numRegistered = 0;
	cycleCount = 0;
	getGVT = false;
	simArray    = new ObjectRecord[totalObjects];
#ifdef STATS
	simObjArray = new simulationObjectStats[numObjects];
#endif

	BasicTimeWarp::inputQ.initialize(numObjects);

    // tell the commManager to initialize itself
	comm.initCommManager(simArray, &gVT, numLPs, this);  
	id = comm.getID();
	ifstream gvtPeriod;
	gvtPeriod.open("GVTPeriod");
	if (!gvtPeriod.good()) {
		MAX_GVT_COUNT = 10000;
		gVT.gvtPeriod = MAX_GVT_COUNT;
	}
	else {
		gvtPeriod >> MAX_GVT_COUNT;
		gvtPeriod >> gVT.noOfGVTToSkip;
		gVT.gvtPeriod = MAX_GVT_COUNT;
		gvtPeriod.close();
	}
	cout << "Using a period of " << MAX_GVT_COUNT << " to calculate GVT\n";
#ifdef STATS
	if(FILE_OUTPUT){
		char filename[20];
		sprintf(filename,"LPSTATS%d",id);
		commitStatsFile.open(filename);
		if( !commitStatsFile.good()){
			cerr << "LPSTATS " << id << " couldn't open logging file!" << endl;
			exit(-1);
		}
	}
	gVT.setStatFile(&commitStatsFile);
#endif

#if defined(LPDEBUG) || defined(OPENLPDEBUGFILE)
	char filenames[20];
	sprintf(filenames,"LP%d",id);
	outFile.open(filenames);
	if(!outFile.good()){
		cerr << "LP " << id << " couldn't open logging file!" << endl;
		exit(-1);
	}
	else {
		gVT.setFile( &outFile );
		comm.setFile( &outFile );

#ifdef LPDEBUG
		scheduler.setFile( &outFile );
#endif
	}
#ifdef MESSAGE_AGGREGATION
	gFileHandle = &outFile;
#endif
#endif

#ifdef STATEDEBUG
	char statedebugFilename[20];
	sprintf(statedebugFilename,"LPSTATE%d",id);
	outFile2.open(statedebugFilename);
	if(!outFile2.good()){
		cerr << "LP " << id << " couldn't open logging file!" << endl;
		exit(-1);
	}
#endif
	for (i = 0; i < totalObjects; i++) {
		simArray[i].ptr = NULL;
		simArray[i].lpNum = MAXINT;
	}

#if  !defined(MPI) && !defined(TCPLIB) && !defined(FWNS)
	if( numLPs > 1 && id == 0 ){
		cerr << "Error! numLPs = " << numLPs 
			<< " but no message passing support  compiled in!\n";
	}
#endif

#ifdef DISPLAY_SIM_CONFIG   
	printConfigInfo();
#endif

#if defined(INFREQ_POLLING)
	infreqInfile.open("infreqPolling.config");
	if(!infreqInfile.good()) {
		cerr << "\n\nCould not open infreqPolling.config file !\n\n";
		abort();
	}
	infreqConfigCheck();
#ifdef INFREQ_TRACE
	char infreqTemp[30];
	sprintf(infreqTemp,"./infreqTraceLP%d",id);
	infreqTraceFile.open(infreqTemp);
	if(!infreqTraceFile.good()){
		cerr << "\n\nCould not open output file to write trace to \n\n" ;
		abort();
	}
#endif
#endif

#ifdef MCS_TRACE
	char infreqTempStats[30];
	sprintf(infreqTempStats,"./MCS_TraceLP%d",id);
	mcsTraceFile.open(infreqTempStats);
	if(!mcsTraceFile.good()){
		cerr << "\n\nCould not open output file to write MCS stats to \n\n" ;
		abort();
	}
#endif

}

LogicalProcess::~LogicalProcess() {
#ifdef STATS
	if(FILE_OUTPUT){
		printSimulationObjectStats(&commitStatsFile);
	} 
	else {
		printSimulationObjectStats(&cout);
	}

	delete [] simObjArray;
	if(FILE_OUTPUT){
		commitStatsFile.close();
	}
#endif

	delete [] simArray;

#ifdef LPDEBUG
	outFile.close();
#endif

#if defined(INFREQ_POLLING)
	infreqInfile.close();
#ifdef INFREQ_TRACE
	infreqTraceFile.close();
#endif
#endif

#ifdef MCS_TRACE
	mcsTraceFile.close();
#endif
}



#if defined(INFREQ_POLLING)

void
LogicalProcess::infreqConfigCheck(){

#if defined(INFREQ_FXD_INCREMENT) || defined(INFREQ_VAR_INCREMENT)
	infreqInfile >> pollMax >> pollDelta ;
#elif defined(INFREQ_STATIC)
	infreqInfile >> pollStatic ;
#else
  // user did not specify anything ; let default be INFREQ_VAR_INCREMENT
	infreqInfile >> pollMax >> pollDelta ;
#endif

	cout <<"\n\nUsing INFREQUENT POLLING of the message communication subsystem\n";

#ifdef INFREQ_FXD_INCREMENT
	cout << "DYNAMIC Strategy with FIXED increment of "
		<< pollDelta
		<< "\nMAX polling frequency is "
		<< pollMax << endl << endl ;
#elif defined(INFREQ_STATIC)
	cout << "STATIC Strategy with polling frequency of "
		<< pollStatic << endl << endl ;
#else
	cout << "DYNAMIC Strategy with VARIABLE increment of "
		<< pollDelta << " %"
		<< "\nMAX polling frequency is "
		<< pollMax << endl << endl ;

	pollDelta *= .01 ;

#endif


};

#endif


#ifdef STATS
void
LogicalProcess::printSimulationObjectStats(ostream* statsFile){

	*statsFile << "**** Time Warp Simulation Object Statistics *****" 
		<< endl;
	*statsFile << "--------------------------------------------------------------------------------" << endl;
	*statsFile << "            Events\n";
	*statsFile << "LP#  Obj#   Undone    Sent+ve   Sent-ve   Recv+ve   Recv-ve   Strag+ve   ProcMsg";
#ifdef ONE_ANTI_MESSAGE
	*statsFile << " MsgSupp" << endl;
#else
	*statsFile << endl;
#endif
	*statsFile << "--------------------------------------------------------------------------------" << endl;

	int i;
	int totalPosStragglers = 0;
	int totalEventsUndone = 0;
	int totalPosEventsSent = 0;
	int totalPosEventsRecv = 0;
	int totalNegEventsSent = 0;
	int totalNegEventsRecv = 0;
	int totalprocessedEvents  = 0;

	for(i=0; i < numObjects; i++){

		int numPosStragglers = simObjArray[i].getNumberOfPosStragglers();
		int eventsUndone     = simObjArray[i].getEventsUndone();
		int numPosEventsSent = simObjArray[i].getPosSent();
		int numPosEventsRecv = simObjArray[i].getPosRecv();
		int numNegEventsSent = simObjArray[i].getNegSent();
		int numNegEventsRecv = simObjArray[i].getNegRecv();
		int processedEvents  = simObjArray[i].getEventsProcessed();

		totalPosStragglers += numPosStragglers;
		totalEventsUndone += eventsUndone;
		totalPosEventsSent += numPosEventsSent;
		totalPosEventsRecv += numPosEventsRecv;
		totalNegEventsSent += numNegEventsSent;
		totalNegEventsRecv += numNegEventsRecv;
		totalprocessedEvents  += processedEvents;

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(2) << id << "] " ;

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(3) << lpStats.eventIds[i] << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(7) << eventsUndone << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(7) << numPosEventsSent << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(7) << numNegEventsSent << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(7) << numPosEventsRecv << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(7) << numNegEventsRecv << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(7) << numPosStragglers << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(7) << processedEvents <<  "] ";

#ifdef ONE_ANTI_MESSAGE
		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(2) << simObjArray[i].getLocalEventsSupp()
			<< "," << setw(2) << simObjArray[i].getRemoteEventsSupp()
			<< "]" << endl;
#else
		*statsFile << endl;
#endif    


	}

	*statsFile << "--------------------------------------------------------------------------------\n";
	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(2) << id << "] " ;

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(3) << "###] ";

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(7) << totalEventsUndone << "] ";

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(7) << totalPosEventsSent << "] ";

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(7) << totalNegEventsSent << "] ";

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(7) << totalPosEventsRecv << "] ";

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(7) << totalNegEventsRecv << "] ";

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(7) << totalPosStragglers << "] ";

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(7) << totalprocessedEvents <<  "] " << endl;



	*statsFile << "--------------------------------------------------------------------------------\n";
	*statsFile << "                  Avg_RBD Avg_RBD  MaxRBD  MaxRBD  Max Sz Avg Sz  StateQ  \n";
	*statsFile << "LP # Obj# Rollbks (Time)  (State)  (Time)  (State) StateQ StateQ  Sampls\n";
	*statsFile << "--------------------------------------------------------------------------------\n";

	VTime totalAvgRollbackDistanceTime = 0;
	int totalAvgRollbackDistanceStates = 0;
	VTime totalMaxRollbackDistanceTime = simObjArray[0].getMaxRollbackDistanceTime();
	int totalMaxRollbackDistanceStates = simObjArray[0].getMaxRollbackDistanceStates();
	int totalRollbacks = 0;
	int totalMaxStateQSize = simObjArray[0].getMaximumStateQSize();
	int totalAvgStateQSize = 0;
	int totalStateQSizeCalls = 0;

	for(i=0; i < numObjects; i++){
		VTime avgRollbackDistanceTime = 0;
		int avgRollbackDistanceStates = 0;

		VTime maxRollbackDistanceTime = simObjArray[i].getMaxRollbackDistanceTime();
		int maxRollbackDistanceStates = simObjArray[i].getMaxRollbackDistanceStates();
		int numRollbacks     = simObjArray[i].getRollbackCount();
		int maxStateQSize    = simObjArray[i].getMaximumStateQSize();
		int avgStateQSize    = simObjArray[i].getAvgStateQSize();
		int stateQSizeCalls  = simObjArray[i].getNumStateQSizeCalls();

		VTime totalRollbackDistanceTime = simObjArray[i].getRollbackDistanceTime();
		int totalRollbackDistanceStates = simObjArray[i].getRollbackDistanceStates();



		if(totalMaxRollbackDistanceTime < maxRollbackDistanceTime) {
			totalMaxRollbackDistanceTime = maxRollbackDistanceTime;
		}

		if(totalMaxRollbackDistanceStates < maxRollbackDistanceStates){ 
			totalMaxRollbackDistanceStates = maxRollbackDistanceStates;
		}

		totalRollbacks += numRollbacks;

		if (totalMaxStateQSize < maxStateQSize) {
			totalMaxStateQSize = maxStateQSize;
		}

		totalAvgStateQSize += avgStateQSize;
		totalStateQSizeCalls += stateQSizeCalls;

		if(numRollbacks != 0){
			avgRollbackDistanceTime = (totalRollbackDistanceTime/numRollbacks);
			avgRollbackDistanceStates = (totalRollbackDistanceStates/numRollbacks);
			totalAvgRollbackDistanceTime = totalAvgRollbackDistanceTime + avgRollbackDistanceTime;
			totalAvgRollbackDistanceStates += avgRollbackDistanceStates;
		}

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(2) << id << "] " ;

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(2) << lpStats.eventIds[i] << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(5) << numRollbacks << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(5) << avgRollbackDistanceTime << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(5) << avgRollbackDistanceStates << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(5) << maxRollbackDistanceTime << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(5) << maxRollbackDistanceStates << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(5) << maxStateQSize << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(5) << avgStateQSize << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(5) << stateQSizeCalls << "] ";

		*statsFile << endl;
	}

	*statsFile << "------------------------------------------------------------------------------\n";
	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(2) << id << "] " ;

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(2) << "##] ";

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(5) << totalRollbacks << "] ";

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(5) << totalAvgRollbackDistanceTime/numObjects << "] ";

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(5) << totalAvgRollbackDistanceStates/numObjects << "] ";

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(5) << totalMaxRollbackDistanceTime << "] ";

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(5) << totalMaxRollbackDistanceStates << "] ";

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(5) << totalMaxStateQSize << "] ";

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(5) << totalAvgStateQSize/numObjects << "] ";

	statsFile->setf(ios::right,ios::adjustfield);
	*statsFile << "[" << setw(5) << totalStateQSizeCalls << "] ";

	*statsFile << endl;

	*statsFile << "------------------------------------------------------------------------------\n";

	*statsFile << "------------------------------------------------------------------------------\n";
	*statsFile << "           Min Sz  Avg Sz  Max Sz   Min Sz  Avg Sz  Max Sz  StateQ  \n";
	*statsFile << "LP # Obj#  StateQ  StateQ  StateQ   StateQ  StateQ  StateQ  Sampls                                                  \n";
	*statsFile << "           State   State   State    Time    Time    Time \n";
	*statsFile << "------------------------------------------------------------------------------\n";
	for(i=0; i < numObjects; i++){
		int minStateQSize    = simObjArray[i].getMinimumStateQSize();
		int maxStateQSize    = simObjArray[i].getMaximumStateQSize();
		int avgStateQSize    = simObjArray[i].getAvgStateQSize();
		VTime minStateQSizeTime    = simObjArray[i].getMinimumStateQTimeDiff();
		VTime maxStateQSizeTime    = simObjArray[i].getMaximumStateQTimeDiff();
		VTime avgStateQSizeTime    = simObjArray[i].getAvgStateQTimeDiff();
		int stateQSizeCalls  = simObjArray[i].getNumStateQSizeCalls();

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(2) << id << "] " ;

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(2) << lpStats.eventIds[i] << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(5) << minStateQSize << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(5) << avgStateQSize << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(5) << maxStateQSize << "]  ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(5) << minStateQSizeTime << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(5) << avgStateQSizeTime << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(5) << maxStateQSizeTime << "] ";

		statsFile->setf(ios::right,ios::adjustfield);
		*statsFile << "[" << setw(5) << stateQSizeCalls << "] ";

		*statsFile << endl;
	}

	*statsFile << "-----------------------------------------------" << endl;
	*statsFile << "**** Time Warp Logical Process Statistics *****" << endl;
	*statsFile << "-----------------------------------------------" << endl;
	*statsFile << "Total Execution time                          : " 
		<< lpStats.getExecutionTime() << " seconds" << endl;
	*statsFile << "Total number of processed Messages            : " 
		<< lpStats.getTotalEventsProcessed() << endl;
	*statsFile << "Total number of committed Messages            : " 
		<< lpStats.getTotalEventsCommitted() << endl;
	*statsFile << "Total number of Messages Undone               : " 
		<< lpStats.getTotalEventsUndone() << endl;
	*statsFile << "Period used to calculate GVT    : " << MAX_GVT_COUNT << endl;
	*statsFile << "Number of simulation cycles     : " << cycleCount << endl;

	*statsFile << "-----------------------------------------------" << endl;
	*statsFile << "      Message Aggregation Statistics           " << endl;
	*statsFile << "-----------------------------------------------" << endl;
	*statsFile << "Total Number of Application Messages Sent     : " 
		<< lpStats.getNumberOfApplicationMessagesSent() << endl;
	*statsFile << "Total Number of Physical Messages Sent        : " 
		<< lpStats.getNumberOfPhysicalMessagesSent() << endl;

	*statsFile << "Total Number of Application Messages Received : " 
		<< lpStats.getNumberOfApplicationMessagesRecv() << endl;
	*statsFile << "Total Number of Physical Messages Received    : " 
		<< lpStats.getNumberOfPhysicalMessagesRecv() << endl;
#ifdef MESSAGE_AGGREGATION
	*statsFile << "-----------------------------------------------" << endl;
	*statsFile << "          Printing Aggregate Sizes for LP " << id << endl;
	*statsFile << "-----------------------------------------------" << endl;
	for(i = 0; i < MAX_AGGREGATE_SIZE; i++){
		if(lpStats.statsArray[i] != 0) {
			*statsFile << i << "  " << lpStats.statsArray[i] << endl;
		}
	}

	*statsFile << "-----------------------------------------------" << endl;
	*statsFile << "          Printing Trigger Frequency for LP " << id << endl;
	*statsFile << "-----------------------------------------------" << endl;
	*statsFile << "# of Triggers due to Window Size Exhaustion      :" << logicalProcessStats::window_size_exhausted << endl;
	*statsFile << "# of Triggers due to no messages in input Q      :" << logicalProcessStats::waited_long_trigger << endl;
	*statsFile << "# of Successful Triggers due to no messages in input Q      :" << logicalProcessStats::waited_too_long << endl;
	*statsFile << "# of Triggers due to receiving a control message :" << logicalProcessStats::received_control_message << endl;
	*statsFile << "# of Triggers due to full aggregation buffer     :" << logicalProcessStats::write_buffer_full << endl;
	*statsFile << "# of Triggers due to rollback                    :" << logicalProcessStats::got_roll_back << endl;

	*statsFile << "Average Over-Optimistic Lookahead                :" 
		<< (logicalProcessStats::overoptimistic_lookahead)/(logicalProcessStats::lookaheadCount)
		<< endl;

#endif
	lpStats.printNumberOfKernelMessages(statsFile);
};
#endif

#ifdef DISPLAY_SIM_CONFIG   
void
	LogicalProcess::printConfigInfo()
{

	cout <<"\n\tConfiguration for this simulation run is :"
		<<endl
		<<endl;

	cout <<"\t--> LTSF Scheduling " <<endl;

#ifdef EXTERNALAGENT
	cout <<"\t--> Using external agent " << endl;
#endif

#ifdef STATEMANAGER
	cout <<"\t--> Saving state information every event " << endl;
#endif

#ifdef INFREQSTATEMANAGER
	cout <<"\t--> Infrequent state saving : InfreqStateManager" << endl;
#endif

#ifdef LINSTATEMANAGER
	cout <<"\t--> Infrequent state saving : LinStateManager" << endl;
#endif

#ifdef NASHSTATEMANAGER
	cout <<"\t--> Infrequent state saving : NashStateManager" << endl;
#endif

#ifdef COSTFUNCSTATEMANAGER
	cout <<"\t--> Infrequent state saving : CostFuncStateManager" << endl;
#endif

#ifdef GVTMANAGER
	cout <<"\t--> pGVT algorithm for GVT estimation " << endl;
#endif

#ifdef MATTERNGVTMANAGER
	cout <<"\t--> Mattern's algorithm for GVT estimation " << endl;
#endif

#ifdef SEGREGATEDMEMALLOC
	cout <<"\t--> Custom memory manager : SegregatedMemAlloc"<<endl;
#elif defined(GLOBALMEMORYMANAGER)
	cout <<"\t--> Custom memory manager : GlobalMemoryManager"<<endl;
#elif defined(BUDDYMEMORYMANAGER)
	cout <<"\t--> Custom memory manager : BuddyMemoryManager"<<endl;
#elif defined(BRENTMEMALLOC)
	cout <<"\t--> Custom memory manager : BrentMemAlloc"<<endl;
#else
	cout <<"\t--> System's memory manager "<<endl;
#endif

#ifdef ONE_ANTI_MESSAGE
	cout <<"\t--> One anti message optimization "<<endl;
#endif

#ifdef MESSAGE_AGGREGATION
	cout <<"\t--> Message aggregation optimization :  ";

#ifdef MEAN_OF_FACTORS
	cout <<"MEAN_OF_FACTORS" << endl ;
#elif defined(FIXED_PESSIMISM)
	cout <<"FIXED_PESSIMISM" << endl ;
#elif defined(FIXED_MSG_COUNT)
	cout <<"FIXED_MSG_COUNT" << endl ;
#elif defined(PROBE_SEND_RECEIVE_MESSAGES)
	cout <<"PROBE_SEND_RECEIVE_MESSAGES" << endl ;
#elif defined(ADAPTIVE_AGGREGATION)
	cout <<"ADAPTIVE_AGGREGATION" << endl ;
#else
	cout <<"ERROR ! NO OPTIMIZATION SPECIFIED. " << endl ;
#endif


#endif


#ifdef INFREQ_POLLING
	cout <<"\t--> Infrequent Polling optimization :  ";

#ifdef INFREQ_STATIC
	cout <<"INFREQ_STATIC" << endl;
#elif defined(INFREQ_FXD_INCREMENT)
	cout <<"INFREQ_FXD_INCREMENT" << endl;
#elif defined(INFREQ_VAR_INCREMENT)
	cout <<"INFREQ_VAR_INCREMENT" << endl;
#else
	cout <<"ERROR ! NO OPTIMIZATION SPECIFIED. " << endl;
#endif


#endif


#ifdef LAZYCANCELLATION
	cout <<"\t--> Lazy cancellation strategy "<<endl;
#elif  defined(LAZYAGGR_CANCELLATION)
	cout <<"\t--> Dynamic cancellation strategy "<<endl;
#else
	cout <<"\t--> Aggressive cancellation strategy "<<endl;
#endif

}
#endif


void 
LogicalProcess::simulate(VTime /*simUntil*/) {
	register int i;

#ifdef STATS  
  // start the stopWatch - we want timings for all LPs
	lpStats.lpStopWatch.start();
#endif

	register int gvtcount=0;

	if(initialized == false) {
		cerr << "allRegistered not called in LP " << id << endl;
	}

  // wait for init messages for every object in the system, except
  // for our own...
#ifdef LPDEBUG
	outFile << "LP " << id << " waiting for start signal" << endl;
#endif
	if(id == 0) {
		StartMsg foo;
    // let's send out start messages - we don't need one ourselves...
		for(i = 1; i < numLPs; i++) {
			foo.destLP = i;
			comm.recvMsg(&foo);
		}
	}
	else {
		comm.waitForStart();
	}
	cout << "LP " << id << ": starting simulation." << endl;
  // this is the actual execution of the simulation
	simulationFinished = false;

#if defined(INFREQ_POLLING)
	int infreqSkips = 0 ;
	int infreqMsgsGot = 0 ;
	float infreqCurFreq = 1 ;

#ifdef INFREQ_STATIC
	infreqCurFreq = pollStatic ;
#endif
#ifdef INFREQ_TRACE
	infreqTraceFile << infreqCurFreq << endl;
#endif
#endif

	while( gVT.getGVT() < SIMUNTIL) {
		cycleCount++;
#if defined(MPI) || defined(TCPLIB) || defined(FWNS)
		if (numLPs > 1) {
#ifdef LPDEBUG
			int numReceived = comm.recvMPI(1000);
			outFile << numReceived << " messages received" << endl;
#else


#if !defined(INFREQ_POLLING)

#ifdef MCS_TRACE
      //      comm.recvMPI(lrand48()%1001);
			mcsTraceFile << comm.recvMPI(1000) << endl;
#else
      //      comm.recvMPI(lrand48()%1001);
			comm.recvMPI(1000);
#endif

#else
			if(infreqSkips == 0) {
	// we skipped the number of events we wanted to skip
	// call mpi and get number of messages recvd in infreqMsgsRecvd
				infreqMsgsGot = comm.recvMPI(1000);
#ifdef MCS_TRACE
				mcsTraceFile << infreqMsgsGot << endl;
#endif

#ifndef INFREQ_STATIC
				if( infreqMsgsGot > 0){
	  // we got one or more messages 
					if(infreqMsgsGot > 1) {
	    // we got more than one message , we are polling too infrequently
	    // let's decrease polling freq
#ifdef INFREQ_FXD_INCREMENT
						infreqCurFreq -= pollDelta ;
#elif defined(INFREQ_VAR_INCREMENT)
						infreqCurFreq *= (1 - pollDelta)  ;
#endif
						if(infreqCurFreq < 1) infreqCurFreq = 1 ;
					}
				}else{
	  // we got zero messages , let's increase poll freq
#ifdef INFREQ_FXD_INCREMENT
					infreqCurFreq += pollDelta ;
#elif defined(INFREQ_VAR_INCREMENT)
					infreqCurFreq *= (1 + pollDelta ) ;
#endif
					if(infreqCurFreq > pollMax) infreqCurFreq = pollMax ; 
				}
#endif // INFREQ_STATIC
				infreqSkips = (int)infreqCurFreq;
#ifdef INFREQ_TRACE
				infreqTraceFile << infreqSkips << endl;
#endif
			} else {
				infreqSkips-- ;
			}
#endif // INFREQ POLLING


#endif // LPDEBUG
#ifdef MESSAGE_AGGREGATION
#ifndef ADAPTIVE_AGGREGATION
			incrementAgeOfAggregatedMessage();
#endif
#endif
		}
#endif // if defined(MPI) || defined(TCPLIB)

    // seek to the first event to execute in the queue
		BasicEvent *nextEvent = BasicTimeWarp::inputQ.getEventToSchedule();
#ifdef MESSAGE_AGGREGATION
#ifdef FIXED_MSG_COUNT
		if(nextEvent == NULL) { //The inputQ is empty
      //flushing when the inputQ is empty
			comm.flushIfWaitedTooLong();
		}
#endif
#if defined (ADAPTIVE_AGGREGATION) || defined(PROBE_SEND_RECEIVE_MESSAGES)
		if(nextEvent == NULL) { //The inputQ is empty
      //flushing when the inputQ is empty
			comm.flush();
		}
		else {
			comm.checkToSend(BasicTimeWarp::inputQ.getNextUnprocessedEventTime());
		}
#endif
#endif
		int executeCycle = 0;
    // if the event is not already processed, execute this event
		if(nextEvent != NULL){

#if (defined(MESSAGE_AGGREGATION) && defined(STATS))

			lpStats.incrementLookahead(nextEvent->recvTime - gVT.getGVT() );

#endif 


#ifdef LPDEBUG
			outFile << *nextEvent << "\n";
#endif
			if(nextEvent->alreadyProcessed != true){
#ifdef EXTERNALAGENT
				if(id == 0){
					if(gVT.numberOfEvents > MAX_EVENTS_TO_WAIT && 
							gVT.tokenAlreadySent == false){
						cout << "numberOfEvents = " << gVT.numberOfEvents  
							<< " && tokenAlreadySent = " << gVT.tokenAlreadySent << endl;
						gVT.startExternalAgent();
						gVT.numberOfEvents = 0;
					}
					else {
						gVT.numberOfEvents++;
					}
				}
				else {
					if(gVT.tokenReceived == true){
						gVT.numberOfEvents = 0;
						gVT.tokenReceived = false;
					}
					else{
						gVT.numberOfEvents++;
						gVT.readyToPassItOn();
					}
				}
#endif
				if(nextEvent->recvTime <= SIMUNTIL){

	  //set the flag ranBetweenTokens in GVTManager
					gVT.setRanBetweenTokens(true) ;

					scheduler.runProcesses();
	  //nextEvent = BasicTimeWarp::inputQ.seek(1,CURRENT);
				}
			}
			else {
				nextEvent = BasicTimeWarp::inputQ.seek(1,CURRENT);
			}
		}
		executeCycle++;
		nextEvent = BasicTimeWarp::inputQ.get();
		if(gvtcount < MAX_GVT_COUNT ) {
			gvtcount++;
		} else  {
			gVT.calculateLGVT();
			gvtcount = 0;
			getGVT = false;
			cout << "GVT: " << gVT.getGVT() << endl;
		}
	} // while ( gVT.getGVT() < simUntil && simulationFinished == false )
#ifdef MESSAGE_AGGREGATION
    //Let me flush out all my send buffers, when I have reached PINFINITY
	comm.flush();
#endif
  // this might need to move, if more than one call to simulate is needed

      // Garbage collect the output queues first and then
      // garbage collect the rest of the stuff in each process.
      // This is needed to avoid memory errors in the system, when the
      // output queue container is pointing to an event in the input queue
      // of a process with a lower id (on the same lp). 

	for(i = 0; (i < totalObjects); i++) {
		simArray[i].ptr->finalGarbageCollectOutputQueue();
	}

	for(i = 0; (i < totalObjects); i++) {
		simArray[i].ptr->finalize();
		simArray[i].ptr->finalGarbageCollect();
	}

#ifdef STATS      
      // stop the clock !!
	lpStats.lpStopWatch.stop();
#endif

	if(id == 0) {
		cout << "Simulation complete!" << endl;
	}
}


void 
LogicalProcess::registerObject(BasicTimeWarp* handle) {
	numRegistered++;

	if (handle->id >= totalObjects) {
		ostrstream errmsg;
		char* errstr;
		errmsg << "Object id " << handle->id << " trying to register, but " 
			<< totalObjects-1 << " is the largest id allowed!" << ends;
		errstr = errmsg.str();
		TerminateMsg * ouch = new TerminateMsg;
		strcpy(ouch->error, errstr);
		comm.recvMsg(ouch);
	}
	if(simArray[handle->id].ptr == NULL){
    //insert process into the process scheduler
		handle->setSchedulerHandle(&scheduler);
		scheduler.initialize(&(handle->inputQ), simArray, id); 
		handle->localId = numRegistered - 1;

    // now let's let the Communication guy know that we're 
    // registering this dude

		simArray[handle->id].ptr = handle;
		simArray[handle->id].lpNum = id;
#ifdef STATS
		lpStats.setEventIds(handle->id); // store event ids for later use
#endif
	}
	else {
		cerr << "Object " << handle->id << " trying to register twice!" << endl;
		TerminateMsg * ouch = new TerminateMsg;
		strcpy(ouch->error, "Object tried to register twice" );
		comm.recvMsg(ouch);
	}

	handle->setCommHandle(simArray);
	handle->gVTHandle = &gVT;

  // this needed by MESSAGE_AGGREGATION, ONE_ANTI_MESSAGE 
  // and for stats collection 
	handle->setLPHandle(this);

#ifdef STATS
	handle->setStatsFile(&commitStatsFile);
#endif
#if defined(LPDEBUG) || defined(OPENLPDEBUGFILE)
	handle->setFile(&outFile);
#endif

#ifdef STATEDEBUG
	handle->setStateFile(&outFile2);
#endif

#ifdef OBJECTDEBUG
	handle->openFile();
#endif

#if defined (LAZYCANCELLATION) || defined(LAZYAGGR_CANCELLATION)
#ifdef STATS
	char dcStatsFilename[80];
	strcpy(dcStatsFilename,handle->name);
	strcat(dcStatsFilename,".dcstats");
	handle->openDCStatsFile(dcStatsFilename);
#endif
#endif

}

void
LogicalProcess::allRegistered( void ) {
	int i,j;

	if( numRegistered == numObjects ){

		gVT.initGVTManager( simArray, &comm, id, totalObjects, numObjects, numLPs);

		if(id == 0){
      // at this point, if we are object 0, we should wait for init messages
      // from everyone...
			comm.waitForInit(totalObjects - numObjects);
      // now we've got all of the objects... we need to send out all of the
      // objects to every one else
			InitMsg initMsg;
			for(i = 0; i < totalObjects; i++) {
				initMsg.objId = i;
				initMsg.lpId = simArray[i].lpNum;
				for(j = 1; j < numLPs; j++) {
					initMsg.destLP = j;
					comm.recvMsg(&initMsg);
				} // for j
			} // for i
		} // if id == 0
		else {
      // we should walk through our local array sending the objects to the
      // main guy...
			for(i = 0 ; i < totalObjects; i++ ){
				if(simArray[i].lpNum == id) {
					comm.iHaveObject(simArray[i].ptr->id);
				} // if
			} // for
      // OK, now we need to receive all of the data FROM the head guy...
			comm.waitForInit( totalObjects );
		}
		cout << "LP " << id << ": initializing simulation objects" << endl << flush;
    // call the initialization routine and save initial state for everyone
		for(i = 0; i < totalObjects; i++) {
			if (simArray[i].lpNum == id) {

	// NEW_STATE_MANAGEMENT
	// With the new state management in place LogicalProcess calls the
	// protected virtual method in BasicTimeWarp which is overloaded
	// in TimeWarp that creates a new state and inturn calls initialize()
				simArray[i].ptr->timeWarpInit();

				simArray[i].ptr->saveState();
			}
		}

    // This has to be done later than the previous loop because of OFC.
		for(i = 0; i < totalObjects; i++) {
			if (simArray[i].lpNum == id) {
	// set preconditions for first state
				simArray[i].ptr->clearInitState(); 
			}
		}
		initialized = true;
		BasicTimeWarp::inputQ.initialized = true;
	}
	else {
		cerr << "LP " << id << " incorrect number of objects registered!" << endl;
		cerr << "Expected " << numObjects << " objects, and " << numRegistered <<
			" registered!" << endl;
		TerminateMsg * ouch = new TerminateMsg;
		strcpy(ouch->error, "invalid number of objects registered" );
		comm.recvMsg(ouch);   
	}
}

int
LogicalProcess::getNumObjects() {
	return numObjects;
}

int
LogicalProcess::getTotalNumberOfObjects() const {
	return totalObjects;

}

void
LogicalProcess::calculateGVT() {
	getGVT = true;
}
#endif

