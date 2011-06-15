//-*-c++-*-
#ifndef COMM_MGR_MSG_AGG_CC
#define COMM_MGR_MSG_AGG_CC
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
// $Id: CommMgrMsgAgg.cc,v 1.7 1999/10/24 23:25:50 ramanan Exp $
//
//---------------------------------------------------------------------------
#include "CommMgrMsgAgg.hh"
#include "LogicalProcess.hh"
#include "LogicalProcessStats.hh"

CommMgrMsgAgg::CommMgrMsgAgg(int numberOfLPs, CommPhyInterface *phyLib) : CommMgrInterface(numberOfLPs, phyLib), msgMgr(numberOfLPs, this, phyLib){
}

CommMgrMsgAgg::~CommMgrMsgAgg(){
}


void
CommMgrMsgAgg::initCommManager(ObjectRecord* sim,      // pointer to sim array
			       FOSSIL_MANAGER* gvt,    // pointer to GVTManager
			       int lpNum,              // number of LPs
			       LogicalProcess* lphandle // handle to the LP 
			       ){ 

  CommMgrInterface::initCommManager(sim, gvt, lpNum, lphandle);
  msgMgr.initMessageManager(getID());
}

BasicMsg *
CommMgrMsgAgg::recvMPIMsg(){

#ifndef ADAPTIVE_AGGREGATION
  incrementAgeOfMessage();
#endif

  return msgMgr.receiveMessage();
}


void
CommMgrMsgAgg::remoteSend(BasicMsg *msg, int size){

#ifdef COMMDEBUG
  *lpFile << "sending : ";
  printCasted(*lpFile, msg);
#endif
  
#ifdef STATS
  lpHandle->lpStats.incrementSentApplicationMessageCount();
#endif

  msgMgr.sendMessage(msg, size);
 
}

int
CommMgrMsgAgg::recvMPI(int maxNum){
  int numReceived = 0;
  bool noMessagesLeft = false;
  BasicMsg *msgReceived;

  while ( numReceived != maxNum && noMessagesLeft == false ){
    msgReceived = recvMPIMsg();
    if ( msgReceived == NULL ){
      noMessagesLeft = true;
#ifdef FIXED_MSG_COUNT
      waitedWithNoInputMessages++;
#endif
    } else {
#ifdef FIXED_MSG_COUNT
      waitedWithNoInputMessages = 0;
#endif
      numReceived++;
      if ( msgReceived->sequence != recvSequenceArray[msgReceived->senderLP]+1){
	// we have to handle this situation sometime soon.
      }
      recvSequenceArray[msgReceived->senderLP]++;
      if ( msgReceived->type == EVENTMSG ){
	gVTHandle->msgRecvPeek((EventMsg *)msgReceived);
      }
      routeMsg(msgReceived);
      delete [] ((char*)msgReceived);
    }
  }
  return numReceived;
}

#ifdef STATS
#ifdef MESSAGE_AGGREGATION
inline void 
CommMgrMsgAgg::incrementStatsArray(int numberOfMsgs) {
   lpHandle->lpStats.statsArray[numberOfMsgs]++;    
}	
#endif
#endif

void
getMessageManagerParameters(int argc, char **argv){
    //A better Arg parser should be written

  char* buf[10];
  buf[0] = NULL;
  //Age
  buf[1] = "10";
  //Receive Delay
  buf[2] = "20";
  //Aggregate Control message
  buf[3] =  "1";
  //Flush when you get anti messages
  buf[4] = "1";

  for(int i=1; i< argc; i++) {
    buf[i] = argv[i];
  }

  gAggregateCtrlMsg = 1;
  gFlushFlag = 1;

  if(argc > 1) {
    gMaximumAge = atoi(buf[1]);
    gMaxReceiveDelay = atoi(buf[2]);
  }
  else {
    gMaxReceiveDelay = 10;
    gMaximumAge = 20;
  }
}

#endif //#ifndef COMM_MGR_MSG_AGG_CC
