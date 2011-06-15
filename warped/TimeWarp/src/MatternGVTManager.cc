//-*-c++-*-
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
// $Id: MatternGVTManager.cc,v 1.10 1999/10/24 23:26:09 ramanan Exp $
//
//---------------------------------------------------------------------------
#include "MatternGVTManager.hh"
#ifdef MATTERNGVTMANAGER
#include "SimulationTime.hh"
#include "LogicalProcess.hh"

void 
MatternGVTManager::initGVTManager( ObjectRecord* tempHandle,
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
  gvtCycle = 0;
  gVTTokenNum = 0;
  gVTTokenPending = false;
  colorAlreadyChanged = false;
  gvtDiffSum = 0;
  avgGVTUpdate = 0;
  minGVTUpdate = PINFINITY;
  maxGVTUpdate = 0;
}

void 
MatternGVTManager::msgRecvPeek( EventMsg* eventMsg) {
  // we don't have to peek at messages for process 0 since
  // it will always be the leader.
  if (id != 0 && eventMsg->color == simArray[0].ptr->color + 1) {
    colorAlreadyChanged = true;
    int j;
#ifdef LPDEBUG
    DEBUGSTREAM << "Changing color to " << eventMsg->color << "\n";
#endif
    for( j=0;j<numObjects;j++){
      simArray[j].ptr->color = eventMsg->color;
      if (eventMsg->color == RED) {
	simArray[j].ptr->tMin = PINFINITY;
      }
    }
  }
}

void 
MatternGVTManager::deliver(BasicEvent* event) {
  commHandle->deliver(event);
}

void 
MatternGVTManager::calculateLGVT( ){
  // okay, time to start off the gvt token
  // check to see that this is LP0-only LP0 can be the initiator
  VTime newLGVT = PINFINITY;
  bool allIdle = true;
  
  if(id == 0){
    if(numLPs > 1 ){
      if(gVTTokenPending == false){
	startGVTToken(); // sending gvt token into the ring
	allIdle = false;
      }
    }
    else {
      // First, calculate the simulation object's estimate
      for(int i=0;i<numObjects;i++){
	if( simArray[i].ptr->iRanLastTime == true ||
	    simArray[i].ptr->peekInputQ() != NULL ){
	  newLGVT = MIN_FUNC(newLGVT, inputQ.findPrevTime(i));
	  //	  newLGVT = MIN_FUNC(newLGVT,simArray[i].ptr->getTimeOfLastSavedState());
	  if(newLGVT < gVT) newLGVT = gVT;
	  allIdle = false;
	  
	  simArray[i].ptr->iRanLastTime = false;
	}
      } 
      setGVT(newLGVT);
      if(gVT > ZERO) {
	calculateGVT();
	gcollect();
      }
    }
  }
  else {
    allIdle = true;
    for(int i=0;i<numObjects;i++){
      if( simArray[i].ptr->peekInputQ() != NULL){
	allIdle = false;
	simArray[i].ptr->iRanLastTime = false;
      }
    }
  }
  if(id == 0 && tokenPending == false && checkIdle() == true) {
    if(numLPs > 1){
      startToken();
    }
    else {
      gVT = PINFINITY;
    }
  }
  else {
    if(simArray[0].ptr->peekInputQ() == NULL){
      lpIdle = true;
    }
  }
  
}

void 
MatternGVTManager::calculateGVT(){
  // Note! this should only get called in LP #0
  static int gvtcount = 0;
  gvtcount++;
  if (gvtcount == 50) {
    cout << "gVT: " << getGVT() << "\n";
    gvtcount = 0;
  }
}

void
MatternGVTManager::terminateSimulation(char *msg){
  TerminateMsg termMsg;
  strcpy(termMsg.error, msg);
  commHandle->recvMsg(&termMsg);
}

bool
MatternGVTManager::checkIdle(){

  bool allIdle = true;

  // First, calculate the simulation object's estimate
  if(simArray[0].ptr->inputQ.get() != NULL
     // Because if end time is reached the process is also idle
     && simArray[0].ptr->inputQ.get()->recvTime <= LogicalProcess::SIMUNTIL ){
    allIdle = false;
  }
  if(allIdle == true ){
    lpIdle = true;
  }
  else {
    // I do this because if allIdle is true then let's set that sucker
    // to false. I hope this doesn't break anything else.
    lpIdle = false;
  }
  
  return lpIdle;
}

// this is the function which sends out the control message to the 
// other LPs
void 
MatternGVTManager::startGVTToken(){
  VTime newLVT = PINFINITY;
  int localCount=0;

  MatternGVTMsg *checkGVT = new MatternGVTMsg;
  for(int i=0;i<numObjects;i++){
    localCount = localCount + simArray[i].ptr->whiteMessages;
    simArray[i].ptr->color = RED; // change to red
    simArray[i].ptr->tMin = PINFINITY;
    if( simArray[i].ptr->peekInputQ() != NULL ){
      newLVT = MIN_FUNC(newLVT, inputQ.findPrevTime(i));
    }
    if(newLVT < gVT) {
      newLVT = gVT;
    }
    simArray[i].ptr->whiteMessages = 0;
  }
  checkGVT->destLP = 1; // for now assume LP0 starts this always
  checkGVT->tokenNum = gVTTokenNum++;
  checkGVT->mClock = newLVT; 
  checkGVT->mSend = PINFINITY; // specified by the algorithm
  checkGVT->count = localCount; // initialization.
  sendEvent(checkGVT);
#ifdef LPDEBUG
  DEBUGSTREAM << "StartGVTToken called : " << endl;
  DEBUGSTREAM << "sending token : " << *checkGVT << endl;
#endif
  gVTTokenPending = true; // we have to wait for this guy to go through
  delete checkGVT;
}
#ifdef EXTERNALAGENT
void 
MatternGVTManager::startExternalAgent(){

  ExtAgentMsg* activateMsg = new ExtAgentMsg;
  activateMsg->destLP = 1; // for now assume LP0 starts this always
  activateMsg->eventsSoFar = 0;
  tokenAlreadySent = true;
  sendEvent(activateMsg);
 
}
#endif
void 
MatternGVTManager::startToken(){
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
  tokenPending = true;
  delete checkIdle;
}



VTime 
MatternGVTManager::getGVT(){
  return gVT;
}


void
MatternGVTManager::setGVT(VTime newGVT){
#ifdef STATS
  if (newGVT != PINFINITY) {
    gvtDiffSum = gvtDiffSum + (newGVT - gVT);
    gvtCycle++;
    avgGVTUpdate = gvtDiffSum/gvtCycle;
    if (minGVTUpdate > newGVT - gVT) {
      minGVTUpdate = newGVT - gVT;
    }
    if (maxGVTUpdate < newGVT - gVT) {
      maxGVTUpdate = newGVT - gVT;
    }
      
  }
#endif
  gVT = newGVT;

#ifdef LPDEBUG
  DEBUGSTREAM << "Setting gVT to " << gVT << "\n";
#endif
}


void
MatternGVTManager::setLGVT(VTime newLGVT){
  lGVT = newLGVT;
}


VTime
MatternGVTManager::getLGVT(){
  return lGVT;
}

VTime
MatternGVTManager::gcollect() {
  register int i;
  VTime lowestTime, restoredTime;
  BasicEvent** inputQptr = new BasicEvent*[numObjects];
  Container<BasicEvent>** outputQptr = new Container<BasicEvent>*[numObjects];
  lowestTime = gVT;
#ifdef STATS
  gcollectCalled++;
#endif

#ifdef LPDEBUG
  *lpFile << "LP " << simArray[0].lpNum 
	  << "'s GVTManager gcollecting for gVT:" << gVT << endl;
#endif
#ifdef STATEDEBUG
  cout << "LP " << simArray[0].lpNum 
       << "'s GVTManager gcollecting for gVT:" << gVT << endl;
#endif
  for (i = 0; i < numObjects; i++) {
    restoredTime = 
      simArray[i].ptr->stateGcollect(gVT, inputQptr[i], outputQptr[i]);
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
    lowestTime = MIN_FUNC(restoredTime, lowestTime);
  }
#ifdef LPDEBUG
  *lpFile << "lowest time amoung states "  << lowestTime << endl;
#endif
  for (i = 0; i < numObjects; i++) {
    simArray[i].ptr->outputGcollect(lowestTime, outputQptr[i]);
  }

  for (i = 0; i < numObjects; i++) {
    simArray[i].ptr->inputGcollect(lowestTime, inputQptr[i]);
  }

  for (i = 0; i < numObjects; i++) {
    simArray[i].ptr->stateClear();
  }


  // The commManager needs to know that he can gcollect as well...
  commHandle->gcollect(lowestTime);
  delete [] inputQptr;
  delete [] outputQptr;
  return(lowestTime);
}

void
MatternGVTManager::sendEvent(MatternGVTMsg  *msg){
  msg->senderLP = id;
  commHandle->recvMsg(msg);
}

#ifdef EXTERNALAGENT
void 
MatternGVTManager::sendEvent(ExtAgentMsg* msg){
  msg->senderLP = id;
  commHandle->recvMsg(msg);
}
#endif
void
MatternGVTManager::sendEvent(CheckIdleMsg  *msg){
  commHandle->recvMsg(msg);
}

void
MatternGVTManager::sendEvent(newGVTMsg  *msg){
  commHandle->recvMsg(msg);
}

void
MatternGVTManager::sendEvent( BasicEvent* event){
  cout << "called sendEvent(BasicEvent* ) in MatternGVTManager! : " 
       <<  *event << endl;
}  

void
MatternGVTManager::sendEvent( TerminateMsg* terminate){
  commHandle->recvMsg( terminate );
}

BasicEvent*
MatternGVTManager::getEvent(){
  cout << "called getEvent in MatternGVTManager!" << endl;
  return (BasicEvent*)NULL;
}  

void
MatternGVTManager::simulate(){
  cout << "called simulate in MatternGVTManager!" << endl;
}  

VTime
MatternGVTManager::calculateMin(){
  cerr << "called calculateMin() in MatternGVTManager!" << endl;
  return ZERO;
}

void
MatternGVTManager::recvEvent(BasicEvent *event){
  cerr << "called recvEvent(BasicEvent  *) in MatternGVTManager! :"
       << *event << endl;
}

void
MatternGVTManager::executeProcess(){
  cerr << "called executeProcess() in MatternGVTManager!" << endl;
}

void
MatternGVTManager::recvMsg( newGVTMsg *msg ){
  static int noOfGVTRecv = 0;
  setGVT(msg->newGVT);
  oldgVT = msg->newGVT;

  //  if (id == 0) { 
  //    cout << "setting gVT to " << gVT << " at cycle " 
  //	 << ((TimeWarp*) simArray[0].ptr)->lpHandle->cycleCount << "\n";
  //  }

  for(int i=0;i < numObjects;i++){
    simArray[i].ptr->color = WHITE; //change to WHITE
  }
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
  if (noOfGVTRecv > noOfGVTToSkip && gVT > ZERO){
    noOfGVTRecv = 0;
    //DEBUGSTREAM <<"LP "<<id<< " gcollecting for " << msg->newGVT << "\n";
    gcollect();
  }
}
#ifdef EXTERNALAGENT
void
MatternGVTManager::recvMsg( ExtAgentMsg* activateMsg){
  // a token is flowing around the ring...  When received by an LP, it
  // waits for the process to execute 1000 events and then calls the
  // appropriate dynamic optimization routine to perform the required
  // optimization. The token is then forwarded to the next LP.
  if(id == 0){ // initiator has received the control message
    cout << "LP 0 has received the control message back !! " << endl;
  }
  else { 
    // someone other than the initiator has received the control message
    // three actions to perform here
    // [a] wait for 1000 events so that the changes to the system settle off.
    // [b] call your decision procedures
    // [c] pass on the token
    tokenHasBeenReceived = true; // token has been received
    delete activateMsg; // we can delete it 
    readyToPassItOn(); // passing it on to another LP
  }
}

void 
MatternGVTManager::readyToPassItOn(){
  if(numberOfEvents > MAX_EVENTS_TO_WAIT){
      numberOfEvents = 0;
      cout << "LP " << id << " calling decision procedures" << endl;
      ExtAgentMsg* newTokenMsg = new ExtAgentMsg;
      passItOn(newTokenMsg); // passing it on to another LP
  }
}
#endif
void
MatternGVTManager::recvMsg( MatternGVTMsg *gvtMsg ){
  // a token is flowing around the ring...  When received by an LP, it sets 
  // the process's color to red if it was white. It checks this LP's LGVT
  // and the minimum timestamp on this LP's queues and this control message
  // gets forwarded to another LP. If this message is received by the 
  // initiator of the control message(and count == 0), then the initiator 
  // sets the gVT as the control message contains the minimum lGVT.
  VTime localMin=PINFINITY, locallVT=PINFINITY;
  SequenceCounter localCount = 0;
  VTime localgVT = 0;
  static int gvtcount = 0;
  int i,j;

  if(id == 0 ){ // initiator has received the control message 
    for( i=0;i<numObjects;i++){
      localCount = localCount + simArray[i].ptr->whiteMessages;
      simArray[i].ptr->whiteMessages = 0;
    }
   
    // Check to see if the count is zero and this is the second round for
    // the token.
    if ( (simArray[0].ptr->color > RED) &&
	 ((localCount + gvtMsg->count) == 0) ) {
			
      localgVT = MIN_FUNC(gvtMsg->mClock, gvtMsg->mSend);
      //      DEBUGSTREAM << "localgVT is " << localgVT << endl;
      // sending the new gvt message
      if(localgVT < gVT){
	localgVT = gVT;
      }
      if(localgVT > ZERO)  {
	static int printGVT = 0;
	printGVT++;
	// send newGVTMsg, only if a new value has been found.
	// (The above comment is WRONG! we need to turn everyone white
	// again, so I commented out this check.)
	//	if(gVT != localgVT){
	newGVTMsg *gVTMsg = new newGVTMsg;
	gVTMsg->newGVT = localgVT;
	sendEvent( gVTMsg );
#ifdef LPDEBUG
	DEBUGSTREAM << "GVT is " << localgVT << "\n";
#endif
	printGVT = 0;
	delete gVTMsg;
	//	}
      }
      gVTTokenPending = false; // end of this gvt calculation cycle
    }
    else {
      // DEBUGSTREAM << "LP " << id << " Begin lVT " << locallVT << " Min " 
      //	     << localMin
      //	     << "\n";
      for( i=0;i<numObjects;i++){
	// get minimum tMin and total count for all objects
	simArray[i].ptr->color++;
	locallVT = MIN_FUNC(locallVT, simArray[i].ptr->getLVT());
	localMin = MIN_FUNC(localMin, simArray[i].ptr->tMin);
	localCount = localCount + simArray[i].ptr->whiteMessages;
	simArray[i].ptr->whiteMessages = 0;
	//     DEBUGSTREAM << "LP " << id << " lVT " << locallVT << " Min " 
	//	    << localMin << " color " << simArray[i].ptr->color 
	//	    << "\n";
      }
      //      DEBUGSTREAM << "LP " << id << " Done" << endl;
      gvtMsg->mClock = locallVT;
      gvtMsg->mSend = MIN_FUNC(gvtMsg->mSend,localMin);
      gvtMsg->count = localCount;
      //      DEBUGSTREAM << "LP " << id << " Passing MatternGVTMsg Token\n"
      //      		  << *gvtMsg << endl;
      passItOn(gvtMsg); // go for another round
    }
  }
  else { // control message received by another LP ( not LP 0)
    // okay, two things to do...[a] set color of all objects on this 
    //                              LP to red 
    //                          [b] pass on the token to processor(i mod n)+1
		
    // We may have already changed our color for this round, so we may 
    // want to skip the color change.
    if (colorAlreadyChanged == false) {
      for (j=0; j < numObjects; j++) {
	simArray[j].ptr->color++;
      }
    }
    else {
      colorAlreadyChanged = false;
    }
		
    if(simArray[0].ptr->color == RED){
      for( j=0;j<numObjects;j++){
	simArray[j].ptr->tMin = PINFINITY;
      }
      passItOn(gvtMsg);
    }
    else {
      for( i=0;i<numObjects;i++){
	localCount = localCount + simArray[i].ptr->whiteMessages;	
      }
      
      //   DEBUGSTREAM << "LP " << id << " Begin mClock " << gvtMsg->mClock 
      //               << " mSend " << gvtMsg->mSend << "\n";
      if (simArray[0].ptr->color > RED) {
	for( i=0;i<numObjects;i++){
	  gvtMsg->mClock = MIN_FUNC(gvtMsg->mClock, simArray[i].ptr->getLVT());
	  gvtMsg->mSend = MIN_FUNC(gvtMsg->mSend, simArray[i].ptr->tMin);
	  // DEBUGSTREAM << "process " << simArray[i].ptr->id << " mClock " 
	  //             << gvtMsg->mClock << " mSend "  << gvtMsg->mSend 
	  //		 << " color " << simArray[i].ptr->color << "\n";
	}
	//    DEBUGSTREAM << "LP " << id << " end" << endl;
      }
      gvtMsg->count = gvtMsg->count + localCount;
      passItOn(gvtMsg);
      for( i=0;i<numObjects;i++){
	simArray[i].ptr->whiteMessages = 0;
      }
    }
  }
	
	
#ifdef LPDEBUG
  DEBUGSTREAM << " Passing MatternGVTMsg Token\n"
	      << *gvtMsg << endl;
#endif
} // received a MatternGVTMsg token

void
MatternGVTManager::recvMsg( CheckIdleMsg *idleMsg ){
  // a token is flowing around the ring...  If it has come around twice,
  // and we're process 0, then we can safely terminate the simulation.
  // If we're not process zero, we check if we're idle or not, and if
  // so, forward the token...  Otherwise, we send an lGVT message back to
  // the head honcho with our idle status in it...

// a flag ranBetweenTokens is now added to MatternGVTManager class.
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
	DEBUGSTREAM << "Passing Token 1st round\n";
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
	  DEBUGSTREAM << "Cancelling Token 2nd round\n";
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
	  if(id!=0){
	    passItOn(idleMsg);
#ifdef LPDEBUG
	    DEBUGSTREAM << "Passing Token 2nd round\n";
#endif
	  }
	  else {

#ifdef LPDEBUG
	    DEBUGSTREAM << "Sending out the PINFINITY message\n";
#endif 
	    newGVTMsg *gVTMsg = new newGVTMsg;
	    gVTMsg->newGVT = PINFINITY;
	    sendEvent( gVTMsg );
	    delete gVTMsg;
	    //#endif
	  }
	}  // we got a token the second time and passed it on
      } // the token is on the second loop
    } // if we're idle
    else {
#ifdef LPDEBUG
      DEBUGSTREAM << "Cancelling token (not idle)\n"
		  << "InputQ size = " <<  simArray[0].ptr->inputQ.size()
		  << endl;
#endif
      // we're not idle!
      cancelToken(idleMsg);
    }  // we're not idle
  } // we're not zero or this wasn't a cancel messgae
} // receive a check idle token

void
MatternGVTManager::passItOn( MatternGVTMsg *gvtMsg ){
  if( id != numLPs - 1){
    gvtMsg->destLP++;
    sendEvent(gvtMsg);
  } // if we not the last guy in the loop
  else {
    // send it back to the beginning of the loop...
    gvtMsg->destLP = 0;
    sendEvent(gvtMsg);
  } // if we're the last guy in the loop
#ifdef LPDEBUG
  *lpFile << "passItOn : passed it on " << endl;
#endif
} 
#ifdef EXTERNALAGENT
void
MatternGVTManager::passItOn( ExtAgentMsg *activateMsg ){
  if( id != numLPs - 1){
    activateMsg->destLP++;
    sendEvent(activateMsg);
  } // if we not the last guy in the loop
  else {
    // send it back to the beginning of the loop...
    activateMsg->destLP = 0;
    sendEvent(activateMsg);
  } // if we're the last guy in the loop
#ifdef LPDEBUG
  *lpFile << "passItOn : passed it on " << endl;
#endif
} 
#endif
void
MatternGVTManager::passItOn( CheckIdleMsg *idleMsg ){
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
MatternGVTManager::cancelToken( CheckIdleMsg *idleMsg){
  // we got a checkidle token and we weren't idle... not good...
  idleMsg->destLP = 0;
  idleMsg->cancel = true;
#ifdef LPDEBUG
  *lpFile << "CancelToken : idleMsg sent " << endl;
#endif
  sendEvent(idleMsg);
}

void
MatternGVTManager::setFile( ofstream *file ){
  lpFile = file;
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
MatternGVTManager::setRanBetweenTokens(bool flag) {
  ranBetweenTokens = flag ;
}

bool
MatternGVTManager::giveRanBetweenTokens() {
  return ranBetweenTokens ;
}


BasicState*
MatternGVTManager::allocateState() {
  cerr << "MatternGVTManager::allocateState() called.\n";
  abort();

  return NULL;
}

#endif  // MATTERNGVTMANAGER
