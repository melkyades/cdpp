// -*-c++-*-
#ifndef LINSTATEMANAGER_CC
#define LINSTATEMANAGER_CC
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
// $Id: LinStateManager.cc,v 1.3 1999/01/29 20:39:06 mal Exp $
//
//
//---------------------------------------------------------------------------

#include "LinStateManager.hh"

LinStateManager::LinStateManager(BasicTimeWarp *myProcessPtr) :
  AdaptStateManager(myProcessPtr) {
  // Arbitrary number of events processed until steady state and
  // between recalculations.
  eventToRecalc = 10000;
  eventsBetweenRecalc = 1000;
  lastRollbackCount = 0;
  lastCheckpointInterval = getStatePeriod();
  forwardExecutionLength = 0;
  for (int i = 0; i < MAXDEFAULTINTERVAL; i++) {
    usedIntervalBefore[i] = false;
  }
}


inline void
LinStateManager::setAdaptiveParameters(long eventstosteadystate,
						 long eventsbeforerecalcuate) {
  eventToRecalc = eventstosteadystate;
  eventsBetweenRecalc = eventsbeforerecalcuate;  
}


inline void
LinStateManager::coastedForwardEvents(int eventsCoastedForward) {
  forwardExecutionLength += eventsCoastedForward;
}


inline void
LinStateManager::committedEvents(long eventsCommitted, 
					      int rollbackCount) {

  userEventsCommitted += eventsCommitted;

  if (nonoptCheckpointInterval) {

    if ( userEventsCommitted >= eventToRecalc) {
      steadyState = true;
      calculatePeriod(rollbackCount);
      eventToRecalc = userEventsCommitted + eventsBetweenRecalc;
    }
    if ((rollbackCount == 0) && ( forwardExecutionLength >= eventToRecalc )) {
      // no rollbacks at all so set checkpoint interval for maximum
      steadyState = true;
      nonoptCheckpointInterval = false;
      setStatePeriod(MAXDEFAULTINTERVAL);
    }
  }
}


inline void
LinStateManager::calculatePeriod(int rollbackCount) {

  double eventTime, stateTime;
  eventTime = EventExecutionTimeWeighted.getAvg();
  stateTime = StateSaveTimeWeighted.getAvg();

  if ( (eventTime != 0)  && (stateTime != 0) ) {
    int numberOfRollbacks, period;
    
    numberOfRollbacks = rollbackCount - lastRollbackCount;
    double squarePeriod, alpha;
      
#ifdef OBJECTDEBUG
    *lpFile << nameOfObject << ": Weighted state saving time = " 
	     << stateTime
	     << "Weighted event execution time = " 
	     << eventTime << endl;
#endif

    if (numberOfRollbacks == 0) {
      period = MAXDEFAULTINTERVAL;
    }
    else {
      alpha = forwardExecutionLength / (float) numberOfRollbacks;
      squarePeriod = ((2.0*alpha + 1.0)*stateTime)/
	eventTime;
      if (squarePeriod < 1.0) {
	period = 1;
      } else {
	period = (int) ceil(sqrt( squarePeriod ));
      }
    }
      
    // check to see if this period has been used before. First we
    // see if the checkpoint interval is the same for the new interval.
    // If not, then we check to see if we have EVER used this interval
    // before.
    if ( (lastCheckpointInterval == period) || 
	 ( (period < MAXDEFAULTINTERVAL) && 
	   (usedIntervalBefore[period] == true) ) ) {
      nonoptCheckpointInterval = false;
    }
    usedIntervalBefore[period] = true;
    lastCheckpointInterval = period;
    
    // reset values
    forwardExecutionLength = 0;
    lastRollbackCount = rollbackCount;
      
    setStatePeriod(period - 1);
  }
}


inline void
LinStateManager::saveState() {
  if (timeAtLastCall != current->lVT) {
    forwardExecutionLength++;
  }
  AdaptStateManager::saveState();
}
#endif
