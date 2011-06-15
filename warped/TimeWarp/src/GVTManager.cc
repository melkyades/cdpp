//-*-c++-*-
#ifndef GVT_MANAGER_CC
#define GVT_MANAGER_CC
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

// $Id: GVTManager.cc,v 1.13 1999/10/24 23:25:55 ramanan Exp $
//
//---------------------------------------------------------------------------

#include "GVTManager.hh"
#include "LogicalProcess.hh"
#include "SimulationTime.hh"
#include "bool.hh"
#ifndef MATTERNGVTMANAGER

GVTManager::GVTManager(){
  gVT = ZERO;
  lGVT = ZERO;
  needAcked.setFunc(AckRecordCompare);
  definedGVT = 0;
  lastReported = ZERO;
  lastIdle = false;
  lpIdle = false;
  tokenPending = false;
  noOfGVTToSkip = 0;
  gvtPeriod = 0;

  ranBetweenTokens = false ;
}


GVTManager::~GVTManager(){
  delete [] simArray;
  delete [] lastTimeSent;
  delete [] lastTimeReceived;
  delete [] dummyPending;

  if (id == 0) {
    delete [] lGVTArray;
    delete [] idleArray;
  }
}


void
GVTManager::initGVTManager( ObjectRecord* tempHandle,
			    CommMgrInterface* comm,
			    int myId,
			    int totalNum,
			    int numObj,
			    int numLogical) {
  
  commHandle = comm;        // handle to commManager
  id = myId;                // the id of the LP we're on
  totalObjects = totalNum;  // the number of simulation objects in the
  // entire system
  numObjects = numObj;  // number of objects ON THIS LP
  numLPs = numLogical;  // the number of LPs

  simArray = new ObjectRecord[numObjects];
  // This array only needs to be local to this LP
  int i=0;      // index variables
  int j=0;

  // we want our array to have ONLY the local objects in it since they
  // are the only ones we care about for gVT

  for(i=0;i<totalNum;i++){
    // if this object's LP id matches our LP id
    if(tempHandle[i].lpNum == id) {
      // then put an entry in our array for it
      simArray[j] = tempHandle[i];
      j++;
    }
  }
  
  if (id==0) {
    // Then I'm responsible for the actual gVT calculations.  I'd better
    // Allocate some space for the array of local estimates, and keep
    // track of the last LP to set gVT
    lGVTArray = new VTime[numLPs];
    idleArray = new bool[numLPs];
    for(i=0;i<numLPs;i++){
      lGVTArray[i] = ZERO;
      idleArray[i] = false;
    }
  }
  else {
    lGVTArray = NULL;
  }

  // all of this stuff is for PGVT without acknowledgement
  
  lastTimeSent = new VTime[numLPs];
  lastTimeReceived = new VTime[numLPs];

  for(i = 0; i < numLPs; i++){
    lastTimeSent[i] = PINFINITY;
    lastTimeReceived[i] = PINFINITY;
  }
  
  dummyPending = new bool[numLPs];
  for(i=0;i<numLPs;i++){
    dummyPending[i] = false;
  }
  lowestUnacknowledged = PINFINITY;
}

void 
GVTManager::deliver(BasicEvent* event) {
  commHandle->deliver(event);
}

void 
GVTManager::calculateLGVT( ){
  // we only need to do this if we were the critical path...
  if(lastReported == gVT ||
     (id == 0 && lastReported > LogicalProcess::SIMUNTIL) ){
    VTime newLGVT = PINFINITY;
    int i;
    bool allIdle = true;

    // First, calculate the simulation object's estimate
    for(i=0;i<numObjects;i++){
      if( simArray[i].ptr->iRanLastTime == true ||
	  simArray[i].ptr->inputQ.getObj(i) != NULL ){
	newLGVT = MIN_FUNC(newLGVT, inputQ.findPrevTime(i));
	if(newLGVT < gVT) newLGVT = gVT;
	allIdle = false;
	
	simArray[i].ptr->iRanLastTime = false;
      }
    }

#if defined(LAZYCANCELLATION) || defined(LAZYAGGR_CANCELLATION)
    // peek into the lazyCancelQ - we might have output messages which might
    // be less than(!!) the LVT when a GVT cycle is initiated.
     for(i=0; i < numObjects; i++){
       if(simArray[i].ptr->getLazyQSize() > 0){
 	newLGVT = MIN_FUNC(newLGVT, simArray[i].ptr->getLazyQMinTime());
	//cout << "simArray[" << i << "].ptr->getLazyQMinTime() = " 
  	//     << simArray[i].ptr->getLazyQMinTime() << endl;
        }
     }
#endif

    // Now, take into account the valley messages that we have that are
    // unacknowledged
    newLGVT = MIN_FUNC( newLGVT, lowestUnacknowledged );
#ifdef LPDEBUG
    *lpFile << "(check lowest) newLGVT 1 : " << newLGVT << endl;
#endif
    // now all of the messages on the outgoing AND incoming channel
    VTime preLoop = newLGVT;
    for ( i=0; i<numLPs; i++ ) {
      newLGVT = MIN_FUNC(newLGVT, lastTimeReceived[i]);
      newLGVT = MIN_FUNC(newLGVT, lastTimeSent[i]);    
    }
#ifdef LPDEBUG
    *lpFile << "(check channels) newLGVT 2 : " << newLGVT << endl;
#endif
    // here's the plan - send a dummy event, and receive it back to
    // clear the channel. When received, set last time received 
    // for that channel gets set to PINFINITY ( since the channel is fifo )
		
    static int count;
    if(preLoop > newLGVT) {
      count++;
      if(count > 100){
	for(i=0; i<numLPs; i++ ) {
	  if( dummyPending[i] == false && 
	      (lastTimeReceived[i] == newLGVT || lastTimeSent[i] == newLGVT )){
	    DummyRequestMsg *dummy = new DummyRequestMsg;
	    dummyPending[i] = true;
	    dummy->destLP = i;
	    sendEvent(dummy);
	    // now we need to set this communication channel's time to
	    // PINIFINITY - the whole point of the dummy stuff...
	    lastTimeSent[dummy->destLP] = PINFINITY;
	    delete dummy;
	  }
	}
	count = 0;
      }
    }
    else {
      count = 0;
    }
		
    setLGVT(newLGVT);
    
    // set lpIdle appropriately
    if( checkIdle() == true && needAcked.size() == 0 ){
      lpIdle = true;
      if(id == 0 && tokenPending == false && checkIdle() == true) {
	if (numLPs > 1) {
	  startToken();
	}
	else {
	  setGVT(PINFINITY);
	  newGVTMsg* gVTMsg = new newGVTMsg;
	  gVTMsg->newGVT = getGVT();
	  sendEvent(gVTMsg);
	  delete gVTMsg;
	}
      }
    }
    else {
      lpIdle = false;
    }
		
    if((newLGVT < lastReported)||(newLGVT < gVT && gVT != PINFINITY) ){
#ifdef LPDEBUG
      *lpFile << "LP " << id << ": something is foobar! newLGVT : " << newLGVT
	      << " lastReported :" << lastReported << " gVT : " << gVT << endl;
      for(int k = 0; k < numObjects;i++){
	*lpFile << inputQ.lVTArray[k] << " ";
      }
      *lpFile << endl ;
      needAcked.print();
#else
      cout << "LP " << id << ": something is foobar! newLGVT : " << newLGVT
	   << " lastReported :" << lastReported << " gVT : " << gVT << endl;
      needAcked.print();
#endif
      calcMin();
      terminateSimulation("something is wrong with GVT");
      abort();
    }
    
    if ( (lastReported == gVT)  &&
	 ( newLGVT != lastReported || lastIdle != lpIdle )) {
#ifdef LPDEBUG
      *lpFile << "lastReported : " << lastReported << " gVT : " << gVT
	      << "\nnewLGVT : " << newLGVT << endl;
#endif
      // we are (one of) the LP(s) furthest behind AND
      // either 1) we've not reported this new value
      // 2) we're idle, so we need to report it...
      // to send new GVT information.  We also need to make sure
      // that if all of our processes are idle, that we send 
      // a PINIFINITY message
      LGVTMsg* newMsg = new LGVTMsg;
      newMsg->lGVT = newLGVT;
      newMsg->lpIdle = lpIdle;
      sendEvent(newMsg);
      delete newMsg;
      lastReported = newLGVT;
      lastIdle = lpIdle;
    }
#ifdef LPDEBUG
    else {
      if(lastReported != gVT) {
	*lpFile << "lastReported : " << lastReported << " gVT : " << gVT << endl;
      }
      else if(lastReported == newLGVT) {
	*lpFile << "already reported : " << newLGVT << endl;
      }
    }
#endif
  }
}

void 
GVTManager::calculateGVT(){
  // Note! this should only get called in LP #0
  int i;
  static int gvtcount2 = 0;
  static int gvtcount = 0;
  static VTime avgGVTinc = 0;
  VTime oldGVT = getGVT();
  VTime newGVT = PINFINITY;
  bool simDone = true;

  for(i=0;i<numLPs;i++){
    VTime tempGVT = newGVT;
    newGVT = MIN_FUNC(newGVT, lGVTArray[i]);
    if(newGVT < tempGVT) {
      definedGVT = i;   // this guy (or guys) defined gVT
    }
    if(idleArray[i] == false) {
      simDone = false;
    }
  }

  if(newGVT > oldGVT){
    gvtcount++; 
    if (gvtcount > 2) {
      avgGVTinc = avgGVTinc * (gvtcount - 1) /gvtcount + 
	(newGVT - oldGVT)/gvtcount;
    }
    setGVT(newGVT);
    newGVTMsg* gVTMsg = new newGVTMsg;
    gVTMsg->newGVT = getGVT();
    sendEvent(gVTMsg);
    delete gVTMsg;
    //setGVT(newGVT); extraneous call
    gvtcount2++;
    if (gvtcount2 == 1000) {
      cout << "gVT: " << getGVT() << "\n";
      gvtcount2 = 0;
    }
  }
  
}

void
GVTManager::terminateSimulation(char *msg){
  TerminateMsg termMsg;
  strcpy(termMsg.error, msg);
  commHandle->recvMsg(&termMsg);
}

bool
GVTManager::checkIdle(){

  bool allIdle = true;

  // First, calculate the simulation object's estimate
  if(simArray[0].ptr->inputQ.get() != NULL
     // Because if end time is reached the process is also idle
     && simArray[0].ptr->inputQ.get()->recvTime <= LogicalProcess::SIMUNTIL){
    allIdle = false;
  }
  if(allIdle == true && needAcked.size() == 0){
    lpIdle = true;
  }
  else {
    // I do this because if allIdle is true then let's set that sucker
    // to false. I hope this doesn't break anything else.
    lpIdle = false;
  }
  return lpIdle;
}

void 
GVTManager::startToken(){
  static int tokenCount;
  tokenCount++;
  lpIdle.reset();

  currentToken = tokenCount;
  
  CheckIdleMsg *checkIdle = new CheckIdleMsg;
  checkIdle->senderLP = 0;
  checkIdle->destLP = 1;
  checkIdle->tokenNum = tokenCount;
  checkIdle->numCirculations = 0;
  checkIdle->cancel = false;

  sendEvent( checkIdle );
#ifdef LPDEBUG
  DEBUGSTREAM << id << " dispatching idle token at time " 
	      << simArray[0].ptr->getLVT() << endl;
#endif
  tokenPending = true;
  delete checkIdle;
}



VTime
GVTManager::getGVT(){
  return gVT;
}


void
GVTManager::setGVT(VTime newGVT){

 gVT = newGVT;

#ifdef LPDEBUG
  DEBUGSTREAM << "Setting gVT to " << gVT << "\n";
#endif
}


void
GVTManager::setLGVT(VTime newLGVT){
  lGVT = newLGVT;
}


VTime
GVTManager::getLGVT(){
  return lGVT;
}

VTime
GVTManager::gcollect() {
  register int i;
  VTime lowestTime, restoredTime;

  BasicEvent** inputQptr = new BasicEvent*[numObjects];
  Container<BasicEvent>** outputQptr = new Container<BasicEvent>*[numObjects];
#ifdef LPDEBUG
  *lpFile << "GVTManager gcollecting for gVT:" << gVT << endl;
#endif

  lowestTime = gVT;

  for (i = 0; i < numObjects; i++) {
    restoredTime = 
      simArray[i].ptr->stateGcollect(lowestTime, inputQptr[i], outputQptr[i]);
    
#ifdef LPDEBUG
  *lpFile << simArray[i].ptr->name 
	    << " Saving state before gvt with timestamp " << restoredTime;
    if (simArray[i].ptr->iRanLastTime == true) {
      *lpFile << " & I ran last time ";
    }
    if (simArray[i].ptr->peekInputQ() == NULL ) {
      *lpFile << " & inputQ is NULL";
    }
    *lpFile << endl;
#endif
//      if( simArray[i].ptr->iRanLastTime == true ||
//  	simArray[i].ptr->peekInputQ() != NULL ||
//  	simArray[i].ptr->useTimeForGcollect() == true) {
//        lowestTime = MIN_FUNC(restoredTime, lowestTime);
//      }
  }
  
  // we'll call clear function of stateQueue here (instead of after 
  // inputgcollect) to null out
  // the outputQPtrs of the events we are going to gcollect in the
  // outputQueue. This saves us an FMR.
  for (i = 0; i < numObjects; i++) {
    simArray[i].ptr->stateClear();
  }

#ifdef LPDEBUG
  *lpFile << "lowest time among states "  << lowestTime << endl;
#endif

  for (i = 0; i < numObjects; i++) {
    simArray[i].ptr->outputGcollect(lowestTime, outputQptr[i]);
  }
  
  
  // The commManager needs to know that he can gcollect as well...
  commHandle->gcollect(lowestTime);
  
  
  for (i = 0; i < numObjects; i++) {
    simArray[i].ptr->inputGcollect(lowestTime, inputQptr[i]);
  }

  delete [] inputQptr;
  delete [] outputQptr;
  return(lowestTime);
}


void
GVTManager::recvMsg( newGVTMsg *msg ){

  static int noOfGVTRecv = 0;
  setGVT(msg->newGVT);
  // Find the lowest state before GVT and only gcollect for times greater
  // than that on this LP, since we pass pointers to events between
  // simulation objects on the same LP. This is only necessary for periodic
  // checkpointing.
  // this has to be done in two loops, because events are passed as pointers
  // as opposed to copies of the data.  Conceptually, the outputQ contains
  // the data, and the inputQ can contain pointers into another object's
  // outputQ.  inputGcollect cleans up the inputQ containers, and gcollect
  // cleans up containers and stored data in the stateQ and outputQ.
  noOfGVTRecv ++;
  if (noOfGVTRecv > noOfGVTToSkip){
    noOfGVTRecv = 0;
    gcollect();
  }

}


void
GVTManager::sendEvent(LGVTMsg  *msg){
  msg->senderLP = id;
  msg->destLP = 0;
  commHandle->recvMsg(msg);
}


void
GVTManager::sendEvent(newGVTMsg  *msg){
  commHandle->recvMsg(msg);
}

void
GVTManager::sendEvent(CheckIdleMsg  *msg){
  commHandle->recvMsg(msg);
}

void
GVTManager::sendEvent( BasicEvent* event){
  cout << "called sendEvent(BasicEvent* ) in GVTManager! : " 
       <<  *event << endl;
}  

void
GVTManager::sendEvent( DummyMsg* dummy){
  commHandle->recvMsg(dummy);
}

void
GVTManager::sendEvent( DummyRequestMsg* dummy){
  commHandle->recvMsg(dummy);
}

void
GVTManager::sendEvent( TerminateMsg* terminate){
  commHandle->recvMsg( terminate );
}

BasicEvent*
GVTManager::getEvent(){
  cout << "called getEvent in GVTManager!" << endl;
  return (BasicEvent*)NULL;
}  

void
GVTManager::simulate(){
  cout << "called simulate in GVTManager!" << endl;
}  

VTime
GVTManager::calculateMin(){
  cerr << "called calculateMin() in GVTManager!" << endl;
  return ZERO;
}

void
GVTManager::recvEvent(BasicEvent *event){
  cerr << "called recvEvent(BasicEvent  *) in GVTManager! :"
       << *event << endl;
}

void
GVTManager::executeProcess(){
  cerr << "called executeProcess() in GVTManager!" << endl;
}

void
GVTManager::msgRecvPeek( EventMsg* eventMsg) {

  // we need to check if this is a valley message from the LP
  // that sent the message 
  if(eventMsg->recvTime < lastTimeReceived[eventMsg->senderLP]) {
    
    // either it's going lower our lGVT and we'll need to report to the
    // head gVT guy before confirming that we are taking responsibility
    // for this message, or we'll just need to confirm...
    
    // I'm not sure if this test is enough...
    if( eventMsg->recvTime < lastReported ) {
      // uh oh...  we'd better let the head gVT guy know that we're 
      // about lower our estimate.
      lGVT = eventMsg->recvTime;
      LowerLGVTMsg* newLGVTMsg = new LowerLGVTMsg;
      newLGVTMsg->lGVT = lGVT;
      newLGVTMsg->destLP = 0;
      newLGVTMsg->lpIdle = false;
      // now that we've got the message ready to send to the gVT guy, and
      // the event in hand that's causing us problems, lets create a record
      // so we can send a message back to the other LP later...
      AckRecord* newRecord = new AckRecord;
      newRecord->requestSequence = commHandle->getSequence(0);
      newRecord->eventSequence = eventMsg->sequence;
      newRecord->eventTime = eventMsg->recvTime;
      newRecord->lpId = eventMsg->senderLP;
      
      // now we need to remember that we're responsible for this message
      // so we'll add it to our list of messages waiting for acknowledgment
      needAcked.insert( newRecord );
#ifdef LPDEBUG
      *lpFile << "GVTManager " << id << ": need ack for (spot 1) " 
	      << *newRecord << endl;
#endif
      // (we'll send the ack when we get something back from the head GVT
      // guy...  That's in recvMsg( GVTAckMsg * )
      
      // we'd better note for future gVT calculations that this is our lowest
      // unacknowledged message.  While this is not STRICTLY necessary (since
      // the other guy is waiting for our acknowledgement, it makes Dale
      // feel more comfortable that it is here...
      lowestUnacknowledged = MIN_FUNC( lowestUnacknowledged, 
				       eventMsg->recvTime);   
#ifdef LPDEBUG
      *lpFile << "lowestUnacknowledged = " << lowestUnacknowledged << endl; 
#endif
      commHandle->recvMsg( newLGVTMsg );
      lastReported = newLGVTMsg->lGVT;
      delete newLGVTMsg;
            
    } // if( eventMsg->recvTime < lGVT )
    else {
      // the gVT guy for some other LP thinks that we are going to send him
      // acknowledgement for this message.  We'll have to do it, even though
      // we aren't sending anything to the head gVT guy....
      LPAckMsg * ackMsg = new LPAckMsg;
      ackMsg->destLP = eventMsg->senderLP;
      ackMsg->sequenceMsgAcked = eventMsg->sequence;

      commHandle->recvMsg( ackMsg );

      delete ackMsg;
    } // eventMsg->recvTime isn't < lGVT
  } // if it's a valley message
  // set the time of this message as the last time received from this LP
  lastTimeReceived[eventMsg->senderLP] = eventMsg->recvTime;
}


void
GVTManager::msgSendPeek( EventMsg* eventMsg ){
  if( eventMsg->recvTime < lastTimeSent[eventMsg->destLP] ){
    // then it's a valley message! ruh roh raggie!
    // we need to assume responsibility of this message for a little bit
    // let's check and see if it's the earliest unacknowledged valley 
    // message we have
    if( eventMsg->recvTime < lowestUnacknowledged ) {
      lowestUnacknowledged = eventMsg->recvTime;
    }
    // now we need to keep track of the fact that we're waiting for
    // acknowledgement on this message
    AckRecord* ackRecord = new AckRecord;

    // this data isn't really needed except for the fact that it is a
    // key value for insertion into the list.  Both fields having to
    // do with sequence number are the same
    ackRecord->requestSequence = eventMsg->sequence;
    //    ackRecord->requestSequence = commHandle->getSequence(eventMsg->destLP);
    ackRecord->eventSequence = eventMsg->sequence;
    ackRecord->lpId = eventMsg->senderLP;
    ackRecord->eventTime = eventMsg->recvTime;
    needAcked.insert(ackRecord);
#ifdef LPDEBUG
    *lpFile << "GVTManager " << id << ": need ack for (spot 2) " 
	    << *ackRecord << endl;
#endif
  }
  lastTimeSent[eventMsg->destLP] = eventMsg->recvTime;
}

void
GVTManager::recvMsg( GVTAckMsg * gVTAck ){
  // we're getting acknowledgement from the head honcho that he's
  // updated his lGVT for us.  Now we can send a message back to the LP
  // who lowered our lGVT, saying we've got it under control...

  // Let's look up the LP who we want to update...
  AckRecord key;
  // the records are sorted by MY sequence number... let's get the
  // number from the msg that we received...
  key.requestSequence = gVTAck->sequenceMsgAcked;
  
  AckRecord* recordFound;
  recordFound = needAcked.find(&key);
  if(recordFound == NULL){
    cout << "[1]GVT Manager " << id <<
      " got gVTAcked for a message that I don't have a record for:\n";
    cout << "Mesg : " << *gVTAck << "\n";
    cout << "List of unacked messages :\n";
    needAcked.print();
    inputQ.print();
    terminateSimulation("I got acked for a message I have no record of");
  }
  else {
    // we've got the message, let's generate the acknowledgement for the
    // LP who sent us the valley!
    LPAckMsg * lpAck = new LPAckMsg;
    lpAck->sequenceMsgAcked = recordFound->eventSequence;
    lpAck->destLP = recordFound->lpId;
    // that's it - now we just need to send it
#ifdef LPDEBUG
    *lpFile << "GVTManager " << id << "Ack for (spot 2) " 
	    << *lpAck << endl;
#endif
    commHandle->recvMsg(lpAck);
    delete lpAck;

    // we also need to get rid of our entry in our list for this guy, and
    // delete the record
    needAcked.removeFind();
    delete recordFound;

    lowestUnacknowledged = PINFINITY;
    // now we need to find the next earliest message to worry about being
    // unacknowledged
    AckRecord* current = needAcked.seek(0, START);
    while( current != NULL ){
      lowestUnacknowledged = 
	MIN_FUNC(lowestUnacknowledged, current->eventTime);
      current = needAcked.seek(1, CURRENT);
    }  // while ( current!=NULL)
  } // recordFound!=NULL
} // recvMsg ( GVTAckMsg * )

void
GVTManager::recvMsg( LPAckMsg * lpAck ) {

  // some LP is telling us that he has taken responsiblity for some valley
  // message that we sent in the past
  AckRecord key;
  // the records are sorted by MY sequence number... let's get the
  // number from the msg that we received...
  key.requestSequence = lpAck->sequenceMsgAcked;
  AckRecord* recordFound;
  recordFound = needAcked.find(&key);
  if(recordFound == NULL){
    cout << "[2]GVT Manager " << id <<
      " got lpAcked for a message that I don't have a record for:\n";
    cout << "Mesg : " << *lpAck << "\n";
    cout << "List of unacked messages :";
    needAcked.print();
    terminateSimulation("I got acked for a message I have no record of");
  }
  else {
    // we also need to get rid of our entry in our list for this guy, and
    // delete the record
    needAcked.removeFind();
    delete recordFound;

    lowestUnacknowledged = PINFINITY;
    // now we need to find the next earliest message to worry about being
    // unacknowledged
    AckRecord* current = needAcked.seek(0, START);
    while( current != NULL ){
      lowestUnacknowledged = 
	MIN_FUNC(lowestUnacknowledged, current->eventTime);
      current = needAcked.seek(1, CURRENT);
    }  // while ( current!=NULL)
  } // recordFound!=NULL
} // recvMsg ( lpAckMsg * )

void
GVTManager::recvMsg( LowerLGVTMsg *lGVTMsg ){
  // Receiving this kind of message tells us that the sending LP needs 
  // acknowledgment back... So we'll update the lGVT array, and send back
  // a message...
#ifdef LPDEBUG
  *lpFile << "LP " << id << " lowered his lGVT !! " << endl;
#endif
  lGVTArray[lGVTMsg->senderLP] = lGVTMsg->lGVT;
  idleArray[lGVTMsg->senderLP] = lGVTMsg->lpIdle;
  // now we will generate the actual response
  GVTAckMsg * gVTMsg = new GVTAckMsg;
  gVTMsg->sequenceMsgAcked = lGVTMsg->sequence;
  gVTMsg->destLP = lGVTMsg->senderLP;
  commHandle->recvMsg( gVTMsg );
  // the CommManager copied it if it needs to be copied
  delete gVTMsg;
  // there is no chance in this case that gVT changed, since this guy is
  // LOWERING his estimate...  Also, there is no chance that he could be
  // the only gVT setter at this point.  Otherwise, gVT would have to be
  // going backwards!
}

void
GVTManager::recvMsg( LGVTMsg* lGVTMsg ){
  // this is a regular update from this guy...  We'll put his new value
  // into the lGVTArray, and if we need to, we'll calculate a new gVT
#ifdef LPDEBUG
  *lpFile << "LGVT Message : " << *lGVTMsg << endl ;
#endif
  if( lGVTMsg->lGVT <  lGVTArray[lGVTMsg->senderLP] ) {
    cerr << "LP " << lGVTMsg->senderLP <<
      " lowering his lGVT with a regular update " << endl;
  }
  lGVTArray[lGVTMsg->senderLP] = lGVTMsg->lGVT;
  idleArray[lGVTMsg->senderLP] = lGVTMsg->lpIdle;
  if(lGVTMsg->senderLP == definedGVT){
    // then we DO need to calculate GVT
    calculateGVT();  // at the moment this sends out a new message as well
    // this might not be the best idea...
  }
}

void
GVTManager::recvMsg( CheckIdleMsg *idleMsg ){
  // a token is flowing around the ring...  If it has come around twice,
  // and we're process 0, then we can safely terminate the simulation.
  // If we're not process zero, we check if we're idle or not, and if
  // so, forward the token...  Otherwise, we send an lGVT message back to
  // the head honcho with our idle status in it...

// a flag ranBetweenTokens is now added to GVTManager class.
// This flag is set to false whenever a lp 
// gets the termination detection token for the first time and the lp is idle.
// After setting it to false the lp forwards it to the next lp. When the token 
// comes around for the second time and if the lp is idle this flag is checked
// if it is true that means that some object on that lp ran after the first 
// visit of the token (even if the lp is now idle). if the flag is false (and 
// the lp is idle) the lp did not run after the token's first visit and the 
// token is forwarded to the next lp. In all other conditions the token is 
// cancelled and returned to lp0
  
  if(id == 0 && idleMsg->cancel == true ){
    tokenPending = false;
    idleArray[idleMsg->senderLP] = false;
  }
  else {
    lpIdle = checkIdle();
    if (lpIdle == true) {
      // is this token on its first round?
      if(idleMsg->numCirculations == 0){
	// this is the first time around. Definitely pass it on...
	currentToken = idleMsg->tokenNum;
	lpIdle.reset();
	
	ranBetweenTokens = false ;
	
	if(id == 0){
	  idleMsg->numCirculations++;
	}
	passItOn(idleMsg);
#ifdef LPDEBUG
	DEBUGSTREAM << id << " Passing Token 1st round at time " 
		    << simArray[0].ptr->getLVT() << endl;
#endif
      }
      else {
	// the token is on it's second loop - have we gone active since
	// it came around the first time?
	if( idleMsg->tokenNum == currentToken &&
	    lpIdle.didItChange() == true) {
	  // ruh roh - we went active since the first pass - time 
	  // to start the whole mess over...
	  cancelToken(idleMsg);
#ifdef LPDEBUG
	  DEBUGSTREAM << id << " Cancelling Token 2nd round at time " 
		      << simArray[0].ptr->getLVT() << endl;
#endif
	}
	else if ( idleMsg->tokenNum != currentToken ) {
	  // this is an old token...  We'd better delete it and wait
	  // for the new one!
	  //	  delete idleMsg;
	  cout << "wugga wugga!  I shouldn't be here!" << endl;
	}
	else if(ranBetweenTokens == true)
	  {
	    cancelToken(idleMsg);
	  }
	else {
	  // we've received this token before, and we haven't gone idle
	  // since the first round...  Pass it on!
	  if(id != 0){
	    passItOn(idleMsg);
#ifdef LPDEBUG
	    DEBUGSTREAM << id  << " Passing Token 2nd round at time " 
			<< simArray[0].ptr->getLVT() << endl;
#endif

              }
	  else {

            newGVTMsg *gVTMsg = new newGVTMsg;
            gVTMsg->newGVT = PINFINITY;
            sendEvent( gVTMsg );
            delete gVTMsg;
  
#ifdef LPDEBUG
	    DEBUGSTREAM << "Sending out the PINFINITY message at time " 
			<< simArray[0].ptr->getLVT() << endl;
	    for (int i=0; i < numLPs; i++ ) {
	      DEBUGSTREAM << "lastTimeReceived[" << i << "] = " 
			  << lastTimeReceived[i] << endl; 
	      DEBUGSTREAM << "lastTimeSent[" << i << "] = " 
			  << lastTimeSent[i] << endl; 
	    }

	    // one last check before we terminate
	    int lastCheckForIdle = 0;
	    for (int i=0; i < numLPs; i++ ) {
	      if((lastTimeReceived[i] == PINFINITY) && 
		 (lastTimeSent[i] == PINFINITY)){
		lastCheckForIdle++;
	      }
	    }

	    if(lastCheckForIdle != numLPs){
	      cancelToken(idleMsg);
	    }
	    else{
	      newGVTMsg *gVTMsg = new newGVTMsg;
	      gVTMsg->newGVT = PINFINITY;
	      sendEvent( gVTMsg );
	      delete gVTMsg;
	    }
#endif
	  }
	}  // we got a token the second time and passed it on
      } // the token is on the second loop
    } // if we're idle
    else {
#ifdef LPDEBUG
      DEBUGSTREAM << "Cancelling token (not idle)\n"
		  << "InputQ size = " <<  simArray[0].ptr->inputQ.size()
		  << "\n need " << needAcked.size() 
		  << " Acknowledgements" << endl;
#endif
      // we're not idle!
      cancelToken(idleMsg);
    }  // we're not idle
  } // we're not zero or this wasn't a cancel messgae
} // receive a check idle token

void
GVTManager::recvMsg( DummyRequestMsg *dummy){
  // the Comm guy is handing us a dummy request...  We need to send back
  // a dummy response of our own...
  DummyMsg *response = new DummyMsg;
  response->destLP = dummy->senderLP;
  // now we can modify our outgoing channel for that guy with PINIFINITY
  lastTimeReceived[response->destLP] = PINFINITY;
  lastTimeSent[response->destLP] = PINFINITY;
  sendEvent(response);
  delete response;
}

void
GVTManager::recvMsg( DummyMsg *dummy ){
  // the comm guy has given us a response from a dummy request...  we
  // can now set the channel that we received this message from to 
  // PINFINITY since we know that the channel has been cleared (and if he
  // sends us a lower message in the meantime, he is treating it as a 
  // valley now...
  lastTimeReceived[dummy->senderLP] = PINFINITY;
  // since we no longer have a dummy message pending on this channel,
  // we should note that as well...
  dummyPending[dummy->senderLP] = false;
}

void
GVTManager::passItOn( CheckIdleMsg *idleMsg ){
  if( id != numLPs - 1){
    idleMsg->destLP++;
    sendEvent(idleMsg);
    // this is still the idle message that we originally received from
    // someone else - we'll let the CommManager delete it...

    // delete idleMsg;

  } // if we not the last guy in the loop
  else {
    // send it back to the beginning of the loop...
    idleMsg->destLP = 0;
    sendEvent(idleMsg);

    // this is still the idle message that we originally received from
    // someone else - we'll let the CommManager delete it...

    // delete idleMsg;

  } // if we're the last guy in the loop
#ifdef LPDEBUG
  *lpFile << "passItOn : passed it on " << endl;
#endif
} // if the token is on its first loop...

void
GVTManager::cancelToken( CheckIdleMsg *idleMsg){
  // we got a checkidle token and we weren't idle... not good...
  idleMsg->destLP = 0;
  idleMsg->cancel = true;
#ifdef LPDEBUG
  *lpFile << "CancelToken : idleMsg sent " << endl;
#endif
  sendEvent(idleMsg);
}

void
GVTManager::setFile( ofstream *file ){
  lpFile = file;
}

void 
GVTManager::calcMin(){
  // figure out WHO caused LGVT to be what it is...
  VTime newLGVT = PINFINITY;
  int i;

  int lowestSimObj = 0;
  // First, calculate the simulation object's estimate
  for(i=0;i<numObjects;i++){
    if( simArray[i].ptr->iRanLastTime == true ||
	simArray[i].ptr->peekInputQ() != NULL ){
      
      VTime lastLGVT = newLGVT;
      
      newLGVT = MIN_FUNC(newLGVT, simArray[i].ptr->calculateMin());
      
      if(newLGVT < lastLGVT){
	lowestSimObj = i;
      }
      
      simArray[i].ptr->iRanLastTime = false;
    }
  }
  VTime afterSimObjs = newLGVT;

  // Now, take into account the valley messages that we have that are
  // unacknowledged
  newLGVT = MIN_FUNC( newLGVT, lowestUnacknowledged );

  VTime afterLowestUnack = newLGVT;
  int lowestChannel = MAXINT;

  // now all of the messages on the outgoing AND incoming channel
  for ( i=0; i<numLPs; i++ ) {
    VTime lastLGVT = newLGVT;
    newLGVT = MIN_FUNC(newLGVT, lastTimeReceived[i]);
    newLGVT = MIN_FUNC(newLGVT, lastTimeSent[i]);    
    if(newLGVT < lastLGVT){
      lowestChannel = i;
    }
  }
  VTime afterCommChannels = newLGVT;

  if(newLGVT == afterSimObjs){
    DEBUGSTREAM << simArray[lowestSimObj].ptr->name
		<< " was lowest " << endl;
    
    if( simArray[lowestSimObj].ptr->peekInputQ() == NULL ){
      DEBUGSTREAM << "No events scheduled" << endl;
    }
    else {
      DEBUGSTREAM << "Events to be scheduled" << endl;
    }

    if( simArray[lowestSimObj].ptr->iRanLastTime == true ){
      DEBUGSTREAM << "iRanLastTime : true" << endl;
    }
    else if( simArray[lowestSimObj].ptr->iRanLastTime == false )  {
      DEBUGSTREAM << "iRanLastTime : false" << endl;
    }
    else{
      DEBUGSTREAM << "iRanLastTime : undefined" << endl;
    }

    // then lGVT was set by object lowestSimObj...
    // lets see what caused that!
    VTime min1 = simArray[lowestSimObj].ptr->inputQ.calculateMin();
    if (min1 == newLGVT) {
      DEBUGSTREAM << "inputQ.calculate() : " << min1 << endl;
    }
    VTime min2 = simArray[lowestSimObj].ptr->getLVT();
    if (min2 == newLGVT) {
      DEBUGSTREAM << "getLVT() : " << min2 << endl;
    }
    if(min1!=newLGVT && min2!=newLGVT){
      DEBUGSTREAM << "rollbackTime must be the reason : newLGVT : " << newLGVT
		  << endl;
    }
  }
  else if (newLGVT == afterLowestUnack){
    DEBUGSTREAM << "lowest unacked : " << newLGVT << endl;
  }
  else if (newLGVT == afterCommChannels){
    DEBUGSTREAM << "LP" << lowestChannel << "'s channel : " <<
      lastTimeReceived[lowestChannel] << endl;
  }
  else {
    DEBUGSTREAM << "I'm befuddled!" << endl;
  }

}



// the following two functions are for setting and returning the value
// of the flag ranBetweenTokens.This flag is set to false whenever a lp 
// gets the termination detection token for the first time and the lp is idle.
// After setting it to false the lp forwards it to the next lp. When the token 
// comes around for the second time and if the lp is idle this flag is checked
// if it is true that means that some object on that lp ran after the first 
// visit of the token (even if the lp is now idle). if the flag is false (and 
// the lp is idle) the lp did not run after the token's first visit and the 
// token is forwarded to the next lp. In all other conditions the token is 
// cancelled and returned to lp0
 
void 
GVTManager::setRanBetweenTokens(bool flag)
{
  ranBetweenTokens = flag ;
}

bool
GVTManager::giveRanBetweenTokens()
{
  return ranBetweenTokens ;

}

BasicState*
GVTManager::allocateState() {
  cerr << "Error::GVTManager::allocateState() called";
  abort();

  return NULL;
}

#endif
#endif // not MATTERNGVTMANAGER
