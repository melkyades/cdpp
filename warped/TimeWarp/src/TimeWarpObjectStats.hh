//-*-c++-*-
#ifndef SIMULATIONOBJECTSTATS_HH
#define SIMULATIONOBJECTSTATS_HH
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
#include "config.hh"

class simulationObjectStats 
{
public:
  simulationObjectStats(){
    rollbackCount = 0;
    rollbackDistance = ZERO;
    rollbackDistanceStates = 0;
    maxRollbackDistanceStates = 0;
    maxRollbackDistanceTime = ZERO;
    //eventsUndone, is used to keep track of the work that is
    //undone because of rollback.
    eventsUndone = 0;
    posSent = 0;
    negSent = 0;
    posRecv = 0;
    negRecv = 0;
    eventsProcessed    = 0;
    posStragglers      = 0;
    maximumStateQSize  = 0;
    minimumStateQSize  = 2147483647;
    maximumStateQTimeDiff  = ZERO;
    minimumStateQTimeDiff  = PINFINITY;
    sumStateQSize      = 0;
    sumStateQSizeTime  = ZERO;
    numStateQSizeCalls = 0;
#ifdef ONE_ANTI_MESSAGE
    localEventsSuppressed = 0;
    remoteEventsSuppressed = 0;
#endif
  };
  
  ~simulationObjectStats(){};

  void incrementRollbackCount(){
    rollbackCount++;
  };
  
  int getRollbackCount(){
    return rollbackCount;
  };

  void setRollbackDistanceTime(VTime newRollbackDistance){
    if (newRollbackDistance > maxRollbackDistanceTime) {
      maxRollbackDistanceTime = newRollbackDistance;
    }
    rollbackDistance = rollbackDistance + newRollbackDistance;
  };

  void setRollbackDistanceStates(int newRollbackDistance){
    if (newRollbackDistance > maxRollbackDistanceStates) {
      maxRollbackDistanceStates = newRollbackDistance;
    }
    rollbackDistanceStates += newRollbackDistance;
  };

  VTime getRollbackDistanceTime(){
    return rollbackDistance;
  };

  int getRollbackDistanceStates(){
    return rollbackDistanceStates;
  };

  VTime getMaxRollbackDistanceTime() {
    return maxRollbackDistanceTime;
  }

  int getMaxRollbackDistanceStates() {
    return maxRollbackDistanceStates;
  }

  void incrementEventsUndone() {
    eventsUndone++;
  }

  int getEventsUndone() {
    return eventsUndone;
  }

  void incrementPosSent(){
    posSent++;
  };
  
  int getPosSent(){
    return posSent;
  };

  void incrementNegSent(){
    negSent++;
  };
  
  int getNegSent(){
    return negSent;
  };

  void incrementPosRecv(){
    posRecv++;
  };
  
  int getPosRecv(){
    return posRecv;
  };

  void incrementNegRecv(){
    negRecv++;
  };
  
  int getNegRecv(){
    return negRecv;
  };

  void incrementEventsProcessed(){
    eventsProcessed++;
  };
  
  int getEventsProcessed(){
    return eventsProcessed;
  };

#ifdef ONE_ANTI_MESSAGE
  void incrementLocalEventsSupp(){
    localEventsSuppressed++;
  };
  
  int getLocalEventsSupp(){
    return localEventsSuppressed;
  };

  void incrementRemoteEventsSupp(){
    remoteEventsSuppressed++;
  };
  
  int getRemoteEventsSupp(){
    return remoteEventsSuppressed;
  };
#endif

  int getNumberOfPosStragglers(){
    return posStragglers;
  }
  
  void incrementNumberOfPosStragglers(){
    posStragglers++;
  }

  void setStateQSize( int size, VTime timeDiff) {
    sumStateQSize += size;
    sumStateQSizeTime += timeDiff;
    numStateQSizeCalls++;
    if (size > maximumStateQSize) {
      maximumStateQSize = size;
    }
    if (timeDiff > maximumStateQTimeDiff) {
      maximumStateQTimeDiff = timeDiff;
    }
    if (size < minimumStateQSize) {
      minimumStateQSize = size;
    }
    if (timeDiff < minimumStateQTimeDiff) {
      minimumStateQTimeDiff = timeDiff;
    }
  }

  void setStateQSize( int size) {
    sumStateQSize += size;
    numStateQSizeCalls++;
    if (size > maximumStateQSize) {
      maximumStateQSize = size;
    }
    if (size < minimumStateQSize) {
      minimumStateQSize = size;
    }
  }

  int getAvgStateQSize() {
    if (numStateQSizeCalls == 0) {
      return 0;
    }
    else {
      return (int) sumStateQSize/numStateQSizeCalls;
    }
  }

//AT: Originally this function returned an int.
  VTime getAvgStateQTimeDiff() {
    if (numStateQSizeCalls == 0) {
      return 0;
    }
    else {
      return sumStateQSizeTime/numStateQSizeCalls;
    }
  }

  int getNumStateQSizeCalls() {
    return numStateQSizeCalls;
  }

  int getMaximumStateQSize() {
    return maximumStateQSize;
  }

  int getMinimumStateQSize() {
    return minimumStateQSize;
  }

  VTime getMaximumStateQTimeDiff() {
    return maximumStateQTimeDiff;
  }

  VTime getMinimumStateQTimeDiff() {
    return minimumStateQTimeDiff;
  }

private:
  int rollbackCount;
  VTime rollbackDistance;
  VTime maxRollbackDistanceTime;
  int rollbackDistanceStates;
  int maxRollbackDistanceStates;
  int eventsUndone;
  int posSent;
  int negSent;
  int posRecv;
  int negRecv;
  int eventsProcessed;
  int posStragglers;
  int maximumStateQSize;
  int minimumStateQSize;
  VTime maximumStateQTimeDiff;
  VTime minimumStateQTimeDiff;
  float sumStateQSize;

  //float sumStateQSizeTime;		05/11 AT
  VTime sumStateQSizeTime;
  int numStateQSizeCalls;

#ifdef ONE_ANTI_MESSAGE
  int localEventsSuppressed;
  int remoteEventsSuppressed;
#endif

  //*** statistics that need to be collected ***
  //Events processed, Events canceled, Stragglers/Time-faults,
  //Events rolled back due to stragglers
  //Events rolled back due to cancellations
  //Most number of events rolled back in a single stretch
  
  // *** Summary Time Warp Statistics*** 
  
  //No. of PEs      
  //No. of LPs      
  //Events processed
  //Longest rollback          
  //FossilCollect attempts    
  //State Reclamation attempts
  
  //Net Events processed      
  //Number of rollbacks       
  //Proc. Evnts RB'd          
  //Proc. Evnts Cncl'd        
  //Average RB Distance       
  //Event Rate (Evnts/Sec)    
  
  //Execution time (Sec)
};

#endif 
