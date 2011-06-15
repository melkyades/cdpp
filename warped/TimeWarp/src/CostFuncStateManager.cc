// -*-c++-*-
#ifndef COSTFUNCSTATEMANAGER_CC
#define COSTFUNCSTATEMANAGER_CC
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
// $Id: CostFuncStateManager.cc,v 1.4 1999/10/25 14:23:22 ramanan Exp $
//
//
//---------------------------------------------------------------------------

#include "CostFuncStateManager.hh"


CostFuncStateManager::CostFuncStateManager(BasicTimeWarp *myProcessPtr) :
  AdaptStateManager(myProcessPtr) {

  eventsBetweenRecalc = 1000;
  forwardExecutionLength = 0;
  adaptionValue = 1;
  oldCostIndex = 0.0;
  filteredCostIndex = 0.0;
}


inline void
CostFuncStateManager::setAdaptiveParameters(long eventsbeforerecalcuate,
					    long dummy) {

  eventsBetweenRecalc = eventsbeforerecalcuate;
}


inline void
CostFuncStateManager::committedEvents(long eventsCommitted, 
				      int rollbackCount) {
  lastRollbackCount = rollbackCount;
}


void
CostFuncStateManager::calculatePeriod() {

  int period = getStatePeriod();
  double costIndex, coastTime, stateTime;
  stateTime = StateSaveTimeWeighted.getAvg();
  coastTime = CoastForwardTimeWeighted.getAvg();
  costIndex = stateTime + coastTime;

//  if ( (filteredCostIndex) && (costIndex > 0) && 
//       (costIndex < 10.0 * filteredCostIndex) ) {
    filteredCostIndex = 0.4 * filteredCostIndex + 0.6 * costIndex;
//  } else if ( filteredCostIndex == 0 ) {
//    filteredCostIndex = costIndex;
//  }

  if (oldCostIndex) {
    if (oldCostIndex > 1.2 * filteredCostIndex ) {
      period += adaptionValue;
      oldCostIndex = filteredCostIndex;
    } else if ( oldCostIndex < 0.8 * filteredCostIndex ) {
      adaptionValue = -adaptionValue;
      period += adaptionValue;
      oldCostIndex = filteredCostIndex;
    }
    if ( coastTime == 0 ) {
      adaptionValue = 1;
      period++;
      oldCostIndex = filteredCostIndex;
    }
  } else {
    period++;
    oldCostIndex = filteredCostIndex;
  }

  if (period < 1) {
    period = 1;
  } else if (period > MAXDEFAULTINTERVAL) {
    period = MAXDEFAULTINTERVAL;
  }

  if (lastRollbackCount == 0) period = MAXDEFAULTINTERVAL;
      
#ifdef OBJECTDEBUG
  *lpFile << "period = " << period - 1 << "\n";
#endif

  // reset values
  forwardExecutionLength = 0;
  StateSaveTimeWeighted.clear();
  CoastForwardTimeWeighted.clear();
      
  setStatePeriod(period - 1);
}


inline void
CostFuncStateManager::coastForwardTiming( double coastforwardtime ) {
  CoastForwardTimeWeighted.update(coastforwardtime);
}


inline void
CostFuncStateManager::saveState() {
  // Here we assume that everytime save state is called then we have executed
  // an event.
  if (timeAtLastCall != current->lVT) {
    forwardExecutionLength++;
  }
  if (forwardExecutionLength > eventsBetweenRecalc) {
    calculatePeriod();
    steadyState = true;
  }
  AdaptStateManager::saveState();
}
#endif
