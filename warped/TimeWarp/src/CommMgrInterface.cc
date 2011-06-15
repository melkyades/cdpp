//-*-c++-*-
#ifndef COMM_MGR_INTERFACE_CC
#define COMM_MGR_INTERFACE_CC
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
// $Id: CommMgrInterface.cc,v 1.12 1999/10/24 23:25:49 ramanan Exp $
//
//---------------------------------------------------------------------------
#include "CommMgrInterface.hh"
#include "LogicalProcess.hh"
#include "TimeWarp.hh"
#include "CommPhyInterface.hh"

#ifdef CLUMPS_ICM
#ifdef CLUMPS_ICM_IRCM
#include "CommPhyClumpsIRCM.hh"
#elif defined(CLUMPS_ICM_ISCM)
#include "CommPhyClumpsISCM.hh"
#elif defined(CLUMPS_ICM_AGGR_ISCM)
#include "CommPhyClumpsAggrISCM.hh"
#else
#include "CommPhyClumpsCCM.hh"
#endif
#endif

#ifdef CLUMPS_DPC
#ifdef CLUMPS_DPC_REG_RETR
#include "CommPhyClumpsDPC.hh"
#elif defined(CLUMPS_DPC_INFREQ_RETR)
#include "CommPhyClumpsDPC.hh"
#endif
#endif


#if !defined(CLUMPS_ICM) && !defined(CLUMPS_DPC)
#ifdef MPI
#include "CommPhyMPI.hh"
#elif TCPLIB
#include "CommPhyTCPMPL.hh"
#else
#include "CommPhyDefault.hh"
#endif
#endif 

#ifdef FWNS
#include "CommPhyForFWNS.hh"
#endif

CommPhyInterface *commLib = NULL;

void 
CommMgrInterface::executeProcess(){
  cerr << "Error!  Called execute process in comm manager!" << endl;
}

CommMgrInterface::CommMgrInterface(int numberOfLPs, CommPhyInterface *phyLib): BasicTimeWarp(){
  numLPs = numberOfLPs;
  commPhyLib = phyLib;
}


CommMgrInterface::~CommMgrInterface(){

  commPhyLib->physicalFinalize();
  delete commPhyLib;
  delete [] sendSequenceArray;
  delete [] recvSequenceArray;
}

#ifdef STATS

void 
CommMgrInterface::incrementReceivedApplicationMessageCount(){
  lpHandle->lpStats.incrementReceivedApplicationMessageCount();
}

void
CommMgrInterface::incrementReceivedPhysicalMessageCount(){
  lpHandle->lpStats.incrementReceivedPhysicalMessageCount();
}

void
CommMgrInterface::incrementSentPhysicalMessageCount(){
  lpHandle->lpStats.incrementSentPhysicalMessageCount();
}

#endif

void
CommMgrInterface::waitForInit( int numExpected ) {
  // let's wait for some init messages...
  int numReceived = 0;
  while(numReceived != numExpected ) {
    BasicMsg * msg = recvMPIMsg();
    if( msg != NULL && msg->type == INITMSG) {
      numReceived++;
      routeMsg((BasicMsg *)msg);
    }
    else if ( msg != NULL ) {
      // we didn't find an init msg, but we need to check for other types
      // of messages
      routeMsg((BasicMsg *)msg);
    } // first proibe unsuccessful
  } // while numRecived != numExpected...
} // CommMgrInterface::waitForInit()

void
CommMgrInterface::waitForStart(){

  BasicMsg *msg;
 
  bool finished = false;
  while(finished==false){
    msg = recvMPIMsg();
    if(msg!=NULL && msg->type == STARTMSG){
      finished = true;
      recvSequenceArray[ msg->senderLP ]++;
      delete [] ((char *)msg);
    } // if flag == 1
    else if(msg != NULL) {
      // we didn't find an init msg, but we need to check for other
      // msgs
      routeMsg((BasicMsg *)msg);
      delete [] ((char *)msg);
    } // first probe unsuccessful
  } // while finished == false
}

void
CommMgrInterface::initCommManager(ObjectRecord* sim, FOSSIL_MANAGER* gvt, 
				  int lpNum, LogicalProcess* lphandle){
  
  // This is to let the CommMgrInterface know the LP he is responsible for
  lpHandle = lphandle;
  
  if (lpNum > 1) {
    // don't init MPI if there's only 1 LP
    id = commPhyLib->physicalGetId();
  }
  else {
    id = 0;
  }
  
  simArray = sim;
  gVTHandle = gvt;
  numLPs = lpNum;
  
  sendSequenceArray = new SequenceCounter[numLPs];
  recvSequenceArray = new SequenceCounter[numLPs];

  for(int i = 0; i < numLPs; i++){
    sendSequenceArray[i] = 1;
    recvSequenceArray[i] = 0;
  }
}

#ifdef COMMDEBUG
void
CommMgrInterface::setFile( ofstream *file ){
  lpFile = file;
}
#endif

void
CommMgrInterface::recvEvent( BasicEvent* event){
  
  // Let's turn this sucker into a basic LP message
  EventMsg* mpiMsg;
  int msgSize = sizeof(BasicMsg) + event->size;

  mpiMsg = (EventMsg*)new char [msgSize];
  new (mpiMsg) EventMsg();
  
  // This is the old way - we know this doesn't completely work
  // *(BasicEvent*)mpiMsg = *event;
  memcpy((BasicEvent *)mpiMsg, event, event->size);
  mpiMsg->destLP = simArray[event->dest].lpNum;
  mpiMsg->senderLP = id;
  mpiMsg->type = EVENTMSG;

  // sequence stuff
  int dest = mpiMsg->destLP;
  mpiMsg->senderLP = id;
  mpiMsg->sequence = sendSequenceArray[dest];
  sendSequenceArray[dest]++;
  
  // someone gave us this message to send - therefore the GVT guy must look
  // at it...
#ifndef MATTERNGVTMANAGER
  gVTHandle->msgSendPeek( mpiMsg );
#endif

  if( mpiMsg->destLP == id ) {
    cerr << "CommMgrInterface " << id 
	 << " received an event for a local guy : " << *event << endl;
    // now we're essentially receiving it too, since we're delivering it
#ifndef MATTERNGVTMANAGER
    gVTHandle->msgRecvPeek( mpiMsg );
#endif
    deliver( mpiMsg );
    delete [] (char* ) mpiMsg;
  }
  else {
#ifdef CHECKASSERTIONS
    if ( (mpiMsg->sign != POSITIVE) && (mpiMsg->sign != NEGATIVE)) {
      *lpFile << "Sending (R) : " << mpiMsg << " - " << *mpiMsg << "\n";
      cout << "LP[" << id << "] failed assertion for sign" << endl;
      *lpFile << "LP[" << id << "] failed assertion for sign" << endl;
    }
    assert( (mpiMsg->sign == POSITIVE) || (mpiMsg->sign == NEGATIVE));
#endif
    remoteSend(mpiMsg, msgSize);
    delete [] (char* ) mpiMsg;
  }
}

void
CommMgrInterface::recvMsg( TerminateMsg *msg ) {
  // uh oh...  it's a terminate message... we'd better
  // generate messages for all the other guys
  int i;
  msg->senderLP = id;
  for(i=0; i<numLPs; i++){
    msg->sequence = sendSequenceArray[i];
    sendSequenceArray[i]++;
    
    msg->destLP = i;
    int size = sizeof( TerminateMsg );
    if(i != id ) {
      // Might as well skip ourselves
      remoteSend(msg, size);
    }
  }
  // Now we need to handle the termination... Let's call the method that
  // handles this
  terminateSimulation( msg );
}

void
CommMgrInterface::recvMsg( LGVTMsg *msg ){
  
  int dest = msg->destLP;
  msg->senderLP = id;
  msg->sequence = sendSequenceArray[dest];
  sendSequenceArray[dest]++;

  if(msg->destLP == id) {
    deliver(msg);
  }
  else {
    int size = sizeof( LGVTMsg);
    remoteSend(msg, size);
  }

}

void
CommMgrInterface::recvMsg( LowerLGVTMsg* msg ){
  
  int dest = msg->destLP;
  msg->senderLP = id;
  msg->sequence = sendSequenceArray[dest];
  sendSequenceArray[dest]++;

  if(msg->destLP == id) {
    deliver( msg );
  }
  else {
    int size = sizeof( LowerLGVTMsg );
    remoteSend( msg, size );
  }
}

void
CommMgrInterface::recvMsg( GVTAckMsg* msg ) {
  
  int dest = msg->destLP;
  msg->senderLP = id;
  msg->sequence = sendSequenceArray[dest];
  sendSequenceArray[dest]++;

  if( msg->destLP == id ) {
    deliver( msg );
  }
  else {
    // we need to send this message remotely..
    int size = sizeof( GVTAckMsg );
    remoteSend( msg, size );
  }
}

void
CommMgrInterface::recvMsg( LPAckMsg* msg ) {
  
  int dest = msg->destLP;
  msg->senderLP = id;
  msg->sequence = sendSequenceArray[dest];
  sendSequenceArray[dest]++;

  if( msg->destLP == id ) {
    deliver( msg );
  }
  else {
    // we need to send this message remotely..
    int size = sizeof( GVTAckMsg );
    remoteSend( msg, size );
  }
}

void
CommMgrInterface::recvMsg( DummyMsg *dummy ){
  // we need to send this...
  int size = sizeof(DummyMsg);
  dummy->senderLP = id;
  dummy->sequence = sendSequenceArray[dummy->destLP];
  sendSequenceArray[dummy->destLP]++;
  
  remoteSend((BasicMsg *)dummy,size);
}

void
CommMgrInterface::recvMsg( DummyRequestMsg *dummy ){
  // we need to send this...
  int size = sizeof(DummyMsg);
  dummy->senderLP = id;
  dummy->sequence = sendSequenceArray[dummy->destLP];
  sendSequenceArray[dummy->destLP]++;
  
  remoteSend((BasicMsg *)dummy,size);
}


void
CommMgrInterface::recvMsg( newGVTMsg* msg ){
  int i;
  int size = sizeof(newGVTMsg);
  // let's send out the new GVT info to all of the LPs
  for(i = 1; i < numLPs; i++) {
    
    msg->senderLP = id;
    msg->sequence = sendSequenceArray[i];
    msg->destLP = i; 
    sendSequenceArray[i]++;
    remoteSend(msg, size);
  }
  // now we'll send a copy to our OWN gVT guy
  deliver( msg );
}  


void
CommMgrInterface::recvMsg( InitMsg * msg ) {
  int size = sizeof(InitMsg);
  msg->senderLP = id;
  msg->sequence = sendSequenceArray[msg->destLP];
  sendSequenceArray[msg->destLP]++;
  
  remoteSend((BasicMsg *)msg,size);
}

void
CommMgrInterface::recvMsg( StartMsg * msg ) {
  int size = sizeof(StartMsg);
  msg->senderLP = id;
  msg->sequence = sendSequenceArray[msg->destLP];
  sendSequenceArray[msg->destLP]++;
  
  remoteSend((BasicMsg *)msg,size);
}

#ifdef MATTERNGVTMANAGER
void
CommMgrInterface::recvMsg( MatternGVTMsg *gvtMsg ){
  // we need to send this...
  int size = sizeof(MatternGVTMsg);
  gvtMsg->senderLP = id;
  gvtMsg->sequence = sendSequenceArray[gvtMsg->destLP];
  sendSequenceArray[gvtMsg->destLP]++;
  remoteSend((BasicMsg*)gvtMsg,size);
}
#endif

#ifdef EXTERNALAGENT
void 
CommMgrInterface::recvMsg( ExtAgentMsg *agentMsg ){
  // we need to send this...
  int size = sizeof(ExtAgentMsg);
  agentMsg->senderLP = id;
  agentMsg->sequence = sendSequenceArray[agentMsg->destLP];
  sendSequenceArray[agentMsg->destLP]++;
  remoteSend((BasicMsg*)agentMsg,size);
}
#endif

void
CommMgrInterface::recvMsg( CheckIdleMsg *idleMsg ){
  // we need to send this...
  int size = sizeof(CheckIdleMsg);
  idleMsg->senderLP = id;
  idleMsg->sequence = sendSequenceArray[idleMsg->destLP];
  sendSequenceArray[idleMsg->destLP]++;
  if(idleMsg->destLP != id){
    remoteSend((BasicMsg *)idleMsg,size);
  }
  else{
    deliver(idleMsg);
  }
}

void
CommMgrInterface::deliver( BasicEvent* event ){
  int receiver = event->dest;
  
  simArray[receiver].ptr->recvEvent(event);
}  

BasicEvent*
CommMgrInterface::getEvent(){
  cerr << "getEvent() called in CommMgrInterface " << id << endl;
  return NULL;
}

void
CommMgrInterface::routeMsg( BasicMsg* msg ) {

#ifdef STATS
  lpHandle->lpStats.recordReceivedKernelMessages(msg->type);
#endif

  switch(msg->type) {
  case INITMSG:
    {
      InitMsg * initMsg = (InitMsg *) msg;
      
      // we just got an init msg...  someone needs to update our simArray...
      // let's do it right here for now...
      // first let's make sure WE don't have that object!!
      
      int objId = initMsg->objId;
      if(simArray[objId].lpNum == id && initMsg->lpId != id ){
	// we've already registered this guy locally!
	cerr << "CommMgrInterface " << id << " received init msg : " << *initMsg <<
	  " and it has a record of object " << objId << " being local!" << endl;
      }
      else{
	simArray[objId].lpNum = initMsg->lpId;
				
	// point the pointer at the CommMgrInterface, so it takes care of the
	// communication
	if(simArray[objId].lpNum!=id){
	  simArray[objId].ptr = this;
	}
      } // the object isn't already local...
			
      // now we can delete it!
      delete [] ((char *)initMsg);
      break;
    }
	
  case STARTMSG:
    cerr << "CommMgrInterface " << id << " asked to route a start message!" << endl;
    break;
		
  case EVENTMSG:
    {
      EventMsg* eventMsg = (EventMsg*) msg;
			
      // first, we need to get the size of the BasicEvent contained in this
      // message, and allocate a new space for it...
      if(eventMsg->size <= 0){
	cerr << "Bogus message received from MPI" << endl;
	cerr << "eventMsg has size = " << eventMsg->size << endl;
	abort();
      }

      // this is need in the event that you receive an eventMsg before
      // the start message.
      //#ifndef MATTERNGVTMANAGER
      gVTHandle->msgRecvPeek( eventMsg );
      //#endif

      BasicEvent *toDeliver = (BasicEvent *)new char[eventMsg->size];  
      // now we'll copy the message into the new chunk...
      memcpy(toDeliver, (BasicEvent *)eventMsg, eventMsg->size);
			
      toDeliver->next = NULL;
      toDeliver->prev = NULL;
      toDeliver->nextObj = NULL;
      toDeliver->prevObj = NULL;

      deliver(toDeliver);
			
      break;
    }
	
  case TERMINATEMSG:
    {
      TerminateMsg *ouch = (TerminateMsg *) msg;
			
      terminateSimulation( ouch );
      // we are the creator and consumer of this message...
      break;
    }
	
#ifdef MATTERNGVTMANAGER
  case MATTERNGVTMSG:
    {
      MatternGVTMsg *checkGvt = (MatternGVTMsg *)msg;
      deliver(checkGvt);
      break;
    }
#endif
#ifdef EXTERNALAGENT
  case EXTAGENTMSG:
    {
      ExtAgentMsg *activateMsg = (ExtAgentMsg *)msg;
      deliver(activateMsg);
      break;
    }
#endif
  case CHECKIDLEMSG:
    {
      CheckIdleMsg *checkIdle = (CheckIdleMsg *)msg;
      deliver( checkIdle );
      break;
    }
	
  case NEWGVTMSG:
    deliver((newGVTMsg*) msg);
    break;
  case LGVTMSG: 
    // we should ONLY receive lGVTMsg's if we're the head GVT guy...
    // let's check, just for the fun of it...
    if(id!=0) {
      cerr << "CommMgrInterface " << id << " received lGVT message! " <<
	(LGVTMsg*)msg << endl;
      // we might want to kill the simulation at this point...
    }
    deliver((LGVTMsg*) msg);
    break;
		
  case LOWERLGVTMSG: 
    // we should ONLY receive lGVTMsg's if we're the head GVT guy...
    // let's check, just for the fun of it...
    if(id != 0) {
      cerr << "CommMgrInterface " << id << " received lGVT message! " << endl;
      // we might want to kill the simulation at this point...
    }
    deliver((LowerLGVTMsg*) msg);
    break;
		
  case GVTACKMSG:
    // the head GVT dude has acknowledged that we are lowering our
    // estimate of GVT.
    deliver((GVTAckMsg*) msg);
    break;
    
  case LPACKMSG:
    deliver((LPAckMsg*) msg);
    break;
		
  case DUMMYMSG:
    deliver((DummyMsg *) msg);
    break;
		
  case DUMMYREQUESTMSG:
    deliver((DummyRequestMsg *) msg);
    break;
		
  default:
    {
      TerminateMsg *ouch;
      cout << "CommMgrInterface " << id << ": unknown message (" << msg
	   << ") type "
	   << *msg << " received!" << endl;
      ouch = new TerminateMsg;
      strcpy(ouch->error, "unknown message type found...");
      recvMsg(ouch);
      break;
    }
  } // end of case on message type
}


int
CommMgrInterface::recvMPI(int maxNum){
  int numReceived = 0;
  bool noMessagesLeft = false;
  BasicMsg * msgReceived;
  //BasicTimeWarp *transferredObject;

  while( numReceived != maxNum &&  noMessagesLeft == false) {
    msgReceived = recvMPIMsg();
    if( msgReceived == NULL ) {
      noMessagesLeft = true;
    }
    else {
      numReceived++;
      // now let's make sure that we didn't get this message out of sequence
      if(msgReceived->sequence != recvSequenceArray[msgReceived->senderLP]+1){
	// we'll really need to handle this sometime soon...
	//	cerr << "CommMgrInterface " << id << 
	//	  " received a message out of sequence!:\n" << *msgReceived;
	//	cerr << "\nLast message received from this guy : " <<
	//	  recvSequenceArray[msgReceived->senderLP] << endl;
      }
      
      recvSequenceArray[msgReceived->senderLP]++;
      if( msgReceived->type == EVENTMSG ) {
	gVTHandle->msgRecvPeek( (EventMsg *)msgReceived );
      }
      routeMsg( msgReceived );
      // since we allocated it, we delete it
      delete [] ((char*) msgReceived);
    } // else
  } //while
  return numReceived;
}

void
CommMgrInterface::deliver( newGVTMsg *msg ){
  gVTHandle->recvMsg(msg);
}

#if !defined(MATTERNGVTMANAGER)
void
CommMgrInterface::deliver( LGVTMsg *msg ){
  gVTHandle->recvMsg(msg);
}
#else 
void
CommMgrInterface::deliver(LGVTMsg*) {}
#endif

#if !defined(MATTERNGVTMANAGER) 
void 
CommMgrInterface::deliver( LowerLGVTMsg *msg ){
  gVTHandle->recvMsg(msg);
}
#else
void 
CommMgrInterface::deliver( LowerLGVTMsg *){}
#endif

#if !defined(MATTERNGVTMANAGER) 

void
CommMgrInterface::deliver( GVTAckMsg *gVTMsg ) {
  gVTHandle->recvMsg(gVTMsg);
}
#else
void
CommMgrInterface::deliver( GVTAckMsg * ) {}
#endif

#if !defined(MATTERNGVTMANAGER) 

void
CommMgrInterface::deliver( LPAckMsg *lpAckMsg ) {
  gVTHandle->recvMsg(lpAckMsg);
}
#else
void
CommMgrInterface::deliver( LPAckMsg * ) {}
#endif

#if !defined(MATTERNGVTMANAGER) 

void
CommMgrInterface::deliver( DummyMsg *dummy ) {
  gVTHandle->recvMsg(dummy);
}
#else
void
CommMgrInterface::deliver( DummyMsg * ) {}
#endif

#if !defined(MATTERNGVTMANAGER) 

void
CommMgrInterface::deliver( DummyRequestMsg *dummy ) {
  gVTHandle->recvMsg(dummy);
}
#else
void
CommMgrInterface::deliver( DummyRequestMsg * ) {}
#endif

void
CommMgrInterface::deliver( CheckIdleMsg *idleMsg ) {
  gVTHandle->recvMsg(idleMsg);
}


#ifdef MATTERNGVTMANAGER
void
CommMgrInterface::deliver( MatternGVTMsg *gvtMsg ) {
  gVTHandle->recvMsg(gvtMsg);
}
#endif

#ifdef EXTERNALAGENT
void
CommMgrInterface::deliver( ExtAgentMsg *activateMsg ) {
  gVTHandle->recvMsg(activateMsg);
}
#endif

void
CommMgrInterface::simulate(){
  cerr << "called simulate in CommMgrInterface!" << endl;
}  


VTime
CommMgrInterface::calculateMin(){
  cerr << "called calculateMin() in CommMgrInterface!" << endl;
  // the GVT guy takes care of this for us
  return PINFINITY;
}


SequenceCounter
CommMgrInterface::getSequence( int lpId ) {
  return sendSequenceArray[ lpId ];
}

void
CommMgrInterface::terminateSimulation( TerminateMsg* ouch ) {
  cerr << "CommMgrInterface " << id << " received terminate message :\n"
       << *ouch << endl;

#ifdef COMMDEBUG
  cout << id << " closing file" << endl;
  lpFile->close();
#endif

  this->~CommMgrInterface();
  exit(-1);
}

void 
CommMgrInterface::sendEvent( BasicEvent* event){
  cerr << "sendEvent( BasicEvent* ) called in CommMgrInterface "
       << id << " : " << *event << endl;
}

void
CommMgrInterface::iHaveObject(int objId){
  InitMsg * msg = new InitMsg;
  msg->objId = objId;

  msg->lpId = id;
  msg->destLP = 0;
  
  // this is a little weird, but what the heck?
  recvMsg( msg );
  
  delete msg;
}

BasicState*
CommMgrInterface::allocateState() {
  cerr << "Error::CommMgrInterface::allocateState() called";
  abort();

  return NULL;
}

void
CommMgrInterface::gcollect(VTime) {
  //  outputQ.gcollect(gtime);
}


void
physicalCommInit(int *argc, char ***argv){
  commLib = new COMMPHYSICAL;
  commLib->physicalInit(argc, argv);
}

int
physicalCommGetId(){
  if ( commLib == NULL ){
    cerr << "Error physicalCommInit(int *argc, char ***argv) not called yet" << endl;
    abort();
  }
  return(commLib->physicalGetId());
}

#endif
