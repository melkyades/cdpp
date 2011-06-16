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
#include "config.hh"
#include "warped.hh"
#include "Stopwatch.hh"
#include "KernelMsgs.hh"
#include <iomanip>
#include <limits>


class logicalProcessStats 
{
public:
  logicalProcessStats(int numberOfObjects);
  
  ~logicalProcessStats(){
    delete [] eventIds;
  };

  Stopwatch lpStopWatch; // measures execution time
  Stopwatch lpIdleStopWatch; // measures LP Idle time
  int *eventIds; // stores event ids of objects on this LP
  int kernelMsgs[22]; // because there are 22 types of control messages

  inline void incrementTotalEventsProcessed(){
    totalEventsProcessed++;
  };
  
  inline void recordReceivedKernelMessages(KernelMsgType msgType){
    kernelMsgs[msgType]++;
  };

  void printNumberOfKernelMessages(ostream* statsFile);

  inline int getTotalEventsProcessed(){
    return totalEventsProcessed;
  };

  inline void setTotalEventsCommitted(int committedEvents){
    totalEventsCommitted = committedEvents;
  };
  
  inline int getTotalEventsCommitted(){
    return totalEventsCommitted;
  };

  inline void incrementTotalEventsUndone() {
    totalEventsUndone++;
  }

  inline int getTotalEventsUndone() {
    return totalEventsUndone;
  }

  inline double getExecutionTime(){
    return(lpStopWatch.elapsed()/1000000000.0);
  }

  inline void setEventIds(int eventId){
    eventIds[objectNum] = eventId;
    objectNum++;
  }


  inline void incrementSentPhysicalMessageCount(int increment=1) {
    numPhyMsgsSent += increment;
  }
  
  inline void incrementReceivedPhysicalMessageCount(int increment=1) {
    numPhyMsgsRecv += increment;
  }
  
  inline void incrementSentApplicationMessageCount(int increment=1) {
    numApplMsgsSent += increment;
  }
  
  inline void incrementReceivedApplicationMessageCount(int increment=1) {
    numApplMsgsRecv += increment;
  }
  
  inline int getNumberOfPhysicalMessagesSent(){
    return numPhyMsgsSent;
  }

  inline int getNumberOfPhysicalMessagesRecv(){
    return numPhyMsgsRecv;
  }
  inline int getNumberOfApplicationMessagesSent(){
    return numApplMsgsSent;
  }
  inline int getNumberOfApplicationMessagesRecv(){
    return numApplMsgsRecv;
  }

#if (defined(MESSAGE_AGGREGATION) && defined(STATS))
  // this keeps track of all the different aggregate sizes that were
  // accumulated during the simulation
  int statsArray[MAX_AGGREGATE_SIZE];

  static int window_size_exhausted;
  static int waited_too_long;
  static int waited_long_trigger;
  static int received_control_message;
  static int received_anti_message;
  static int write_buffer_full;
  static int got_roll_back;

  static VTime overoptimistic_lookahead;
  static int   lookaheadCount;

  static void incrementLookahead(VTime lookahead){
  // lookahead indicates how far ahead of GVT this LP is

    overoptimistic_lookahead += lookahead;
    lookaheadCount++;
  }
    
  static void increment_window_size_exhausted() {
    window_size_exhausted++;
  }

  static void increment_waited_too_long() {
    waited_too_long++;
  }

  static void increment_waited_long_trigger() {
    waited_long_trigger++;
  }

  static void increment_received_control_message() {
    received_control_message++;
  }

  static void increment_received_anti_message() {
    received_anti_message++;
  }

  static void increment_write_buffer_full() {
    write_buffer_full++;
  }

  static void increment_got_roll_back() {
    got_roll_back++;
  }

#endif

private:
  int totalEventsProcessed; // events processed by this LP
  int totalEventsCommitted; // events committed by this LP
  int totalEventsUndone;
  int objectNum; // counter for registering simulation objects

  //This count gives the number of MPI_Bsend calls have been made
  int numPhyMsgsRecv;
  int numPhyMsgsSent;

  //This count gives the number of Application Messages(Events)
  //being generated
  int numApplMsgsRecv;
  int numApplMsgsSent;

};

#endif 
