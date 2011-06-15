// -*-c++-*-
#ifndef ADAPTSTATEMANAGER_CC
#define ADAPTSTATEMANAGER_CC
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
// $Id: AdaptStateManager.cc,v 1.3 1999/01/26 18:08:35 mal Exp $
//
//
//---------------------------------------------------------------------------

#include "AdaptStateManager.hh"

AdaptStateManager::AdaptStateManager(BasicTimeWarp *myProcessPtr):
  InfreqStateManager(myProcessPtr) {
    steadyState = false; 
    nonoptCheckpointInterval = true;
    lastRollbackCount = -1;
    statePeriod = 0;
    userEventsCommitted = 0;
    sumEventTime = 0;
    rollbackTime = 0;
    pausedTime = 0;
}

inline void
AdaptStateManager::startEventTiming() {
  pausedTime = 0;
  stopWatch.start();
}

inline bool
AdaptStateManager::stopEventTiming() {
  stopWatch.stop();
  eventTime = stopWatch.elapsed() - pausedTime;
#ifdef DEBUGTIMINGS
    *lpFile << nameOfObject << " Execution Time : " << stopWatch.elapsed()
	     << "\n" << nameOfObject << " paused Time: " << pausedTime
	     << "\n" << nameOfObject << " Event Time: " << eventTime
	     << "\n";
#endif
  return ( executionTiming(eventTime) ); 
}


inline void
AdaptStateManager::pauseEventTiming() {
  messageStopWatch.start();
}


inline void
AdaptStateManager::resumeEventTiming() {
  messageStopWatch.stop();
  pausedTime = messageStopWatch.elapsed();
}


inline void
AdaptStateManager::startRollbackTiming() {
  rollbackStopWatch.start();
}


inline void
AdaptStateManager::finishRollbackTiming() {
  rollbackStopWatch.stop();
  rollbackTime += rollbackStopWatch.elapsed();
#ifdef DEBUGTIMINGS
    *lpFile << nameOfObject << " Rollback Time : " << rollbackTime
	     << "\n";
#endif
}


inline double
AdaptStateManager::getEventExecutionTime() {
return(EventExecutionTimeWeighted.getAvg());
}


inline double
AdaptStateManager::getStateSavingTime() {
return(StateSaveTimeWeighted.getAvg());
}


inline double
AdaptStateManager::getCoastForwardTime() {
return(CoastForwardTimeWeighted.getAvg());

}



inline void 
AdaptStateManager::startStateTiming() {
  stopWatch.start();
}


inline void 
AdaptStateManager::stopStateTiming() { 
  stopWatch.stop();
  if (timeAtLastCall != current->lVT) {
#ifdef DEBUGTIMINGS
    *lpFile << nameOfObject << " State Saving Time : " << stopWatch.elapsed()
	     << "\n";
#endif
    StateSaveTimeWeighted.update ( stopWatch.elapsed() );
  }
}


inline bool
AdaptStateManager::executionTiming(double execTime) { 
  if (timeAtLastCall == current->lVT) {
#ifdef DEBUGTIMINGS
    *lpFile << nameOfObject << " Execution Time : " << execTime << "\n";
#endif
    sumEventTime += execTime;
  }
  else {
    // NOTE: This is the execution time for the PREVIOUS RUN. We sum the 
    // values until we move to the next LVT value and then save the sum.
#ifdef DEBUGTIMINGS
    *lpFile << nameOfObject << " Execution Time update: " << sumEventTime 
	     << "\n";
#endif
    EventExecutionTimeWeighted.update( sumEventTime);

    // add the time required for any rollbacks to the time to execute 
    // this event.
    sumEventTime = execTime + rollbackTime;
    rollbackTime = 0;

  }
  return((bool)!steadyState);
}

void 
AdaptStateManager::saveState() {
// This procedure is duplicated to allow the timing code to be turned off
// It should be similar to InfreqStateManager::saveState code otherwise.

  if (timeAtLastCall != current->lVT) {

    if (periodCounter == 0) {
      if (steadyState == false) {
	// We have not saved a state for this time and we have not
	// executed a message at this time, so we have just executed the
	// first message at this time.
	// (Only save the first state at this time)
	startStateTiming();
	// state will be clean;
	current->dirty = false;
	StateManager::saveState();
	periodCounter = statePeriod;
	stopStateTiming();
      }
      else {
	// state will be clean;
	current->dirty = false;
	StateManager::saveState();
	periodCounter = statePeriod;
      }
    }
    else {
      periodCounter--;
      // If this function is called then the last state will be dirty
      // unless this state is saved.
      (stateQ.back() )->dirty = true;

    }
    timeAtLastCall = current->lVT;
  }
  else {
    // If this function is called then the last state will be dirty
    // unless this state is saved.
    (stateQ.back() )->dirty = true;
  }

}
#endif
