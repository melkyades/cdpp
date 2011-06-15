// -*-c++-*- 
#ifndef NASHSTATEMANAGER_CC 
#define NASHSTATEMANAGER_CC 
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
// $Id: NashStateManager.cc,v 1.2 1999/01/26 18:08:36 mal Exp $
//
//
//---------------------------------------------------------------------------

#include "NashStateManager.hh"

NashStateManager::NashStateManager(BasicTimeWarp *myProcessPtr)  :
  AdaptStateManager(myProcessPtr){
  // Arbitrary number of events processed until steady state and
  // between recalculations.
  eventToRecalc = 1000;
  eventsBetweenRecalc = 1000;
  lastRollbackCount = 0;
  lastCheckpointInterval = 0;
  forwardExecutionLength = 0;
  rollbackLengthWeighted.set_weights(0.6, 0.4);
}


NashStateManager::~NashStateManager() {
#ifdef STATS
  *statsFile << "Weighted average of Rollbacks = " 
	     << numberOfRollbacksWeighted.getAvg()
	     << "\nAverage rollback length = "
	     << rollbackLength.getAvg()
	     << "\nWeighted average of events committed = " 
	     << eventsCommittedWeighted.getAvg();

#endif
}


inline void
NashStateManager::setAdaptiveParameters(long eventstosteadystate,
						 long eventsbeforerecalcuate) {
  eventToRecalc = eventstosteadystate;
  eventsBetweenRecalc = eventsbeforerecalcuate;  
}


inline void
NashStateManager::coastedForwardEvents(int eventsCoastedForward) {
  forwardExecutionLength += eventsCoastedForward;

}


void
NashStateManager::committedEvents(long eventsCommitted, 
					      int rollbackCount) {

  userEventsCommitted += eventsCommitted;

  if ( userEventsCommitted >= eventToRecalc) {
    calculatePeriod(rollbackCount);
    eventToRecalc = userEventsCommitted + eventsBetweenRecalc;
  }
  if ((rollbackCount == 0) && ( forwardExecutionLength >= eventToRecalc )) {
    // no rollbacks at all so set checkpoint interval for maximum
    setStatePeriod(MAXDEFAULTINTERVAL);
  }
}


void
NashStateManager::calculatePeriod(int rollbackCount) {

  double eventTime, stateTime;

  eventTime = EventExecutionTimeWeighted.getAvg();
  stateTime = StateSaveTimeWeighted.getAvg();

#ifdef LPDEBUG
      *lpFile << nameOfObject << "Weighted event execution time = " << eventTime
	       << "Weighted state saving time = "  << stateTime;
#endif
  if ( (eventTime != 0) && 
       (stateTime != 0) ) {

    double squarePeriod, alpha, beta;
    int numberOfRollbacks, period;

    EventExecutionTimeWeighted.update(eventExecutionTime.getAvg());
    StateSaveTimeWeighted.update(stateSaveTime.getAvg());

    alpha = stateTime/eventTime;

    numberOfRollbacks = rollbackCount - lastRollbackCount;

    numberOfRollbacksWeighted.update(numberOfRollbacks);

    eventsCommittedWeighted.update(userEventsCommitted - 
				   lastUserEventsCommitted);

    rollbackLengthWeighted.update(rollbackLength.getAvg()*
				  ((getStatePeriod()+1)/2));

#ifdef LPDEBUG
      *lpFile << nameOfObject << " numberOfRollbacks = " << numberOfRollbacks
	       << " numberOfRollbacksWeighted = " 
	       << numberOfRollbacksWeighted.getAvg()
               << "\n rollbackLength = "
               << rollbackLength.getAvg()
	       << " userEventsCommitted = " << userEventsCommitted << "\n"
	       << "\n eventsCommittedWeighted = " 
	       << eventsCommittedWeighted.getAvg()
               << "\nalpha = " << alpha;
#endif
    if (numberOfRollbacksWeighted.getAvg() > 0) {
      beta = eventsCommittedWeighted.getAvg()/
	numberOfRollbacksWeighted.getAvg();
#ifdef LPDEBUG
      *lpFile << nameOfObject << " beta = " << beta
	       << endl;
#endif      
      squarePeriod = (2.0*alpha*(beta +rollbackLengthWeighted.getAvg() - 1.0));

      if (squarePeriod < 1.5) {
	period = 1;
      } else {
	period = (int) ceil(sqrt( squarePeriod ));
      }
 
      cout << nameOfObject << "period = " << period - 1 << endl;

#ifdef LPDEBUG
      *lpFile << nameOfObject << "period = " << period - 1 << "\n";
#endif
      // reset timers
      StateSaveTimeWeighted.clear();
      EventExecutionTimeWeighted.clear();

      // reset values
      forwardExecutionLength = 0;
      lastRollbackCount = rollbackCount;
      lastUserEventsCommitted = userEventsCommitted;
      
      setStatePeriod(period - 1);
    }
  }
}


inline void
NashStateManager::saveState() {
  if (timeAtLastCall != current->lVT) {
    forwardExecutionLength++;
  }
  AdaptStateManager::saveState();
}


inline VTime
NashStateManager::restoreState(VTime restoreTime) {
  VTime tmp;
  tmp = StateManager::restoreState(restoreTime);
  rollbackLength.update(statesRolledBack);
//  cout << "Called NashStateManager::restoreState StatesRolledBack = " 
//       << statesRolledBack << " Avg Rollbacklength ="  
//       << rollbackLength.getAvg()
//       << "\n";
  return(tmp);
}
#endif
