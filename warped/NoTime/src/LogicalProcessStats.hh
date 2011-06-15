//-*-c++-*-
#ifndef LOGICALPROCESSSTATS_HH
#define LOGICALPROCESSSTATS_HH
// Copyright (c) 1994-1998 Ohio Board of Regents and the University of
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
// $Id: 
//
//---------------------------------------------------------------------------
#include "warped.hh"
#include "util/Stopwatch.hh"
#include "KernelMsgs.hh"
#include <iomanip.h>

class logicalProcessStats 
{
public:
  logicalProcessStats(int numberOfObjects){
    totalEventsProcessed = 0;
    totalEventsCommitted = 0;
    totalEventsUndone = 0;
    objectNum = 0;
    eventIds = new int[numberOfObjects];
    for(int i=0;i < 22; i++){
      kernelMsgs[i] = 0;
    }
#ifdef MESSAGE_AGGREGATION
    numPhyMsgsRecv  = 0;
    numPhyMsgsSent  = 0;
    numApplMsgsRecv = 0;
    numApplMsgsSent = 0;
    for(int i=0; i < MAX_AGGREGATE_SIZE; i++){
      statsArray[i] = 0;
    }
#endif
  };
  
  ~logicalProcessStats(){};

  Stopwatch lpStopWatch; // measures execution time
  int *eventIds; // stores event ids of objects on this LP
  int kernelMsgs[22]; // because there are 22 types of control messages

  void incrementTotalEventsProcessed(){
    totalEventsProcessed++;
  };
  
  void recordReceivedKernelMessages(KernelMsgType msgType){
    kernelMsgs[msgType]++;
  };

  void printNumberOfKernelMessages(ostream* statsFile){
    *statsFile << "-----------------------------------------------------------------------" << endl;    
    *statsFile << " *** Kernel Messages Sent/Received through MPI ***" << endl;
    *statsFile << "-----------------------------------------------------------------------" << endl;
    *statsFile <<"TERMN EVENTMSGS LGVTMSG LLGVTMG NEWGVT GVTACK LPACK  RCHKPT" 
	       << endl;
    *statsFile << "-----------------------------------------------------------------------" << endl;
    for(int i=1; i < 22; i++){
      switch(i) {
      case 3:
	*statsFile << "[" << setw(3) << kernelMsgs[i] << "] ";
	break;
      case 5:
	*statsFile << "[" << setw(7) << kernelMsgs[i] << "] ";
	break;
      case 6:
      case 7:
	*statsFile << "[" << setw(5) << kernelMsgs[i] << "] ";
	break;
      case 8:
      case 9:
      case 10:
	*statsFile << "[" << setw(4) << kernelMsgs[i] << "] ";
	break;
#ifdef MATTERNGVTMANAGER
      case 13:
	*statsFile << endl << "Number of MATTERNGVTMSG : " << kernelMsgs[i] 
		  << endl;
	break;
      case 14:
        *statsFile << "Number of RESTORECKPTMSG : " << kernelMsgs[i] << endl;
	break;
      case 20:
	*statsFile << "Number of EXTAGENTMSG : " << kernelMsgs[i] << endl;
	break;
#else
      case 13:
	*statsFile << "[" << setw(4) << kernelMsgs[i] << "]" << endl;
	*statsFile << "-----------------------------------------------------------------------" << endl;
	break;
#endif
#ifdef MESSAGE_AGGREGATION
      case 15:
	//*statsFile << "Number of TIMEWARPOBJMSG : " << kernelMsgs[i] << endl;
	break;
      case 16:
	//*statsFile << "Number of TRANSFEROBJMSG : " << kernelMsgs[i] << endl;
	break;
      case 17:
	//*statsFile << "Number of OUTPUTQOBJMSG : " << kernelMsgs[i] << endl;
	break;
      case 18:
	//*statsFile << "Number of INPUTQOBJMSG : " << kernelMsgs[i] << endl;
	break;
      case 19:
	//*statsFile << "Number of STATEQOBJMSG : " << kernelMsgs[i] << endl;
	break;
      case 21:
	//*statsFile << "Number of AGGREGATEMSG : " << kernelMsgs[i] << endl;
	break;
#endif
      default:
	break;
      }
    }
  };

  int getTotalEventsProcessed(){
    return totalEventsProcessed;
  };

  void setTotalEventsCommitted(int committedEvents){
    totalEventsCommitted = committedEvents;
  };
  
  int getTotalEventsCommitted(){
    return totalEventsCommitted;
  };

  void incrementTotalEventsUndone() {
    totalEventsUndone++;
  }

  int getTotalEventsUndone() {
    return totalEventsUndone;
  }

  double getExecutionTime(){
    return(lpStopWatch.elapsed()/1000000000.0);
  }

  void setEventIds(int eventId){
    eventIds[objectNum] = eventId;
    objectNum++;
  }
#ifdef MESSAGE_AGGREGATION
  void incrementSentPhysicalMessageCount(int increment=1) {
    numPhyMsgsSent += increment;
  }
  
  void incrementReceivedPhysicalMessageCount(int increment=1) {
    numPhyMsgsRecv += increment;
  }
  
  void incrementSentApplicationMessageCount(int increment=1) {
    numApplMsgsSent += increment;
  }
  
  void incrementReceivedApplicationMessageCount(int increment=1) {
    numApplMsgsRecv += increment;
  }
  
  int getNumberOfPhysicalMessagesSent(){
    return numPhyMsgsSent;
  }

  int getNumberOfPhysicalMessagesRecv(){
    return numPhyMsgsRecv;
  }
  int getNumberOfApplicationMessagesSent(){
    return numApplMsgsSent;
  }
  int getNumberOfApplicationMessagesRecv(){
    return numApplMsgsRecv;
  }
  
#ifdef STATS
  // this keeps track of all the different aggregate sizes that were
  // accumulated during the simulation
  int statsArray[MAX_AGGREGATE_SIZE];
#endif

#endif
    
private:
  int totalEventsProcessed; // events processed by this LP
  int totalEventsCommitted; // events committed by this LP
  int totalEventsUndone;
  int objectNum; // counter for registering simulation objects

#ifdef MESSAGE_AGGREGATION
  //This count gives the number of MPI_Bsend calls have been made
  int numPhyMsgsRecv;
  int numPhyMsgsSent;

  //This count gives the number of Application Messages(Events)
  //being generated
  int numApplMsgsRecv;
  int numApplMsgsSent;
#endif
};

#endif 
