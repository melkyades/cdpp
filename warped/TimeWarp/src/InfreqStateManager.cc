// -*-c++-*-
#ifndef INFREQSTATEMANAGER_CC
#define INFREQSTATEMANAGER_CC
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
// $Id: InfreqStateManager.cc,v 1.4 1999/10/24 23:25:57 ramanan Exp $
//
//
//---------------------------------------------------------------------------
#include "InfreqStateManager.hh"


inline
InfreqStateManager::InfreqStateManager(BasicTimeWarp *myProcessPtr):
  StateManager(myProcessPtr) {

  statePeriod = 3;
  periodCounter = 0;
  timeAtLastCall = INVALIDTIME;
}

void
InfreqStateManager::createInitialState() {
  StateManager::createInitialState();
  current->dirty = false;
}


void 
InfreqStateManager::saveState() {

#ifdef LPDEBUG
  *lpFile << nameOfObject << " timeAtLastCall: " << timeAtLastCall
	   << " lvt = " << current->lVT << "\n";
#endif

  if (timeAtLastCall != current->lVT) {

    if (periodCounter == 0) {
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


VTime 
InfreqStateManager::gcollect(VTime gtime, 
			     BasicEvent*& inputQptr,
			     Container<BasicEvent>*& outputQptr) {
  
  VTime tmpTime = stateQ.gcollect(gtime, inputQptr, outputQptr);
  
  if ( (stateQ.front() ) == NULL ) {
    cout << "StateQueue is NULL at gtime " << gtime << " for " 
	 << nameOfObject << endl;
  }
  if ( (stateQ.front() )->dirty == true) {
    // There are messages between the last saved state before gVT and  
    // gVT.
    return(tmpTime);
  }
  else {
    return(gtime);
  }
}

VTime 
InfreqStateManager::restoreState(VTime restoreTime) {
  VTime tmpTime = StateManager::restoreState(restoreTime);
  // reset time at last call so that another state is not saved at this
  // time.
  timeAtLastCall = tmpTime;
  return (tmpTime);
}


inline void 
InfreqStateManager::setStatePeriod(int period) {
  statePeriod = period;
#ifdef OBJECTDEBUG
  // This check needs to be done becuase the timewarp constructor calls
  // this function before logical process sets the pointer
  if (lpFile->good())  *lpFile << "period = " << period << "\n";
#endif
}

#endif


