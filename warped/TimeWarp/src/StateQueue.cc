//-*-c++-*-
#ifndef STATE_QUEUE_CC
#define STATE_QUEUE_CC
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
// $Id: StateQueue.cc,v 1.7 1999/10/25 14:23:23 ramanan Exp $
//
// 
//
//---------------------------------------------------------------------------
#include "StateQueue.hh"
#include "SimulationTime.hh"
#include "config.hh"

    
    // The following insert function redefines the LTSFInputQueue::insert 
    // function . States with the same lVT are now inserted away from
    // the head of the queue in the order in which their input msg was
    // processed. That is given a set of states with the same lVT the
    // one farthest from the head will be the one last processed.
    // Comparison of states and search of the queue have also been eliminated
    // in this redefinition. The states are simply added to the tail each
    // time insert is called.

void
StateQueue::insert(BasicState* newelem)
{
  if(newelem != NULL)
    {
      if(listsize == 0)
	{
	  head = newelem ;
	  tail = newelem ;
	  head->prev=NULL ;
	  tail->next=NULL ;
	  insertPos=newelem ;
	}
      else
	{
	  tail->next=newelem ;
	  newelem->prev=tail;
	  tail=newelem;
	  tail->next=NULL;
	  insertPos=newelem ;
	}
      listsize++ ;
    }
  else
    {
      cerr <<"Trying to insert NULL element in Statequeue !!!! " << endl ;
    }
}
      

BasicState*
StateQueue::find(VTime findTime, findMode_t mode = EQUAL ) {
  static BasicState key;
  BasicState* findState;

  key.lVT = findTime;

  findState = SortedListOfEvents < BasicState >::find(&key, mode);

  
  return(findState);
}


VTime
StateQueue::gcollect(VTime gtime, BasicEvent*& inputQptr,
		     Container<BasicEvent>*& outputQptr) { 
  BasicState *lastkeep;


  BasicState *delptr;
  BasicState *saveCurrent;
  
  // find the first state before gVT
  lastkeep = find(gtime, LESS);
  
  // no state found !
  if (lastkeep == NULL) {
    if(gtime == PINFINITY){ 
      // simulation end reached; return pointers to the last inputQ &
      // outputQ event.

      inputQptr = tail->inputPos;
      outputQptr = tail->outputPos;
      cout << "End of simulation reached with no states left " << endl;
      return PINFINITY;		
    }
    cout << "ERROR: StateQueue::gcollect--no states before gVT of " 
	 << gtime << "!" << endl;
    cout << "Dumping the state queue..." << endl;
    print();
    exit(-12);
  }
#ifdef STATEDEBUG
  *stateFile << "The last element in the state Queue before gtime" << *lastkeep
	     << endl; 
  *stateFile << " The outputQ element that it points to is " ;
  if (lastkeep != NULL && lastkeep->outputPos != NULL){
    *stateFile << *(lastkeep->outputPos->object) << endl;
  }
  else {
    *stateFile << " NULL" << endl;
  }
  *stateFile << endl;
#endif
  // Note: In the very first state in the stateQ, the outputPos actually 
  // points to the first output event of the next state, if it is not the 
  // initial state. 
  // In all other states, outputPos points to the last event in the current 
  // state
#if defined(INFREQSTATEMANAGER) || defined(LINSTATEMANAGER) || defined(NASHSTATEMANAGER) || defined(COSTFUNCSTATEMANAGER)
  outputQptr = NULL;
  inputQptr = NULL;
#else
  outputQptr = lastkeep->outputPos;
  inputQptr  = lastkeep->inputPos;
#endif

  lastkeep->outputPos = NULL;

if (outputQptr != NULL) {
    delptr              = lastkeep->next;
    while ((delptr != NULL) && (delptr->outputPos == outputQptr)) {
      delptr->outputPos    = NULL;
      delptr->myLastOutput = false;
      delptr = delptr->next;
    }
}

  

  saveCurrent = currentPos;
  delptr = seek(0, START);
  while (delptr != NULL && delptr != lastkeep) {
    
    delete removeCurrent(); // advance currentPos, delete container & state
    delptr = get();         // get new element at currentPos
  }
  currentPos = saveCurrent;
  
  return(lastkeep->lVT);
}

// this function resets the outputPos and myLastOutput variables
// in the state. This is basically because it could happen that
// more than one state could point to the same element in the 
// outputQueue. In this case, we let the first state point to the
// element and we null out the pointers in all other states that
// follow the first state.
void 
StateQueue::clear(){

  BasicState* delptr;
  Container<BasicEvent>* oldOutputPos;

  saveCurrent();
  delptr = seek(0, START);
  if((delptr != NULL) && (delptr->lVT != ZERO)){
    oldOutputPos = delptr->outputPos;
    while(currentPos != NULL && (currentPos->outputPos == oldOutputPos))
      { 
	currentPos->outputPos = NULL;
	// this indicates that NULL should be interpreted as first output 
	// event.
	currentPos->myLastOutput = false;
	currentPos = currentPos->next;
      }
  }
  restoreCurrent();

}

VTime
StateQueue::timeOfStateFromCurrent(int statesFromCurrent) {
  VTime time;


  BasicState* saveCurrent;

  
  BasicState* tmpPtr;
	
  time = PINFINITY;


  saveCurrent = getCurrent();


  tmpPtr = seek(-statesFromCurrent,END);
  if (tmpPtr != NULL) {
    time = tmpPtr->lVT;  
  }
  else {
    tmpPtr = seek(1,START);
    if (tmpPtr != NULL) {
      time = tmpPtr->lVT;
    }
  }


  setCurrent(saveCurrent);

  
  return time;

}

VTime
StateQueue::timeDiff() {
  VTime returnVal = INVALIDTIME;
  if (head != NULL && tail != NULL) {
    returnVal = tail->lVT - head->lVT;
  }
  return returnVal;
}

#endif









