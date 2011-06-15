// -*-c++-*-
#ifndef STATEMANAGER_CC
#define STATEMANAGER_CC
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
// $Id: StateManager.cc,v 1.12 1999/10/24 23:26:19 ramanan Exp $
//
//---------------------------------------------------------------------------

#include <new.h>
#include "StateManager.hh"
#include "BasicTimeWarp.hh"

// NEW_STATE_MANAGEMENT

// With the new style of state management fully in place the default  
// constructor should be stripped off and this constructor will be the new
// interface to StateManager. Ofcourse the template should go too.

StateManager::StateManager(BasicTimeWarp *myProcessPtr) {
  processPointer = myProcessPtr;
  current        = NULL;
  nameOfObject   = NULL;
  memoryUseage   = 0;
  maxQsize       = 0;
  statesRolledBack = 0;
  returnMyLastOutput = true;
}

StateManager::~StateManager() {
  if(nameOfObject != NULL) 
    delete [] nameOfObject;	  // delete the name
  delete current;		  // delete current State
  gcollect(PINFINITY);		  // delete all but last stateQ element
  
  stateQ.setCurrent(stateQ.getHead());
  
  delete stateQ.removeCurrent();  // remove & delete last element from stateQ
}

VTime
StateManager::restoreState(VTime restoreTime) {
  BasicState *tmpState;
  // first find the closest state in the state queue
  tmpState = stateQ.find(restoreTime, LESS);
  if (tmpState != NULL) { // found a replacement state
    // current needs to be a _copy_, not a pointer assignment
    // *(current) =  *(UserState *)tmpState;
    current->copyState(tmpState);
  }
  else {
    stateQ.print();
    return PINFINITY;
  }

  stateQ.setCurrentToFind();
  removeStatesAfterCurrent();
  
  //this is done, because removeStatesAfterCurrent alters currentPos
  // can be removed - sanity check
  stateQ.setCurrentToFind();
  
  return current->lVT;
}

void 
StateManager::removeStatesAfterCurrent() {
  BasicState *tmpState;
  int counter = 0;

  // this is definitely non-optimal
  tmpState = stateQ.seek(1,CURRENT);
  
  // int Qsize = stateQ.size();

  while(tmpState != NULL){
    // this will advance currentPos one state forward
    stateQ.removeCurrent();

    delete tmpState;
    // look at the next state...
    tmpState = stateQ.get();
    counter++;
  }
  
  // This is for optimistic fossil collection and adaptive state savings
  statesRolledBack = counter;
}

void 
StateManager::saveState() {
  BasicState* tempState;
#ifdef LPDEBUG
  *lpFile << nameOfObject << " Saved state with lVT: " 
	  << current->lVT << endl;
#endif
  
  // This is the way the state manager will be allocating new state henceforth
  // for now, since TimeWarp is templatized its allocateState creates a new
  // UserState and returns it to us. Later when the templates are stripped off
  // the user's process should overload this function do allocate a new state
  
  tempState = (BasicState *) getProcessPointer()->allocateState();
  
  // The copyState function should copy the BasicState* pointer passed
  // to it to itself. Ofcourse it has type cast it to the corresponding type
  // of the state in the corresponding overloaded method.
  // As of now, this function simply calls operator=()
  
  tempState->copyState(current);
  
  stateQ.insert(tempState);
}

void
StateManager::clear(){
  stateQ.clear();
}

VTime
StateManager::gcollect(VTime gtime, BasicEvent*& inputQptr,
		       Container<BasicEvent>*& outputQptr) {
  //first we gcollect for stateq and also get back appropriately initialized
  // intputQPtr and outputQPtr . then since we want to call clear function
  // of statequeue to null out the outputPos pointers of the head elements
  // of the statequeue before gcollecting for outputQ we store the appropriate
  // outputPos value in returnMyLastOutput. 
  
  int Qsize = stateQ.size();
  if (Qsize > maxQsize) {
    maxQsize = Qsize;
  }

  VTime returnTime = stateQ.gcollect(gtime, inputQptr, outputQptr) ;

  returnMyLastOutput = stateQ.getHead()->myLastOutput;
  
  return returnTime ;
}

VTime 
StateManager::timeOfStateFromCurrent(int statesFromCurrent) {

  return stateQ.timeOfStateFromCurrent(statesFromCurrent);
}

// This function is needed for the new style of state management
// This function will be called by timeWarpInit() to allocate the
// initial state for the process in the system.

void
StateManager::createInitialState() {
  current        = (BasicState *) getProcessPointer()->allocateState();
  current->dirty = false;
  current->lVT   = ZERO;
}

int
StateManager::queueSize() {
  return stateQ.size();
}

int
StateManager::getStatePeriod() {
  return -1;
}

VTime
StateManager::gcollect(VTime) {
  return ZERO;
}

bool
StateManager::useTimeForGcollect() {
  return false;
}

void
StateManager::clearHead() {
  stateQ.getHead()->outputPos    = NULL;
  stateQ.getHead()->myLastOutput = false;
}

void
StateManager::setTail(Container<BasicEvent>* outpos, bool lastOutput) {
  stateQ.getTail()->outputPos = outpos;
  stateQ.getHead()->myLastOutput = lastOutput;
}

#ifdef STATEDEBUG
//Do not remove - Vital error checking function
void StateManager::checkEvent(Container<BasicEvent> *checkEvent, int flag){
  BasicState *tempState;
  tempState = stateQ.getHead();
  while(tempState != NULL){
    if(tempState->outputPos != NULL){
      if(tempState->outputPos == checkEvent){
	// checking outputPos in current state to first state
	if(tempState->outputPos != stateQ.getHead()->outputPos){
	  cout << "Head of State Queue at " << stateQ.getHead()->lVT 
	       << endl;
	  switch(flag){
	  case 1:
	    cout << "Anti-message for outputPos message at " << endl;
	    cout << "Referenced in state at " << tempState->lVT 
		 << " to " << *checkEvent->object << endl;
	    break;
	  case 2:
	    cout << "Warning -- Garbage collect outputPos message" 
		 << endl;
	    cout << "Referenced in state at " << tempState->lVT 
		 << " to " << *checkEvent->object << endl;
	    break;
	  default:
	    cout << "Scooby dooo !!" << endl;
	  }
	}
      }
    }
    tempState = tempState->next;
  }
}
#endif

void
StateManager::setFile(ofstream *outfile) { 
  lpFile = outfile; 
  stateQ.setStateFile(outfile);
  stateQ.setFile(outfile);
}

VTime
StateManager::findMinimumState(const VTime &gTime) {
  BasicState *lastkeep;

  if ((lastkeep = stateQ.find(gTime, LESS)) != NULL) {
    return lastkeep->lVT;
  }

  return stateQ.getHead()->lVT;
}

#endif
