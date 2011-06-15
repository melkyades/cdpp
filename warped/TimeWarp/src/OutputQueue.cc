//-*-c++-*-
#ifndef OUTPUT_QUEUE_CC
#define OUTPUT_QUEUE_CC
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
// $Id: OutputQueue.cc,v 1.11 1999/10/25 14:23:23 ramanan Exp $
//
// 
//
//---------------------------------------------------------------------------

#include "OutputQueue.hh"
#include "BasicTimeWarp.hh"

OutputQueue::~OutputQueue() {}

void OutputQueue::reset() {
  currentPos = NULL;
  insertPos = NULL;
  findPos = NULL;
  head = NULL;
  tail = NULL;
  listsize = 0;
}

BasicEvent*
OutputQueue::find(VTime findTime, findMode_t mode) {
  static BasicEvent key;
  BasicEvent* findEvent;
  
  key.sendTime = findTime;
  findEvent = SortedList<BasicEvent>::find(&key, mode );
  return(findEvent);
}


void
OutputQueue::gcollect(VTime gtime, ObjectRecord* simArray) {
  BasicEvent *delptr;

  saveCurrent();

  delptr = seek(0, START);
	
  while (delptr != NULL && delptr->sendTime < gtime) {
    // check to see if the event is local
    if(simArray[delptr->dest].ptr != NULL &&
	   simArray[delptr->dest].ptr->isCommManager()) {
      // this advances currentPos and deletes the container and 
      // the event since the event in non-local  
      delete [] (char *) removeCurrent(); 
    }
    else {
      // this event is local and will be deleted by the input queue
      // removeCurrent will delete only the container
      removeCurrent();
    }
    delptr = get();  // get new element at currentPos
  }

  restoreCurrent();
}


#ifdef STATEDEBUG
void
OutputQueue::gcollect(Container<BasicEvent>*& outputQptr,
		      ObjectRecord* simArray, bool myLastOutput,
		      StateManager* state) {
#else
  void
    OutputQueue::gcollect(VTime gtime, Container<BasicEvent>*& outputQptr,
			  ObjectRecord* simArray, bool myLastOutput){
#endif
    Container<BasicEvent>* temp;
    BasicEvent *delptr;
    
    if (outputQptr == NULL) {
      // Nothing to garbage collect ...
      // save time and get out right away ...
      return;
    }
    
    // save currentPos for later since we are going to modify it
    saveCurrent(); 
    
    delptr = seek(0, START);
    temp = getCurrent();
    
    while (temp != NULL && temp->object != NULL) {
      if(temp->object->sendTime <= gtime){
#ifdef STATEDEBUG
	state->checkEvent(getCurrent(), 2);
#endif
	
	// this advances currentPos, deletes container
	delptr = removeCurrent(); 
	
	// Events send to another LP are not in inputQ and therefore they have
	// to be handled/removed from here
	if(simArray[delptr->dest].ptr != NULL &&
	   simArray[delptr->dest].ptr->isCommManager()) {
	  delete [] ((char *)delptr);
	}
	delptr = get();  // get new element at currentPos
	
	if (temp == outputQptr) {
	  // If we have deleted upto AND INCLUDING outputQptr break out...
	  break;
	}
	
	temp = currentPos;
      }
      else {
	//break out of the while loop as we dont need to check further
	break;
      }
    }
    restoreCurrent(); // set currentPos back to what it was
    
}


#endif
