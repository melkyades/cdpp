//-*-c++-*-
#ifndef DYNAMICCANCELLATION_CC
#define DYNAMICCANCELLATION_CC
// Copyright (c) 1994,1995 Ohio Board of Regents and the University of
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
// $Id: DynamicCancellation.cc,v 1.6 1999/10/24 23:25:52 ramanan Exp $
//
//
//---------------------------------------------------------------------------
#if defined(LAZYCANCELLATION) || defined(LAZYAGGR_CANCELLATION)
#include "DynamicCancellation.hh"

#ifdef PERMANENTLYSET
LazyAggrTimeWarp::LazyAggrTimeWarp():measurementCycle(PERMANENTSETVAL)
#else
LazyAggrTimeWarp::LazyAggrTimeWarp():measurementCycle(FILTER_DEPTH)
#endif
{
  comparisonResults = new int[measurementCycle];
  
  currentMode = AGGRESSIVE;
  hitCount = missCount = 0;
  lazyAggrHit = lazyHit = false;
  hitRatio = 0.0;
  noOfComparisions = 0;
  for(int i=0;i < measurementCycle; i++){
    comparisonResults[i] = 0;
  }
  
#ifdef PERMANENTLY_AGGR
  permanentlyAggressive = false;
#endif
  
#ifdef PERMANENTLYSET
  modePermanent = false;
#endif
}

LazyAggrTimeWarp::~LazyAggrTimeWarp() {
  delete [] comparisonResults;
  lazyAggrQ.gcollect(PINFINITY);
}

void 
LazyAggrTimeWarp::executeSimulation() {
  
  TimeWarp::executeSimulation();
  
#if defined(PERMANENTLY_AGGR)
  if(!permanentlyAggressive) {
    if((suppressMessage & LAZYAGGRCANCEL) || (suppressMessage & LAZYCANCEL)) {
      setCancellationMode();
    }
  }
#elif defined(PERMANENTLYSET)
  if(!modePermanent) {
    // call setCancellationMode() only when we are recovering from a 
    // rollback - otherwise we are just wasting time calling it.
    if((suppressMessage & LAZYAGGRCANCEL) || (suppressMessage & LAZYCANCEL)) {
      setCancellationMode();
    }
  }
#else
  if((suppressMessage & LAZYAGGRCANCEL) || (suppressMessage & LAZYCANCEL)) {
    setCancellationMode();
  } 
#endif
}

bool
LazyAggrTimeWarp::suppressMessageTest(BasicEvent *toSend) {

  // Aggressive Cancellation without checkpointing (optimizations).
  if(suppressMessage == NONE) {
    return false;
  }
  
  if(suppressMessage == COASTFORWARD) {
    return true;
  }
  
#if defined(PERMANENTLY_AGGR)
  // Skip this if the hitRatio drops below the THIRD_THRESHOLD
  if(!permanentlyAggressive) {
    if(suppressMessage == LAZYAGGRCANCEL) {
      checkForLazyAggrHit(toSend);
      return false;
    }    
  }
#elif defined(PERMANENTLYSET)
  // Skip this after PERMANENTSETVAL comparisons
  if(!modePermanent) {
    if(suppressMessage == LAZYAGGRCANCEL) {
      checkForLazyAggrHit(toSend);
      return false;
    }
  }
#else
  if(suppressMessage == LAZYAGGRCANCEL) {
    // Do a passive compare while sending out the anti-messages
    checkForLazyAggrHit(toSend);
    return false;
  }
#endif
  
  bool suppressDecision = false;

  if(suppressMessage == LAZYCANCEL) {
    suppressDecision = LazyAggrTimeWarp::lazyCancel(toSend);
  }
  
  return suppressDecision;
}

void
LazyAggrTimeWarp::checkForLazyAggrHit(BasicEvent *toSend) {

  BasicEvent *lACOutputEvent;  
  lazyAggrHit    = false;
  lACOutputEvent = lazyAggrQ.get();
  
#ifdef LPDEBUG
  outFile << name << ":Start of Lazy Aggressive Cancellation Phase" << endl;
  outFile << name << ": for message\n" << *toSend << "\nCancelling:" << endl;
#endif
  
  // Cancel any messages between the last message processed in the output 
  // Queue and the current message to send.
  while((lACOutputEvent != NULL) 
	&& (lACOutputEvent->sendTime < state->current->lVT)) {
    delete [] (char *)lazyAggrQ.removeCurrent();
    lACOutputEvent = lazyAggrQ.get();
  }
  
#ifdef LPDEBUG
  outFile << name << ":Done cancelling past messages  " << endl;
  if (lACOutputEvent == NULL) {
    outFile << name << "lcOutputEvent = NULL" << endl;
  }
  else {
    outFile << name << "Comparing messages to send:\n";
  }
#endif
  
  // Check messages at the current time for lazy aggressive cancellation.
  while ((lACOutputEvent != NULL) 
	 && (toSend->sendTime == lACOutputEvent->sendTime) 
	 && (lazyAggrHit == false) ) {
    
#ifdef LPDEBUG
    outFile << name <<  ": against " << endl << *lACOutputEvent << endl;
#endif
    
    if(toSend->lazyCmp(lACOutputEvent)) {
      delete [] (char*)lazyAggrQ.removeCurrent();
      lazyAggrHit = true;
      lACOutputEvent = lazyAggrQ.get();
    }
    else{
      lazyAggrQ.seek(1,CURRENT); 
      lACOutputEvent = lazyAggrQ.get();
    }
  } // end of while(....)
  
#if defined(PERMANENTLYSET)
  if(noOfComparisions < measurementCycle) {
    comparisonResults[noOfComparisions] = lazyAggrHit;
  }
#else
  comparisonResults[noOfComparisions % measurementCycle] = lazyAggrHit;
#endif
  ++noOfComparisions;
  
  if (lazyAggrQ.size() == 0) {
    // End of Lazy Aggressive Cancellation phase, so set it to normal until
    // another rollback
    suppressMessage &= ~LAZYAGGRCANCEL;
    
#ifdef LPDEBUG
    outFile << name << ":Lazy Cancellation Queue Exhausted!" << endl;
    outFile << name << ":Suppression Flag = " << suppressMessage << endl;
#endif
  }
  else {
    // Restore Lazy Aggressive Queue to the first event for next call.
    lazyAggrQ.seek(0,START);
  }
  
#ifdef LPDEBUG
  outFile << name << ":End of Lazy Aggressive Cancellation Phase" << endl;
#endif
}

void
LazyAggrTimeWarp::cancelMessages(const VTime rollbackTime) {

  if(currentMode == AGGRESSIVE) {
    BasicEvent *cancelEvent, *negEvent;
    
    // while using infrequent state saving strategies we cannot get
    // cancelEvent info from state.current so we search the outputQueue
    // with rollbackTime as search parameter.
    if(state->getStatePeriod() != -1)  {
	cancelEvent = outputQ.find(rollbackTime, GREATEREQUAL);
	outputQ.setCurrentToFind();
    }
    else  {
      // Instead of doing this search, we will try to get the outputQ position
      // through the state. Excluding the first state, outputPos pointer in the
      // state points to the last event created in the last simulation cycle. 
      // So the event we need to undo is the one next to this one.
      
      // Note: Special Case : If the first state is the initial state (namely
      // state saved at time ZERO or ZERO(1), outputPos pointer points to last 
      // event created. Otherwise if the first state is the first one after the
      // GVT update (not ZERO, the state right before gvt), outputPos pointer 
      // points to the next event or to the first output event of the following 
      // cycle. Exception in this case: outputPos points to NULL, outputPos is 
      // the first event in outputQ

      if(state->current->myLastOutput == false){
	// if it is NULL, take the head of the queue
	if (state->current->outputPos == NULL)  {
	  if (outputQ.getHead() != NULL)  { // check this case
	    cancelEvent = (BasicEvent *) (outputQ.getHead()->object);
	    outputQ.setCurrent(outputQ.getHead());
	  }
	}
	else {
	  cout << "this should never happen !! " << endl;
	  cancelEvent = NULL;
	}
      }
      else {
	if(state->current->outputPos == NULL)  {
	  // Initial state has been restored but myLastOutput is true 
	  // cancelEvent = outputQ.getHead()->object;
	  // outputQ.setCurrent(outputQ.getHead());
	  cancelEvent = NULL;
	}
	else  {
	  if(state->current->outputPos->next != NULL){
	    cancelEvent = (BasicEvent *) 
	      (state->current->outputPos->next->object);
	    outputQ.setCurrent(state->current->outputPos->next);
	  } 
	  else {
	    cancelEvent = NULL;
	  }
	}
      }
    }
    // at this point cancelEvent points to the first Event sent after
    // this time
    
#ifdef LPDEBUG
    if((cancelEvent) && (cancelEvent->sendTime < rollbackTime)){
      outFile << name << ":found : " << *cancelEvent << "\n" 
              << "rollbackTime : " << rollbackTime << endl;
    }
#endif

    if(cancelEvent != NULL) {
      
#if defined(PERMANENTLYSET)
      // Once the mode is set permanently, we don't have do to the passive
      // comparisions, but we need it till we determine what the permanent 
      // mode is going to be.
      if(!modePermanent) {
	suppressMessage |= LAZYAGGRCANCEL;
      }
#elif defined(PERMANENTLY_AGGR)
      if(!permanentlyAggressive) {
	suppressMessage |= LAZYAGGRCANCEL;
      }
#else
      suppressMessage |= LAZYAGGRCANCEL;
#endif
      
#ifdef LPDEBUG
      outFile << name << ":Suppression Flag = " << suppressMessage << "\n"
	      << "Inserting events into lazyAggrQ." << endl;
#endif
      while(cancelEvent != NULL && outputQ.size() != 0 ) {
	// send out the negative messages
	// the outputQ has the actual data that the pointers in the other sim.
	// objects' InputQ points to.  To send out a negative message, we need
	// to allocate a new one.
	
	int destID = cancelEvent->dest;
	negEvent = (BasicEvent*) new char[cancelEvent->size];
	new (negEvent) BasicEvent(cancelEvent);
	
	// send out the -ve message as usual 
	negEvent->sign = NEGATIVE;
	//negEvent->size = sizeof(BasicEvent);
#ifdef STATEDEBUG
	*lpFile2 << "sending " << endl;
	*lpFile2 << *negEvent << "Id " << id << endl;
	// sanity check - compares state outputPos with neg events
	state->checkEvent(outputQ.getCurrent(), 1);
#endif
	BasicEvent* insertEvent = (BasicEvent*) new char[cancelEvent->size];
	new (insertEvent) BasicEvent(cancelEvent); 
	
	TimeWarp::sendEventUnconditionally(negEvent);
	
	// Also insert a copy into the lazyAggrQ
#if defined(PERMANENTLYSET)
	if(!modePermanent) {
	  lazyAggrQ.insert(insertEvent); // FMI, insert makes a copy
	}
#elif defined(PERMANENTLY_AGGR)
	if(!permanentlyAggressive) {
	  lazyAggrQ.insert(insertEvent); 
	}
#else
	lazyAggrQ.insert(insertEvent);
#endif

	outputQ.removeCurrent();
	if(commHandle[destID].ptr->isCommManager()){
	  delete [] ((char *)negEvent);
	  delete [] (char*)cancelEvent;
	}
	// Get the next one now
	cancelEvent = outputQ.get();
      }
    }
    
    // reset lazyAggrQ to point to the beginning of the queue
#if defined(PERMANENTLYSET)
    if(!modePermanent) {
      lazyAggrQ.seek(0, START);
    }
#elif defined(PERMANENTLY_AGGR)
    if(!permanentlyAggressive) {
      lazyAggrQ.seek(0, START);
    }
#else
    lazyAggrQ.seek(0, START); 
#endif
  }
  else { 

    // the "suppressMessage" flag is set to LAZYCANCEL in 
    // TimeWarp::moveMessagesToLazyQueue(), so we don't need to worry about 
    // it here. 
    TimeWarp::moveMessagesToLazyQueue(rollbackTime);
  }
}

bool
LazyAggrTimeWarp::lazyCancel(BasicEvent *toSend) {
  
  lazyHit = false;
  lazyHit = TimeWarp::lazyCancel(toSend);
  
#if defined(PERMANENTLYSET)
  if(!modePermanent) {
    comparisonResults[noOfComparisions] = lazyHit;
  }
#elif defined(PERMANENTLY_AGGR)
  if(!permanentlyAggressive) {
    comparisonResults[noOfComparisions % measurementCycle] = lazyHit;
  }
#else
  comparisonResults[noOfComparisions % measurementCycle] = lazyHit;
#endif
  
  ++noOfComparisions; 
  return lazyHit;
}

void
LazyAggrTimeWarp::setCancellationMode() {
  
  hitCount = 0;

#if defined(PERMANENTLYSET)
  // If modePermanent is true then fix the Cancellation mode once and for
  // all. determineMode() is called only once and that is when
  // noOfComparisons == PERMANENTSETVAL.
  if(noOfComparisions == PERMANENTSETVAL) {
    modePermanent = true;
    
    for(int i = 0; i < measurementCycle;i++) {
      if(comparisonResults[i] == true) {
	++hitCount;
      }
    }      
    hitRatio = (float)hitCount/measurementCycle;     
    
    // Now determine what cancellation strategy should be used from now on 
    determineMode();
    // if the cancellation mode is decided to be AGGRESSIVE then
    // suppressMessage = NONE because from now on we don't have to do
    // passive comparisions.
    if(currentMode == AGGRESSIVE) {
      suppressMessage = NONE;
    }
  }
#elif defined(PERMANENTLY_AGGR)
  if(!permanentlyAggressive) {
    if(noOfComparisions >= measurementCycle) {
      for(int i = 0; i < measurementCycle;i++) {
	if(comparisonResults[i] == true) {
	  ++hitCount;
	}
      }      
      hitRatio = (float)hitCount/measurementCycle;
      determineMode();
    }
  }
#else
  if(noOfComparisions >= measurementCycle) {
    for(int i = 0; i < measurementCycle;i++) {
      if(comparisonResults[i] == true) {
	++hitCount;
      }
    }      
    hitRatio = (float)hitCount/measurementCycle;
#ifdef STATS
    dcStatsFile << "@(" << state->current->lVT << ") hitCount = " 
	 << hitCount << " hitRatio = " << hitRatio << endl;
#endif
    determineMode();
  }
#endif
}
 
void 
LazyAggrTimeWarp::determineMode() {

  register CancellationMode nextMode = currentMode;
  register bool lazyToAggressive     = false;
  register bool aggressiveToLazy     = false;

#ifdef PERMANENTLY_AGGR
  if(INDEX < THIRD_THRESHOLD) {
    nextMode              = AGGRESSIVE;
    permanentlyAggressive = true;
    
    if(currentMode == LAZY) {

      // Discard all the anti-messages
      BasicEvent *cancelEvent, *negEvent;
      // The current ptr of lazyCancelQ has already been set to point to
      // the 1st event in this queue.
      cancelEvent = lazyCancelQ.get();
      while(cancelEvent != NULL) {
	int destID = cancelEvent->dest;
	negEvent = (BasicEvent *) new char[cancelEvent->size];
	negEvent->BasicEvent(cancelEvent);
	negEvent->sign = NEGATIVE;
	negEvent->size = sizeof(BasicEvent);
	TimeWarp::sendEventUnconditionally(negEvent);
	
	lazyCancelQ.removeCurrent();
	if(commHandle[destID].ptr->isCommManager()){
	  delete [] ((char *)negEvent);
	  delete [] (char*)cancelEvent;
	}
	cancelEvent = lazyCancelQ.get();
      }
 
      // We are done dispatching all the anti-messages at this point
      suppressMessage &= ~LAZYCANCEL;
    }
    // else don't bother
  }
  else {
#endif
    if((currentMode == AGGRESSIVE) && (INDEX > AGGRESSIVE_TO_LAZY)) {
      nextMode = LAZY;
      aggressiveToLazy = true;
#ifdef STATS
      dcStatsFile << "@(" << state->current->lVT 
	   << ") Aggressive Cancellation -----> Lazy Cancellation" << endl;
#endif
#ifdef PERMANENTLYSET
      if(!modePermanent) {
	suppressMessage &= ~LAZYAGGRCANCEL;
      }
#else
      suppressMessage &= ~LAZYAGGRCANCEL;
#endif
    }
    else if((currentMode == LAZY) && (INDEX < LAZY_TO_AGGRESSIVE)) {
      nextMode = AGGRESSIVE;  
      lazyToAggressive = true;
#ifdef STATS
      dcStatsFile << "@(" << state->current->lVT 
	   << ") Lazy Cancellation -----> Aggressive Cancellation" 
	   << endl
#endif
    }
    else {
    }
    
    // Dispatch all the anti-messages before changing to Aggressive
    if(lazyToAggressive) {
      BasicEvent *cancelEvent, *negEvent;
      // The current ptr of lazyCancelQ has already been set to point to
      // the 1st event in this queue.
      cancelEvent = lazyCancelQ.get();
      while(cancelEvent != NULL) {
	int destID = cancelEvent->dest;
	negEvent = (BasicEvent*) new char[cancelEvent->size];
	new (negEvent) BasicEvent(cancelEvent);
	negEvent->sign = NEGATIVE;
	negEvent->size = sizeof(BasicEvent);
	TimeWarp::sendEventUnconditionally(negEvent);
	
	lazyCancelQ.removeCurrent();
	if(commHandle[destID].ptr->isCommManager()){
	  delete [] ((char *)negEvent);
	  delete [] (char*)cancelEvent;
	}
	cancelEvent = lazyCancelQ.get();
      }
      suppressMessage &= ~LAZYCANCEL;
      //suppressMessage |= LAZYAGGRCANCEL; // This may have to be set to NONE
    }
    
    // Flush the lazyAggrQ.
    if(aggressiveToLazy) {
      BasicEvent *eventToRemove;
      // Reset currentPos
      lazyAggrQ.seek(0, START);
      eventToRemove = lazyAggrQ.get();
      while(eventToRemove != NULL) {
	lazyAggrQ.removeCurrent();
	delete [] (char *)eventToRemove;
	eventToRemove = lazyAggrQ.get();
      }
    }
#ifdef PERMANENTLY_AGGR
  }
#endif
  currentMode = nextMode; 
}
#endif
#endif




