//-*-c++-*-
#ifndef TIMEWARP_CC
#define TIMEWARP_CC
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
// $Id: TimeWarp.cc,v 1.23 1999/10/25 14:23:24 ramanan Exp $
//
//---------------------------------------------------------------------------

#include "LogicalProcess.hh"
#include "warpedNewHandler.hh"
#include "SortedList.hh"
#include <math.h>

TimeWarp::TimeWarp() : stdoutFileQ(FileQueue::standardOut), MAXDIFF(1000) {

  state          = new STATE_MANAGER(this);

  numInFiles     = 0;
  inFileQ        = NULL;
  numOutFiles    = 0;
  outFileQ       = NULL;
  eventCounter   = 0;
  rollbackCount  = 0;

  printIt        = false;
  
  // CurrentState gets set in the StateManager constructor
  
  set_new_handler(warpedNewHandler);
  timeWindow      = PINFINITY;

  suppressMessage = NONE; // this is the default

  timingsNeeded   = true;

#ifdef MATTERNGVTMANAGER
  color                = 0; // color initialized to white
  currentColorMessages = 0; // number of white messages initialized to zero
#endif

#ifdef ONE_ANTI_MESSAGE
  alreadySentAntiMessage = NULL;
#endif    

}

void
TimeWarp::finalGarbageCollectOutputQueue() {
  Container<BasicEvent>* outputQptr = NULL;
  BasicEvent*            inputQptr  = NULL;
  stateGcollect(PINFINITY, inputQptr, outputQptr);
  outputGcollect(PINFINITY, outputQptr);
}

void
TimeWarp::finalGarbageCollect()  {
  BasicEvent* inputQptr             = NULL;
  
  inputGcollect(PINFINITY, inputQptr);
#ifdef STATS
  //collect stats
  lpHandle->lpStats.setTotalEventsCommitted(inputQ.getNumberOfCommittedEvents());
#endif
  
  // garbage collect file queues and close files
  register int i;
  for(i = 0; (i < numInFiles); i++)  {
    inFileQ[i].gcollect(PINFINITY);
  }
  
  if (inFileQ != NULL) {
    delete [] inFileQ;
    inFileQ = NULL;
  }
  
  for(i = 0; (i < numOutFiles); i++) {
    outFileQ[i].gcollect(PINFINITY);
  }
  
  if (outFileQ != NULL) {
    delete [] outFileQ;
    outFileQ = NULL;
  }
  
  stdoutFileQ.gcollect(PINFINITY);

}
  
TimeWarp::~TimeWarp()  {

#ifdef REUSESTATS
  Container<BasicEvent> dummy;
  cout << " " << name;
  dummy.printReuseStats(cout);
#endif

#if defined(INFREQSTATEMANAGER) || defined(LINSTATEMANAGER) || defined(NASHSTATEMANAGER) || defined(COSTFUNCSTATEMANAGER)
#ifdef STATS
  *fileHandle << name << " state period = " << state->getStatePeriod() << endl;
#endif
#endif
  
#ifdef OBJECTDEBUG
  *lpFile << "all done!" << endl;
  outFile.close();
#endif


#ifdef DC_DEBUG
  dcFile.close();
#endif

#ifdef STATEDEBUG
  *lpFile2 << " After garbage collection for PINFINITY (object) " << id 
	   << endl;
  outputQ.print(*lpFile2);
#endif
  
#ifdef STATEDEBUG
  *lpFile2 << " After garbage collection for PINFINITY (object) " << id 
	   << endl;
  outputQ.print(*lpFile2);
#endif

#ifdef ONE_ANTI_MESSAGE
  delete [] alreadySentAntiMessage;
#endif

  delete state;
  if (name != NULL) {
    delete [] name;
    name = NULL;
  }
  
}


BasicEvent*
TimeWarp::getEvent()  {

#ifdef MESSAGE_AGGREGATION
#ifndef ADAPTIVE_AGGREGATION
  lpHandle->incrementAgeOfAggregatedMessage();
#endif
#endif

  BasicEvent *toExecute;
  

  // if a rollback needs to be performed, it should have happened before now

  // this is the object ID, and not the localID used by the singleQ.
  toExecute = inputQ.getAndSeek(id, localId);
  
  // toExecute is now either NULL or a valid message
  
  if (toExecute != NULL) {

#ifdef STATS
    // collect stats
    lpHandle->simObjArray[localId].incrementEventsProcessed();
    lpHandle->lpStats.incrementTotalEventsProcessed();
#endif
    state->current->lVT = toExecute->recvTime;
  }
  
  return(toExecute);
}


void
TimeWarp::sendEvent(BasicEvent* toSend )  {
  bool suppressDecision;
  
  FILL_EVENT_INFO(toSend);
  
  if (suppressMessage == NONE) { 
    suppressDecision = false;
  }
  else if (suppressMessage == COASTFORWARD) {
    suppressDecision = true;
  }
  else { 
    suppressDecision = LAZY_CANCEL_TEST(toSend); 
  }

  if(suppressDecision == false) {
    sendEventUnconditionally(toSend);
  }
  else {
    delete [] (char *) toSend;
  }
}


void
TimeWarp::fillEventInfo(BasicEvent* eventToSend ) {
  eventToSend->sendTime = state->current->lVT;
  eventToSend->sender   = id;
  eventToSend->alreadyProcessed = false;
}


void
TimeWarp::sendEventUnconditionally(BasicEvent* toSend) {
  int receiver = toSend->dest;

#ifdef MATTERNGVTMANAGER
  toSend->color = color;
  if(toSend->dest != toSend->sender){
    if(color == 0){
      whiteMessages++;
    }
    else{
      redMessages++;
      tMin = MIN_FUNC(tMin,state->current->lVT); 
    }
  } 
#endif

  if(toSend->sign == POSITIVE){

#ifdef STATS
    //collect stats
    lpHandle->simObjArray[localId].incrementPosSent();
#endif
    
    // finish filling in the event to send
    toSend->sendTime = state->current->lVT;
    toSend->eventId  = eventCounter;
    toSend->sender   = id;
    toSend->alreadyProcessed = false;

    // increment our event counter
    eventCounter++;

    // we now have all the info we need.  We want to send the positive message
    // for real, and put it on the outputQ in case of rollback

    // grab the communication handle of the receiving object and send message
    // send the positve message
    
#ifdef LPDEBUG
    *lpFile << name << ":sending : " << *toSend << endl;
#endif
    
#ifdef DC_DEBUG
    dcFile << name << ":sending : " << *toSend << endl;
#endif

    // Hack Hack
    // pause event timing because we don't want to take into account the
    // time required to rollback a process on the same LP. (This is due
    // to recvEvent being called for TW processes on the same LP instead
    // of CommManager::recvEvent() ).
    state->pauseEventTiming();
    commHandle[receiver].ptr->recvEvent(toSend);
    state->resumeEventTiming();

    // put the positive message on the output queue
    outputQ.insert(toSend);

  }
  else if( toSend->sign == NEGATIVE ) {

#ifdef STATS
    // collect stats
    lpHandle->simObjArray[localId].incrementNegSent();
#endif

    // msg has been sent to us from the outputQ, and the sign has already
    // been set to negative.  We just sent it off.
    
    // grab the communication handle of the receiving object and send message
#ifdef LPDEBUG
    *lpFile << name << ":sending : " << *toSend << endl;
#endif
    
    // Hack Hack
    // pause event timing because we don't want to take into account the
    // time required to rollback a process on the same LP. (This is due
    // to recvEvent being called for TW processes on the same LP instead
    // of CommManager::recvEvent() ).
    state->pauseEventTiming();
    commHandle[receiver].ptr->recvEvent(toSend);
    state->resumeEventTiming();
  }
  else {
    cerr << name << " sending messages with unknown sign! : "
	 << *toSend << endl;
  }
}

void 
TimeWarp::executeSimulation() {

  if (timingsNeeded) {
    state->startEventTiming();
    simulate();
    timingsNeeded = state->stopEventTiming();
  }
  else {
    simulate();
  }
  
  saveState();
}


void 
TimeWarp::simulate(){

  VTime preExecute = getLVT();
  int i;

  // inFile stuff--save the position in file before execution.  We need to
  // look into the inputQ to determine what time we are about to execute
  // at, and the inFileQ to determine where we are before execution.
  
  for (i = 0; i < numInFiles; i++) {
    inFileQ[i].storePos(inputQ.get()->recvTime,
			inFileQ[i].access().tellg());
  }

  state->current->inputPos = inputQ.getCurrent();

  executeProcess();
  
#ifdef LPDEBUG
  *lpFile << name << ": Finished running timestamp " << getLVT() 
	  << " ";
  state->current->print(*lpFile);
  *lpFile << "\n";
#endif
  
  if (preExecute != getLVT() ) {
    iRanLastTime = true;
  }
  
  // Flush the lazyCancelQ in case we have messages that are never regenerated
#ifdef LAZYCANCELLATION
  if((inputQ.testCurrentObj(this->localId) == false) 
     && (lazyCancelQ.size() != 0)) {
   
    // send out negative messages
    BasicEvent *cancelEvent, *negEvent;
    cancelEvent = lazyCancelQ.seek(0, START);

#ifdef LPDEBUG
    if((cancelEvent) && (cancelEvent->sendTime < rollbackTime)){
      *lpFile << name << ":found : " << *cancelEvent << "\n"
	      << name << ":rollbackTime : " << rollbackTime << endl;
    }
#endif

    while ((cancelEvent != NULL) &&
	   (cancelEvent->sendTime < state->current->lVT)) {
      // the outputQ has the actual data that the pointers in the other sim.
      // objects' InputQ points to.  To send out a negative message, we need
      // to allocate a new one.
      
      int destination = cancelEvent->dest;
      negEvent        = (BasicEvent *) new char[cancelEvent->size];
      new(negEvent) BasicEvent(cancelEvent);

      negEvent->sign = NEGATIVE;
      negEvent->size = sizeof(BasicEvent);
#ifdef DC_DEBUG 
      dcFile << "[3]Sending antimessage : " << *cancelEvent << endl;
#endif
      // remove the + event's container from the outputQ, and delete both
      // + and - messages--they've been handled fine on the receiving end,
      // as far as we know.
      
      lazyCancelQ.removeCurrent();

      sendEventUnconditionally(negEvent);

      if(commHandle[destination].ptr->isCommManager()){
	delete [] ((char *) negEvent);
	delete [] ((char *) cancelEvent);
      }

      // remove moves the current pointer forward one spot
      cancelEvent = lazyCancelQ.get();
    }
  }
#endif // end of LAZYCANCELLATION
}


void 
TimeWarp::saveState() {
  // Note: the outputPos can get 3 possible values
  //   (i)   No outputs were generated. Hence NULL
  //   (ii)  The pointer to the last event generated in this execution cycle
  //   (iii) Last event generated in the previous state, since no event was
  //         generted in this execution cycle.
  
  state->current->outputPos    = outputQ.getTail();
  state->current->myLastOutput = true;
  state->saveState();
}

int
TimeWarp::inputGcollect(VTime gtime, BasicEvent* inputQptr) {
  register int collected;

#ifdef OBJECTDEBUG
  *lpFile << name << ":input gVT: " << gtime << endl;
#endif
  
  for(register int i = 0; (i < numInFiles); i++) {
    inFileQ[i].gcollect(gtime);
  }

#if defined(INFREQSTATEMANAGER) || defined(LINSTATEMANAGER) || defined(NASHSTATEMANAGER) || defined(COSTFUNCSTATEMANAGER)
  collected = inputQ.gcollect(gtime, localId);
#else
  collected = inputQ.gcollect(gtime, localId, inputQptr);
#endif  //INFREQSTATEMANAGER   

  state->committedEvents(collected, rollbackCount);
  
  return collected;
}

VTime
TimeWarp::stateGcollect(VTime gtime, BasicEvent*& inputQptr,
			Container<BasicEvent>*& outputQptr) {
#ifdef OBJECTDEBUG
  *lpFile << name << ":state gcollecting for  gVT: " << gtime << endl;
#endif

#ifdef STATS
  if (gtime != PINFINITY) {
    lpHandle->simObjArray[localId].setStateQSize(state->queueSize(),
						 state->timeDiff());
  }
#endif
  VTime tmpTime = state->gcollect(gtime, inputQptr, outputQptr);

  return(tmpTime);
}

void
TimeWarp::stateClear() {
  state->clear();
}

void
TimeWarp::clearInitState()  {
  // Set the correct values in "current" state...
  
  state->current->outputPos    = outputQ.getTail();
  state->current->myLastOutput = true;
}

void
TimeWarp::outputGcollect(VTime gtime, Container<BasicEvent>*& outputQptr)  {
  for(register int i = 0; (i < numOutFiles); i++) {
    outFileQ[i].gcollect(gtime);
  }
  
#ifdef STATEDEBUG
  outputQ.gcollect(outputQptr, commHandle, 
		   state->getHeadMyLastOutput(), 
		   (StateManager*)&state);
#else //ifdef STATEDEBUG

#if defined(INFREQSTATEMANAGER) || defined(LINSTATEMANAGER) || defined(NASHSTATEMANAGER) || defined(COSTFUNCSTATEMANAGER)
  outputQ.gcollect(gtime, commHandle); 
#else
  outputQ.gcollect(gtime, outputQptr, commHandle, 
		   state->getHeadMyLastOutput());
#endif
#endif //STATEDEBUG
}

// ***************************************************************************
// Function: void rollback(VTime rollbackTime)
// ***************************************************************************

void
TimeWarp::rollback(VTime rollbackTime) {

  cout << name << ":rollback : " << rollbackTime << " from " << state->current->lVT << endl << flush;

#ifdef STATS
  // collect stats 
  lpHandle->simObjArray[localId].incrementRollbackCount();
  lpHandle->simObjArray[localId].setRollbackDistanceTime((state->current->lVT - rollbackTime)); 
  lpHandle->simObjArray[localId].setRollbackDistanceStates(state->statesRolledBack); 
  if (lastMessageSign == POSITIVE) {
    lpHandle->simObjArray[localId].incrementNumberOfPosStragglers();
  }
#endif

#ifdef LPDEBUG
  *lpFile << name  << ":rollback : " << rollbackTime << " from  " 
	  << state->current->lVT << endl;
#endif
  
  // restore to the correct state
  VTime timeRestored = state->restoreState(rollbackTime);
 
#ifdef LPDEBUG
  *lpFile << name << ":stateRestored : " << *state->current << endl;
  state->current->print(*lpFile);
  *lpFile << endl;
#endif
  
  if(timeRestored == PINFINITY) {
    cerr << "ERROR: " << name << " can't restore state during rollback to " 
	 << rollbackTime << " from LVT " << state->current->lVT << endl;
    exit(-11);
  }
  
  if (state->getStatePeriod() != -1) {
    coastForward(timeRestored);
  }
  
  // we need to set the lVT
  state->current->lVT = timeRestored; 

  rollbackFileQueues(rollbackTime);
  
  CANCEL_MESSAGES(rollbackTime);

#ifdef MESSAGE_AGGREGATION
#ifdef STATS
  logicalProcessStats::increment_got_roll_back();
#endif
    getLPHandle()->flushOutputMessages();
#endif
} // end of rollback()

// ***************************************************************************
// Function: void cancelMessagesAggressively(const VTime rollbackTime)
// ***************************************************************************

void
TimeWarp::cancelMessagesAggressively(const VTime rollbackTime)  {
  // send out negative messages
  BasicEvent *cancelEvent, *negEvent;
  
  // while using infrequent state saving strategies we cannot get
  // cancelEvent info from state->current so we search the outputQueue
  // with rollbackTime as search parameter.

#if defined(INFREQSTATEMANAGER) || defined(LINSTATEMANAGER) || defined(NASHSTATEMANAGER) || defined(COSTFUNCSTATEMANAGER)
  cancelEvent = outputQ.find(rollbackTime, GREATEREQUAL);
  outputQ.setCurrentToFind();
#else
    // Instead of doing the above search, we will try to get the outputQ
    // position through the state. Excluding the first state, outputPos
    // pointer in the state points to the last event created in the last
    // simulation cycle.  So the event we need to undo is the one next to
    // this one.
    cancelEvent = findPositionFromState();
#endif  
    // at this point cancelEvent points to the first Event sent after
    // this time., The currentPos of the outputQ is also set to point to
    // that event so that iteration of the outputQ can be easily done.

#ifdef LPDEBUG
  if((cancelEvent)&&(cancelEvent->sendTime < rollbackTime)){
    *lpFile << name << ":found : " << *cancelEvent << "\n"
	    << name << ":rollbackTime : " << rollbackTime << endl;
  }
#endif
  
#ifdef ONE_ANTI_MESSAGE
  int iterateUpTo = getLPHandle()->getTotalNumberOfObjects();
  
  while(cancelEvent != NULL){
    
#ifdef MESSAGE_AGGREGATION
#ifndef ADAPTIVE_AGGREGATION
    lpHandle->incrementAgeOfAggregatedMessage();
#endif
#endif
    
    // the outputQ has the actual data that the pointers in the other sim.
    // objects' InputQ points to.  To send out a negative message, we need
    // to allocate a new one.
    int destID = cancelEvent->dest;
    
    if (alreadySentAntiMessage[destID] != NULL) {
#ifdef LPDEBUG
      *lpFile << "OAM-Supressed negative message for : " << *cancelEvent
	      << endl;
#endif

      if (commHandle[destID].ptr->isCommManager()) {
	delete [] (char *)outputQ.removeCurrent();
#if defined(ONE_ANTI_MESSAGE) && defined(STATS)
	lpHandle->simObjArray[localId].incrementRemoteEventsSupp();
#endif
      }
      else{
	outputQ.removeCurrent();
#if defined(ONE_ANTI_MESSAGE) && defined(STATS)
	lpHandle->simObjArray[localId].incrementLocalEventsSupp();
#endif
      }
    }
    else {
      alreadySentAntiMessage[destID] = cancelEvent;
      outputQ.removeCurrent();
    }
    cancelEvent = outputQ.get();
  }
  
  for(int index2 = 0; (index2 < iterateUpTo); index2++){
    if(alreadySentAntiMessage[index2] != NULL){
      cancelEvent = alreadySentAntiMessage[index2];
      int destination = cancelEvent->dest;
      
      negEvent = (BasicEvent*) new char[sizeof(BasicEvent)];
      new(negEvent) BasicEvent((const BasicEvent *) cancelEvent);
      
      negEvent->sign = NEGATIVE;
      negEvent->size = sizeof(BasicEvent);
      
      sendEventUnconditionally(negEvent);
#ifdef STATS
      lpHandle->lpStats.incrementTotalEventsUndone();;
      lpHandle->simObjArray[localId].incrementEventsUndone();
#endif      

      if(commHandle[destination].ptr->isCommManager()){
	delete [] ((char *)negEvent);
	
	// There will be only one copy of negEvent, but cancelEvent
	// could exist in another processes inputQ on this LP.
	delete [] (char*)cancelEvent;
      }
      alreadySentAntiMessage[index2] = NULL;
    }
  }
  
#ifdef LPDEBUG
  //this test will induce FMRs if the pts in the state are incorrect.
  stateCheck();
#endif
  
#else  // We are not using the ONE_ANTI_MESSAGE STUFF
  while (cancelEvent != NULL) {
    
#ifdef MESSAGE_AGGREGATION
#ifndef ADAPTIVE_AGGREGATION
    lpHandle->incrementAgeOfAggregatedMessage();
#endif
#endif
    // the outputQ has the actual data that the pointers in the other sim.
    // objects' InputQ points to.  To send out a negative message, we need
    // to allocate a new one.
    
    int destID = cancelEvent->dest;
    negEvent   = (BasicEvent*) new char[cancelEvent->size];
    new(negEvent) BasicEvent(cancelEvent);
    
    negEvent->sign = NEGATIVE;
    negEvent->size = cancelEvent->size;
    
#ifdef STATEDEBUG
    *lpFile2 << "sending " << endl;
    *lpFile2 << *negEvent << "Id " << id << endl;
    // sanity check - compares state outputPos with neg events
    state->checkEvent(outputQ.getCurrent(), 1);
#endif
    // remove the + event's container from the outputQ, and delete both
    // + and - messages--they've been handled fine on the receiving end,
    // as far as we know.
    
    outputQ.removeCurrent();

    sendEventUnconditionally(negEvent);
#ifdef STATS
    lpHandle->lpStats.incrementTotalEventsUndone();;
    lpHandle->simObjArray[localId].incrementEventsUndone();
#endif          
    if(commHandle[destID].ptr->isCommManager()){
      delete [] ((char *)negEvent);
      delete [] (char*)cancelEvent;
    }
    // remove moves the current pointer forward one spot
    cancelEvent = outputQ.get();
  }
#endif // end of ONE_ANTI_MESSAGE ifdef
}

BasicEvent*
TimeWarp::findPositionFromState()  {

  // Instead of doing a search, we will try to get the outputQ position
  // through the state. Excluding the first state, outputPos pointer in the
  // state points to the last event created in the last simulation cycle. 
  // So the event we need to undo is the one next to this one.
  
  // Note: Special Case : If the first state is the initial state (namely
  // state saved at time ZERO or ZERO(1), outputPos pointer points to last 
  // event created. Otherwise, if the first state is the first one after the
  // GVT update (not ZERO, the state right before gvt), outputPos pointer 
  // points to the next event or to the first output event of the following 
  // cycle. Exception in this case: outputPos points to NULL, outputPos is 
  // the first event in outputQ
  
  BasicEvent *cancelEvent = NULL;
  
  if (state->current->outputPos == NULL) {
    if (outputQ.getHead() != NULL) {
      cancelEvent = outputQ.getHead()->object;
      outputQ.setCurrent(outputQ.getHead());
    }
  }
  else {
    if (state->current->outputPos->next != NULL) {
      cancelEvent = state->current->outputPos->next->object;
      outputQ.setCurrent(state->current->outputPos->next);
    }
  }

  return cancelEvent;
}

#if defined(LAZYCANCELLATION) || defined(LAZYAGGR_CANCELLATION)

void
TimeWarp::moveMessagesToLazyQueue(const VTime rollbackTime)  {

  // send out negative messages
  BasicEvent *cancelEvent;

  if(state->getStatePeriod() != -1){
    cancelEvent = outputQ.find(rollbackTime, GREATEREQUAL);
  }
  else{
    cancelEvent = findPositionFromState();
  }
  
#ifdef DC_DEBUG
  if((cancelEvent)&&(cancelEvent->sendTime < rollbackTime)){
    dcFile << name << ":found : " << *cancelEvent << "\n" 
	   << "rollbackTime : " << rollbackTime << endl;
  }
#endif
  
  if (cancelEvent != NULL) {
    suppressMessage |= LAZYCANCEL;
#ifdef DC_DEBUG
    dcFile << name << ":Suppression Flag = " << suppressMessage << endl
	   << name << ":Inserting events into Lazy Cancellation Queue" << endl;
#endif
    
    while (cancelEvent != NULL) {
#ifdef DC_DEBUG
      dcFile << *cancelEvent << endl;
#endif
      lazyCancelQ.insert(cancelEvent);
      
      // remove from the outputQueue
      outputQ.removeCurrent();
      
      // remove moves the current pointer forward one spot
      cancelEvent = outputQ.get();
    }
  }
  // Reset the lazy cancellation queue to the beginning
  lazyCancelQ.seek(0,START);
}

#endif // end of LAZYCANCELLATION or LAZYAGGR_CANCELLATION

// ***************************************************************************
// Function: void recvEvent(BasicEvent *newEvent)
// ***************************************************************************

void
TimeWarp::recvEvent(BasicEvent *newEvent) {
#ifdef CHECKASSERTIONS
  if (state->getHead() != NULL) {
    //    assert(newEvent->recvTime > state->getHead()->lVT);
    if (newEvent->recvTime <= state->getHead()->lVT) {
      cout << "newEvent: " << *newEvent << "\n"
	   << "Head of queue: " << *(state->getHead()) << "\n"
	   << "Tail of queue: " << *(state->getTail()) << "\n"
	   << "gVT = " << gVTHandle->getGVT() << "\n";
      *lpFile << "newEvent: " << *newEvent << "\n"
	   << "Head of queue: " << *(state->getHead()) << "\n"
	   << "Tail of queue: " << *(state->getTail()) << "\n"
	   << "gVT = " << gVTHandle->getGVT() << endl;

      if (gVTHandle->recoveringFromCheckpoint == true) {
	cout << "Recovering from checkpoint" << endl;
	*lpFile << "Recovering from checkpoint" << endl;
      }
      abort();
    }
  }
  assert( (newEvent->sign == POSITIVE) || (newEvent->sign == NEGATIVE));
#endif
  
#ifdef MESSAGE_AGGREGATION
#ifndef ADAPTIVE_AGGREGATION
  (this)->lpHandle->incrementAgeOfAggregatedMessage();
#endif
  lpHandle->incrementAgeOfAggregatedMessage();
#endif
  bool inThePast;

#ifdef LPDEBUG
  *lpFile << name << ":received : " << *newEvent << endl;
#endif
  
#ifdef MATTERNGVTMANAGER
  if(newEvent->dest != newEvent->sender){
    if(newEvent->color == 0 ){
      whiteMessages--;
    }
  }
  else {
    if(newEvent->color == 1){
      redMessages--;
    }
  }
#endif

#ifdef STATS
  lastMessageSign = newEvent->sign;
  // collect stats
  if (newEvent->sign == POSITIVE) {
    lpHandle->simObjArray[localId].incrementPosRecv();
  }
  else {
    lpHandle->simObjArray[localId].incrementNegRecv();
  }
#endif

  VTime newEventTime = newEvent->recvTime;

  // insert event into input queue
  inThePast = inputQ.insert(newEvent,localId);
  
  if (inThePast == true){  

    if(newEventTime < gVTHandle->getGVT()) {
      cerr << "Rolling back before gVT (" << gVTHandle->getGVT() 
	   << ") due to event at time: " << newEventTime << "\n"
	   << *newEvent << endl;
      cout << "*****************************************" << endl;
      inputQ.printlVTArray();
      cout << "*****************************************" << endl;
      abort();
    }

    state->startRollbackTiming();
    
#if defined(MESSAGE_AGGREGATION) && defined(FIXED_SLOPE_WITH_ERROR)
    lpHandle->setRollingBackFlagInMessageManager();
#endif
    
    // rollback now ...
    rollback(newEventTime);
    
#if defined(MESSAGE_AGGREGATION) && defined(FIXED_SLOPE_WITH_ERROR)
    lpHandle->resetRollingBackFlagInMessageManager();
#endif

    state->finishRollbackTiming();
  } 

}
// **** end of function recvEvent *******************************************


// **************************************************************************
// Function: void calculateMin()
// **************************************************************************

VTime
TimeWarp::calculateMin()  {
  VTime retval = PINFINITY;
  
  // if we have an event to execute, that is what we want to use...
  BasicEvent *tempEvent = inputQ.get();
  
  if( tempEvent != NULL ){
    //  we need to use the event we're about to execute
    retval = tempEvent->recvTime;
  }
  
  return retval;
}
// **** end of function calculateMin ****************************************

// **************************************************************************
// Function: void withinTimeWindow()
// **************************************************************************

bool
TimeWarp::withinTimeWindow()  {
  bool withinWindow = false;
  
  // this doesn't get an event for execution - it just checks the time
  BasicEvent *currentEvent = inputQ.get();	
  
  if((currentEvent) && 
     ((currentEvent->recvTime - gVTHandle->getGVT()) < timeWindow)){ 
    withinWindow = true;
  }
  else if((getLVT() - gVTHandle->getGVT()) < timeWindow) {
    // this condition is for source - like objects which may or may not
    // be event driven.
    
    withinWindow = true;
  }
  
  return withinWindow;
}
//**** end of function withinTimeWindow *************************************

bool
TimeWarp::suppressLazyMessages(BasicEvent* toSend) {
  bool suppressDecision = false;
  
#if defined(LAZYCANCELLATION) || defined(LAZYAGGR_CANCELLATION)
  if (suppressMessage & LAZYCANCEL) {
    suppressDecision = lazyCancel(toSend);
  }
#endif
  return suppressDecision;
}
//**** end of function suppressLazyMessages *********************************

void
TimeWarp::coastForward(VTime timeRestored) {
  BasicEvent *nextToExecute = NULL;
  BasicEvent *findEvent     = NULL;
  
  // This needs to be optimized, so that timings are not taken for those
  // algorithms that do not need them.
  if (timingsNeeded) { 
    stopWatch.start();
  }
  
  inputQ.saveCurrent();
  // Peek at next message to execute after rollback is completed for this 
  // object.
  nextToExecute = inputQ.getObj(localId);
  
  // find first message to re-apply during coasting forawrd
  // the objectId (id) is needed because the compare func for LTSF also
  // uses the dest of the objects.  LocalId is needed to get this object's
  // miniList.
  findEvent = inputQ.find(timeRestored, id, GREATEREQUAL, localId);
  inputQ.setCurrentObjToFindObj(localId);
  // if we have an event at this time then we should skip it because it
  // has already been executed.
  if (findEvent != NULL && timeRestored == findEvent->recvTime) {
    findEvent = inputQ.getAndSeekObj(id,localId);
  }

  findEvent = inputQ.getObj(localId);
  
  suppressMessage |= COASTFORWARD;  // turn on message suppression
  
#ifdef LPDEBUG
  *lpFile << name << ":Suppression Flag = " << suppressMessage << endl;
  if ((findEvent != NULL) && (nextToExecute != NULL)) {
    *lpFile << name <<": Before Coast : lVT ="<< state->current->lVT 
	    << " looking at event\n" << name << ":"
	    << *findEvent << "\n" << name 
	    << ":next event after coasting should be\n"
	    << name << ":" << *nextToExecute << "\n";
  }
  else {
    if (findEvent == NULL) {
      *lpFile << name << ":No event found!" << "\n";
    }
    else {
      *lpFile << name <<": Before Coast : lVT ="<< state->current->lVT 
	      << " looking at event\n" << name << ":"
	      << *findEvent << "\n";
    }
    if (nextToExecute == NULL) {
      *lpFile << name << ":No end event!" << "\n";
    }
  }
#endif

  register int counter = 0;
  VTime timeAtLastCall = getLVT();
  
  while ((findEvent != NULL) && (nextToExecute != findEvent)) {
    
#ifdef LPDEBUG
    *lpFile << name <<": Coasting :"<< findEvent->recvTime << " Event = "
	    << *findEvent << "\n";
    state->current->print(*lpFile);
    *lpFile << "\n";
#endif
    
    if (timeAtLastCall != getLVT() ) {
      // ignore events at the same time in our count of the forward
      // execution.
      counter++;
    }
    
    findEvent->alreadyProcessed = false;
    // Set the input Queue to point at the message to use in coasting
    // forward for this object.
    inputQ.setCurrentToCurrentObj(localId);
    executeProcess();
    findEvent = inputQ.getObj(localId);
  }
  
  inputQ.restoreCurrent();
  
  suppressMessage &= ~COASTFORWARD; // turn message suppression off
  state->coastedForwardEvents( counter );
  
  if (timingsNeeded) {
    stopWatch.stop();
    state->coastForwardTiming( stopWatch.elapsed() );
  }
  
#ifdef LPDEBUG
  *lpFile << name <<": finished coasting next event for this object is ";
  if (findEvent == NULL) {
    *lpFile << "NULL" << endl;
  }
  else {
    *lpFile << *findEvent << endl;
  }
  inputQ.printSmallQ(*lpFile,localId);
#endif
}
//**** end of function coastForward *****************************************


#if defined(LAZYCANCELLATION) || defined(LAZYAGGR_CANCELLATION)

bool
TimeWarp::lazyCancel(BasicEvent* toSend) {

  bool suppressDecision = false;
#ifdef DC_DEBUG
  int flag = 0;
  dcFile << name << ":Begin Lazy Cancel Phase" << endl;
#endif
  
  BasicEvent *lcOutputEvent = NULL, *negEvent = NULL;
  lcOutputEvent = lazyCancelQ.get();
  
  // Cancel any messages between the last message processed in the output 
  // Queue and the current message to send.
  
  while ((lcOutputEvent != NULL) 
	 && (lcOutputEvent->sendTime < state->current->lVT)) {
    
    int destID = lcOutputEvent->dest;
    negEvent   = (BasicEvent *) new char[lcOutputEvent->size];
    new(negEvent) BasicEvent(lcOutputEvent);
    
    negEvent->sign = NEGATIVE;
    negEvent->size = sizeof(BasicEvent);
#ifdef DC_DEBUG
    dcFile << "Compared with          : " << *toSend << endl;
    dcFile << "[1]Sending antimessage : " << *lcOutputEvent << endl;
    flag++;
#endif
    
    lazyCancelQ.removeCurrent();
    
    sendEventUnconditionally(negEvent);
 
    if(commHandle[destID].ptr->isCommManager()){
      delete [] ((char *)negEvent);
      delete [] (char*)lcOutputEvent;
    }
    lcOutputEvent = lazyCancelQ.get();
  }
  
  bool Hit = false;
  
  // Check messages at the current time for lazy cancellation.
  while ((lcOutputEvent != NULL) 
	 && (toSend->sendTime == lcOutputEvent->sendTime) 
	 && (Hit == false)) {
    
#ifdef DC_DEBUG
    if (lcOutputEvent == NULL) {
      dcFile << name << ":lcOutputEvent = NULL" << endl;
    }
    else {
      dcFile << name << ":Comparing messages to send:\n";
      dcFile << "comparing " << endl << *lcOutputEvent << " and " << endl 
	     << *toSend << endl;
    }
#endif
    
    // This is a little wierd so lets be detailed. We want to
    // only compare the users' stuff and the destination, so in
    // essence we want to ignore the eventId.
    SequenceCounter oldLcOutputEvent;
    oldLcOutputEvent       = lcOutputEvent->eventId;
    lcOutputEvent->eventId = toSend->eventId;
    
    bool tempFlag = lcOutputEvent->alreadyProcessed;
    lcOutputEvent->alreadyProcessed = false;
    
    if (toSend->lazyCmp(lcOutputEvent)) {
      // Got one! Move event from Lazy Cancel Queue to output Queue.
      // restore the old eventId.
      lcOutputEvent->eventId = oldLcOutputEvent;
      
      lcOutputEvent->alreadyProcessed = tempFlag;
      
#ifdef DC_DEBUG
      dcFile << "Lazy Hit : putting back into outputQ : " << endl 
	     << *lcOutputEvent << endl;
      flag++;
#endif
      
      outputQ.insert(lcOutputEvent);
      lazyCancelQ.removeCurrent();
      
      suppressDecision = true;
      Hit = true;
    }
    else { // If LAZY cancellation check failed
      lcOutputEvent->alreadyProcessed = tempFlag;
      
#ifdef DC_DEBUG
      dcFile << "Lazy Miss " << " : " << *lcOutputEvent << endl;
      flag++;
#endif
      // restore the old eventId.
      lcOutputEvent->eventId = oldLcOutputEvent;
      
      int destID = lcOutputEvent->dest;
      negEvent   = (BasicEvent *) new char[lcOutputEvent->size];
      new(negEvent) BasicEvent(lcOutputEvent);
      
      negEvent->sign = NEGATIVE;
      negEvent->size = sizeof(BasicEvent);
      
#ifdef DC_DEBUG
      dcFile << "Compared with          : " << *toSend << endl;
      dcFile << "[4]Sending antimessage : " << *lcOutputEvent << endl;
      flag++;
#endif
      
      lazyCancelQ.removeCurrent();
      sendEventUnconditionally(negEvent);
      
      if(commHandle[destID].ptr->isCommManager()){
	delete [] ((char *)negEvent);
	delete [] (char*)lcOutputEvent;
      }
      lcOutputEvent = lazyCancelQ.get();
    }
#ifdef DC_STATS
    dcFile << Hit << endl;
#endif
  }
  
#ifdef DC_DEBUG
  if(flag == 0){
    dcFile << "New case : " << *toSend << endl;
    lazyCancelQ.print(dcFile);
  }
#endif
  
  if (lazyCancelQ.size() == 0) {
    // End of Lazy Cancellation phase.
    suppressMessage &= ~LAZYCANCEL;
    
#ifdef LPDEBUG
    *lpFile << name << ":Lazy Cancellation Queue Exhausted!" << endl;
    *lpFile << name << ":Suppression Flag = " << suppressMessage << endl;
#endif
  }
  else {
    // Restore Lazy Cancellation Queue to the first event for next call.
    lazyCancelQ.seek(0,START);
    lcOutputEvent = lazyCancelQ.get();
    
    // Not needed for all applications but you never know when u need this so
    // it gets called always
    while ((lcOutputEvent != NULL) 
	   && (lcOutputEvent->sendTime == state->current->lVT)) {
      
      int destID = lcOutputEvent->dest;
      negEvent   = (BasicEvent *) new char[lcOutputEvent->size];
      new(negEvent) BasicEvent(lcOutputEvent);
      
      negEvent->sign = NEGATIVE;
      negEvent->size = sizeof(BasicEvent);
#ifdef DC_DEBUG
      dcFile << "[2]Sending antimessage : " << *lcOutputEvent << endl;
#endif
      lazyCancelQ.removeCurrent();
      sendEventUnconditionally(negEvent);
      
      
      if(commHandle[destID].ptr->isCommManager()){
	delete [] ((char *)negEvent);
	delete [] (char*)lcOutputEvent;
      }
      
      lcOutputEvent = lazyCancelQ.get();
    }
    
    lazyCancelQ.seek(0,START);
  }
#ifdef DC_DEBUG
  dcFile << name << ":End Lazy Cancel Phase(suppressDecision =  " 
	 << suppressDecision << endl;
#endif
  return (suppressDecision);
}
#endif // end of LAZYCANCELLATION or LAZYAGGR_CANCELLATION

//**** end of function lazyCancel ******************************************

// NEW_STATE_MANAGEMENT

void 
TimeWarp::timeWarpInit() {
  getStateManager()->createInitialState(); 
#ifdef MATTERNGVTMANAGER
  tMin = state->current->lVT;
#endif
  initialize();
}

void
TimeWarp::outputGcollect(VTime) {
  // This method is written to stop CC from yelling that the other
  // outputGcollect method hides the virtual method in BasicTimeWarp.
  cerr << "This method must NEVER be called." << endl;
  abort();
}

void
TimeWarp::setTimeWindow(VTime newWindow) {
  timeWindow = newWindow;
}

void
TimeWarp::setLPHandle(LogicalProcess* lpHandle2) {
  this->lpHandle = lpHandle2;
#ifdef ONE_ANTI_MESSAGE
  if (alreadySentAntiMessage != NULL) {
    delete [] alreadySentAntiMessage;
  }
  alreadySentAntiMessage = 
    new BasicEvent*[lpHandle->getTotalNumberOfObjects()];
  
  int iterateUpTo = getLPHandle()->getTotalNumberOfObjects();
  
  for(int i = 0; (i < iterateUpTo); i++) {
    alreadySentAntiMessage[i] = NULL;
  }
#endif
}


void
TimeWarp::rollbackFileQueues(VTime time) {
  int i;

  // Rollback input file queues
  for (i = 0; i < numInFiles; i++) {
    inFileQ[i].rollbackTo(time);
  }

  // Rollback output file queues
  for (i = 0; i < numOutFiles; i++) {
    outFileQ[i].rollbackTo(time);
  }
}

void
TimeWarp::setFile(ofstream *outfile) {
  lpFile = outfile;
  inputQ.setFile(outfile);
  state->setFile(outfile);
  outputQ.setFile(outfile);
}

#ifdef STATEDEBUG
void
TimeWarp::setStateFile(ofstream *outfile) {
  lpFile2 = outfile;
  state->setFile(lpFile2);
  inputQ.setStateFile(lpFile2);
}
#endif

#ifdef DC_DEBUG
void
TimeWarp::openDCFile(char* dcFilename) {
  dcFile.open(dcFilename);
}
#endif

void
TimeWarp::terminateSimulation(char *msg)  {
  ((FOSSIL_MANAGER *)gVTHandle)->terminateSimulation(msg);
}

#if defined(LAZYCANCELLATION) || defined(LAZYAGGR_CANCELLATION)
VTime
TimeWarp::getLazyQMinTime()  {
  if(lazyCancelQ.getHead() != NULL){
    return lazyCancelQ.getHead()->object->sendTime;
  }
  else{
    return PINFINITY;
  }
}

int
TimeWarp::getLazyQSize(){
  return lazyCancelQ.size();
}

#endif // end of LAZYCANCELLATION or LAZYAGGR_CANCELLATION


void
TimeWarp::deAllocateState(BasicState* oldState) {
  delete oldState;
}


#endif
