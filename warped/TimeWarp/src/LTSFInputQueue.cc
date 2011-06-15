//-*-c++-*-
#ifndef LTSFINPUTQUEUE_CC
#define LTSFINPUTQUEUE_CC
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
// $Id: LTSFInputQueue.cc,v 1.10 1999/10/24 23:25:59 ramanan Exp $ 
//---------------------------------------------------------------------------
#include "LTSFInputQueue.hh"

LTSFInputQueue::LTSFInputQueue() {
	setFunc( BasicEventCompareRecv );
	initialized = false;
	noOfEventsCommitted = 0;
	memoryUseage = 0;
}

void
LTSFInputQueue::reset() {
	int i;
	initialized = false;
	listsize = 0;
  // initialize all listvalues
	head = NULL;
	tail = NULL;
	currentPos = NULL;
	insertPos = NULL;
	findPos = NULL;
	for (i = 0; i < numObjects; i++) {
		findObj[i] = insertObj[i] = currentObj[i] = NULL;
		headObj[i] = tailObj[i] = NULL;
		listSize[i] = 0;
		lVTArray[i] = ZERO;
		idleObjArray[i] = false;
	}
}

LTSFInputQueue::~LTSFInputQueue() {
}

inline void
LTSFInputQueue::miniListUnprocessMessages(BasicEvent* toInsert, int id) {
	
#ifdef ONE_ANTI_MESSAGE
	BasicEvent *deletePtr = NULL;
#endif
	
  // save the old pointers
	tmpCurrentPtr = currentPos;
	currentPos = currentObj[id];
	if (currentPos != NULL) {
		unprocessPtr = currentPos;
    // Set the item we inserted to true, and start unprocessing messages
    // (can't use currentPos because that points to the first message at
    // the same time instead of our message.)
		toInsert->alreadyProcessed = true;
	}
	else {
		unprocessPtr = NULL;
	}
	
#ifdef ONE_ANTI_MESSAGE
  // If this flag is set then we will get only one anti message for each
  // rollback. So go ahead and delete all the messages corresponding to
  // the object that sent the negative message.
	if ((toInsert->sign == NEGATIVE) && (unprocessPtr != NULL)) {
		int  negativeSourceId = toInsert->sender;
		
		BasicEvent* sanityPtr = unprocessPtr->prevObj;
		while (unprocessPtr != NULL) {
			deletePtr = unprocessPtr;
			unprocessPtr = unprocessPtr->nextObj;
			
			if (deletePtr->sender == negativeSourceId) {
				
				if (tmpCurrentPtr == deletePtr) {
					tmpCurrentPtr = tmpCurrentPtr->next;
				}
#ifdef LPDEBUG      
				*lpFile << "OAM-Scavenged : " << *deletePtr << endl;
#endif
				
				delete [] (char *)MultiList<BasicEvent>::remove(deletePtr, id);
			}
			else {
				deletePtr->alreadyProcessed = false;
			}
		}
		
		if (sanityPtr != NULL) {
			currentObj[id] = sanityPtr->nextObj;
		}
		else {
			currentObj[id] = headObj[id];
		}
#ifdef LPDEBUG      
		*lpFile << "OAM-Scavenged " << scavengeCount << endl;
		*lpFile << "OAM-Reset " << resetNumber << " events" << endl;
#endif    
	}
#endif
	
  // unprocess all messages until we get a message that hasn't been
  // processed, or we reach the end of the queue.
	while(unprocessPtr != NULL && 
			unprocessPtr->alreadyProcessed == true) {
		unprocessPtr->alreadyProcessed = false;
		currentPos = currentPos->nextObj;
		if (currentPos == NULL) {
			unprocessPtr = NULL;
		}
		else {
			unprocessPtr = currentPos;
		}
	}
	
  // reset alreadyProcessed flag in case the above didn't do it 
  // (this should not be necessary, but I'll check later).
	toInsert->alreadyProcessed = false;
	
	// reset currentPos;
	currentPos = tmpCurrentPtr;
	
}

inline bool
LTSFInputQueue::miniListRollback(BasicEvent *toInsert, int id) {
	bool inMiniListPast = false;
  // check to see if message is in minilist's past. In most cases lVTArray
  // will have the correct time of the last message executed, so this will
  // be used to determine rollbacks. There are some cases where this time
  // will be incorrect. This is for a negative message cancelling the last
  // message in the miniqueue[*] In this case the recvTime of the antimessage
  // will be taken as the last time executed. This can cause another rollback
  // which will be unnecessary if another message is received with a
  // a timestamp less than the antimessage and greater than the state
  // restored, but processing will still be correct.
  // [*] - the miniqueue is an abstraction of the multiqueue where each
  // destination in the multiqueue has a miniqueue that contains just
  // the messages for that destination. All pointers that end with "Obj"
  // are referring to the miniqueue.
	
  // !!!!!! 23/01/2001 Originalmente: toInsert->recvTime <= lVTArray[id]
  // Ahora cambia el <= por el < para poder recibir mensajes simultaneos.
	
	if ( toInsert->recvTime < lVTArray[id] ) {
		if (toInsert->sign == POSITIVE) {
			MultiList<BasicEvent>::find(toInsert, EQUAL, id);
			currentObj[id] = findPos;
		}
		else {
			MultiList<BasicEvent>::find(toInsert, GREATEREQUAL, id);
			currentObj[id] = findPos;
		}
		
    // Now we need to unprocess some messages from the miniqueue that is
    // being rolledback.
		
		miniListUnprocessMessages(toInsert, id);
		
    // This is the state that is going to be restored, so lets reset
    // our lVT value to it since any future message should not cause
    // a rollback except if it's before this time.
		if (currentObj[id] != NULL) {
			if (currentObj[id]->prevObj != NULL) {
				lVTArray[id] = currentObj[id]->prevObj->recvTime;
			}
			else {
				lVTArray[id] = currentObj[id]->recvTime;
			}
		}
		else {
      // currentObj is NULL so lets see if we have any processed messages
      // in our list, and if so then let's use them to determine the time
      // otherwise use the recieve time of the message.
			if (tailObj[id] == NULL) {
	// This is the time we are going to rollback to even if this is
	// a negative message.
				lVTArray[id] = lastGVT;
			}
			else {
				if (tailObj[id]->alreadyProcessed == true) {
					lVTArray[id] = tailObj[id]->recvTime;
				}
				else {
					lVTArray[id] = lastGVT;
				}
				
			}
		}
		
		inMiniListPast = true;
	}
	else {
    // we had no events and now we have one, or we have a new one
    // before currentObj, so reset currentObj to point to the new mesage
    // for positive messages, or to any message at or after the message 
    // for a negative message.
		if ( currentObj[id] == NULL ||
				currentObj[id] != NULL &&
				(toInsert->recvTime < currentObj[id]->recvTime ||
					(toInsert->recvTime == currentObj[id]->recvTime &&
	   // "<=" has been set due to reasons explained in insert()
						toInsert->dest <= currentObj[id]->dest) ) ) {
			if (toInsert->sign == POSITIVE) {
				currentObj[id] = insertObj[id]; 
			}
			else {
				MultiList<BasicEvent>::find(toInsert, GREATEREQUAL, id);
				currentObj[id] = findPos;
#ifdef ONE_ANTI_MESSAGE	
				miniListUnprocessMessages(toInsert, id);
#endif		
			}
		}
#ifdef ONE_ANTI_MESSAGE    
		else {
      // cout << "The specical case we found just now" << endl;
			if(toInsert->sign == NEGATIVE){
				BasicEvent *oldCurrentPos = currentObj[id];
				
				MultiList<BasicEvent>::find(toInsert, GREATEREQUAL, id);
				currentObj[id] = findPos;
				
				miniListUnprocessMessages(toInsert, id);
				currentObj[id] = oldCurrentPos;
			}
		}
#endif    
	}
	return inMiniListPast;
}

VTime
LTSFInputQueue::getLVTForThisObj(int id){
	BasicEvent* unProcessMsg;
	unProcessMsg = headObj[id];
	if(unProcessMsg->alreadyProcessed == false){
		return(unProcessMsg->recvTime);
	}
	else{
		while(unProcessMsg->alreadyProcessed == true){
			unProcessMsg = unProcessMsg->nextObj;
		}
		return(unProcessMsg->prevObj->recvTime);
	}
}

bool
LTSFInputQueue::insert(BasicEvent *toInsert, int id) {
	register BasicEvent *objectPtr;
	bool msgCancel = false;
	bool inThePast = false;
  // bool implodeProcessedFlag = false;
	
  // insert performs two functions: 1) insert (or implode) the element
  // into both the main and appropriate minilists.  2) figure out if the
  // miniList should be rolledback. 3) figure out where currentPos now 
  // belongs. 1) is simple. 2) is harder. 3) is very tricky. This code
  // here performs 1).
	if (toInsert->sign == POSITIVE) {
    // if the sign is positive, we'll just insert it have fifo messaging,
    // there are no fast antimessages to implode with.
		MultiList<BasicEvent>::insert(toInsert, id);
		lastInserted = toInsert;
	}
	else { // inserting a negative event--will implode!
    // look for first element with same time
		objectPtr = MultiList<BasicEvent>::find(toInsert, EQUAL, id);
		
    // walk through events with same time looking for implosion or duplicate
		while (msgCancel == false && objectPtr != NULL) {
			if ((objectPtr->eventId == toInsert->eventId)  &&
					(objectPtr->sender == toInsert->sender)) { // implosion
				if (objectPtr->sign != toInsert->sign) {
	  // we have to remember whether we have executed the guy we just
	  // imploded in order to decide whether to rollback or not.
	  //implodeProcessedFlag = objectPtr->alreadyProcessed;
					
					delete [] ((char *) MultiList<BasicEvent>::removeFind(id));
					msgCancel = true;
				}
				else { // duplicate message--oops!
					cout << "Error: duplicate negevent received-aborting" << endl;
					abort();
				}
			}
			
      // Multilist moves currObj and insertObj if they point to the last
      // element and it's deleted. This screws up currObj[id], so this
      // line checks and fixes it if necessary.
			if (currentObj[id] != NULL && currentObj[id] == tailObj[id] && 
					currentObj[id]->alreadyProcessed == true) {
				currentObj[id] = NULL;
			}
			objectPtr = MultiList< BasicEvent >::findNext(id);
		} // while
		
		if (msgCancel == false) {
#ifdef STATEDEBUG
			cout << " Inside the printing of the fast antimessage problem " << endl;
			*outFile << "LTSFInputQ: fast antimessage found!\n";
			cout << toInsert << " " << *toInsert << "\n";
			print(*outFile);
			abort();
#else
			cout << "LTSFInputQ: fast antimessage found!" << endl;
			cout << toInsert << " " << *toInsert << "\n";
			print();
			abort();
#endif
			
		}
	} // if toInsert->sign == POSITIVE
	
  // Perform step 2: Check if the miniQueue should be rolledback
	inThePast = miniListRollback(toInsert,id);
#ifdef STATEDEBUG
//   if (inThePast == true) {
//     printSmallQ(*outFile, id);
//     *outFile << "The lvt of this object is " << lVTArray[id] << endl;
//     if (currentObj[id] != NULL){
//       *outFile << *currentObj[id] << endl;
//     }
//     else {
//       *outFile << "The current Object now is null" << endl;
//     }
//   }
#endif
  //if currentPos is NULL, the list is either empty, or everything left
  // in it has been processed. This means a positive or negative will
  // rollback one queue and the first unprocessed message in that queue
  // should be processed next.
	if (currentPos == NULL) {
		currentPos = currentObj[id];
	}
	else {
		
		if (toInsert->sign == NEGATIVE) {
      // For a negative message in the past, if the next message to execute
      // in the miniqueue is before currentPos then we should reset 
      // currentPos.
      // NOTE: The paraenthesis around the third part of the expression
      // are necessary since && has higher precedence than || so do not
      // remove them!
			if (inThePast == true && currentObj[id] != NULL &&
					(currentObj[id]->recvTime < currentPos->recvTime ||
						currentObj[id]->recvTime == currentPos->recvTime &&
	   // Here "<=" implies that our assumption is that
	   // when we get events at the same time and for the
	   // same destination, we insert before the first such
	   // instance of this event at this time and dest.
	   // Refer to MultiList::insert
						//!!!! 26/01/2001
						currentObj[id]->dest < currentPos->dest) ) {
				currentPos = currentObj[id];
			}
		}
		else {
      // Positive message, so we adjust the current pointer
      // to this message if it's recieve time is less than the current's,
      // or at the same time with a destination less than the current's
      // (thus in current's past).
			if (currentObj[id] != NULL &&
					(currentObj[id]->recvTime < currentPos->recvTime ||
						(currentObj[id]->recvTime == currentPos->recvTime &&
	    // Here "<=" implies that our assumption is that
	    // when we get events at the same time and for the
	    // same destination, we insert before the first such
	    // instance of this event at this time and dest.
	    // Refer to MultiList::insert	
	    // !!!! 26/01/2001
							currentObj[id]->dest < currentPos->dest)) ) {
				currentPos = currentObj[id];
			}
		}
	}
	if(msgCancel == true){
		
    // message implosion - delete event
		delete [] ((char *)toInsert);
	}
	
	return inThePast;
}

BasicEvent*
LTSFInputQueue::find(VTime& findTime, int objId, findMode_t mode, int localId){
	static BasicEvent key;
	BasicEvent* findEvent;
	
	key.recvTime = findTime;
	key.dest = objId;
	findEvent = MultiList<BasicEvent>::find(&key, mode, localId);
	return(findEvent);
}

BasicEvent*
LTSFInputQueue::getEventToSchedule() {
	BasicEvent* nextEvent = get(); 
	
	while ((nextEvent != NULL) && (nextEvent->alreadyProcessed == true)) {
		nextEvent = seek(1,CURRENT);
	}
	
	return nextEvent;
}

BasicEvent*
LTSFInputQueue::getAndSeek(int id, int localId) {
  // note: this is the object id, not the localId relevant to the MultiQueue
	BasicEvent *toExecute = get();
	
  // toExecute is now either NULL or a valid + message
	if (toExecute != NULL && toExecute->dest == id) {
    // move to the next message, but not if the id changes
		toExecute->alreadyProcessed = true; // needed for VHDL
		if(currentObj[localId] != NULL){
			currentObj[localId] = currentObj[localId]->nextObj;
		}
		lVTArray[localId] = toExecute->recvTime;
		seek(1, CURRENT);
	}
	else {
		toExecute = NULL;
	}
	return toExecute;
}

BasicEvent*
LTSFInputQueue::getAndSeekObj(int id, int localId) {
  // note: this is the object id, not the localId relevant to the MultiQueue
	BasicEvent *toExecute = getObj(localId);
	
  // toExecute is now either NULL or a valid + message
	if (toExecute != NULL && toExecute->dest == id) {
		toExecute->alreadyProcessed = true; // needed for VHDL
		if(currentObj[localId] != NULL){
			currentObj[localId] = currentObj[localId]->nextObj;
		}
		lVTArray[localId] = toExecute->recvTime;
	}
	else {
		toExecute = NULL;
	}
	return toExecute;
}


int
LTSFInputQueue::gcollect(VTime& gtime, int localId) {
	register BasicEvent *delptr, *tempPtr;
	int nodeleted = 0;
	
	delptr = headObj[localId];
	if (delptr != NULL) {
		while (delptr != NULL && delptr->recvTime < gtime) {
			noOfEventsCommitted++;
			nodeleted++;
      // InputQ sanity check to see that we have processed all messages that
      // we are deleting. This line also contains a workaround for Optimistic
      // Fossil Collection which may need to remove everything from the
      // Queue regardless if it has been processed or not.
			
			if(delptr->alreadyProcessed == 0) {
				cout << "Attempt to delete unprocessed messages!" << endl;
			}
			tempPtr = delptr->nextObj;
			
			MultiList<BasicEvent>::remove(delptr, localId);
			
			delete [] ((char *)delptr);
			delptr = tempPtr;
		}
		return(noOfEventsCommitted);
	}
	else {
		return 0;
	}
	
}



int
LTSFInputQueue::gcollect(VTime gtime, int localId, BasicEvent* ptrMarker) {
	
	register BasicEvent *delptr, *tempPtr;
	
	
	int nodeleted = 0;
  // The inputPointer passed in is the point up to garbage has to be collected
  // Starting from the head garbage collect all objects up to this point. 
  // If the pointer passed in is null do not garbage collect anything. 
	
	delptr = headObj[localId];
	if ((ptrMarker != NULL) || 
			(ptrMarker == NULL)) {
		while (delptr != NULL && 
				delptr != ptrMarker && 
				delptr->recvTime < gtime){
			noOfEventsCommitted++;
			nodeleted++;
      // InputQ sanity check to see that we have processed all messages that
      // we are deleting. This line also contains a workaround for Optimistic
      // Fossil Collection which may need to remove everything from the
      // Queue regardless if it has been processed or not.
			
			if(delptr->alreadyProcessed == 0) {
				cout << "Attempt to delete unprocessed messages!" << endl;
				cout << *delptr << endl;
				print();
				abort();
			}
			tempPtr = delptr->nextObj;
			
			MultiList<BasicEvent>::remove(delptr, localId);
			
			delete [] ((char *)delptr);
			delptr = tempPtr;
		}
		return(noOfEventsCommitted);
	}
	else {
		return 0;
	}
}

ostream&
operator << (ostream& os, const LTSFInputQueue& Iq) {
	Iq.print(os);
	return os;
}

void
LTSFInputQueue::print(ostream& out) const {
	out << "\n\nLTSFInputQueue print called\n";
	out << "total list length is " << listsize << "\n";
	SortedListOfEvents<BasicEvent>::print(out);
}


void
LTSFInputQueue::printMiniList(int i) {
  //cout << "\n\nLTSFInputQueue print called\n";
  //cout << "total list length is " << listsize << "\n";
	
	cout << "MiniList " << i << ": length = " << listSize[i] << "\n";
	if (headObj[i] != NULL) {
		cout << "HeadObj = " << headObj[i] << " " 
			<< *headObj[i] << "\n";
	}
	if (tailObj[i] != NULL) {
		cout << "TailObj = " << tailObj[i] << " " 
			<< *tailObj[i] << "\n";
	}
	if (findObj[i] != NULL) {
		cout << "FindObj = " << findObj[i] << " " 
			<< *findObj[i] << "\n";
	}
	if (insertObj[i] != NULL) {
		cout << "InsertObj = " << insertObj[i] << " " 
			<< *insertObj[i] << "\n";
	}
	if (currentObj[i] != NULL) {
		cout << "CurrentObj = " << currentObj[i] << " " 
			<< *currentObj[i] << "\n";
	}
	printSmallQ(cout,i);
	cout.flush();
	
}

#endif // LTSFINPUTQUEUE_CC

