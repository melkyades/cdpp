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
//---------------------------------------------------------------------------
#include "Transceiver.hh"
#include "KernelMsgs.hh"
#include <iostream.h>
#include <fstream.h>

#include "MessageManager.hh"
#include "CommPhyInterface.hh"
#include "CommMgrMsgAgg.hh"

extern ofstream* gFileHandle;

//inline
Transceiver::Transceiver() {
  lpId = 0;
  currentPosition = buffer;
  numberOfMsgs = 0;
  messageManagerHandle = NULL;
#ifdef ADAPTIVE_AGGREGATION
  leastReceiveTime = PINFINITY;
  maxReceiveTime = ZERO;
#endif

#ifdef PROBE_SEND_RECEIVE_MESSAGES
  inputReceiveInfo.receivedMessageSendTime = ZERO;
  inputReceiveInfo.myLVT = ZERO;
  outputSendInfo.leastReceiveTime = ZERO;
  channelState = AGGRESSIVE_SEND;
  leastRollbackTime = PINFINITY;
  maxReceiveTime = ZERO;
  totalPeriod = ZERO;
  usefulPeriod = ZERO;
  uselessPeriod = ZERO;
  cycleStartTime = ZERO;
  numberOfPeriods = 0;
  avgTotalPeriod = ZERO;
  avgUsefulPeriod = ZERO;
#endif

}


//inline
Transceiver::~Transceiver() {
}


//inline 
void
Transceiver::setLpId(int id) {
  lpId = id;
}

void
Transceiver::setMessageManagerHandle(MessageManager* msgMgrPtr) {
  messageManagerHandle = msgMgrPtr;
}

//inline 
int
Transceiver::readMsgSize() {

  int* ptr;
  ptr = (int*)currentPosition;
  //The following operation is REALLY COMPILER, ARCHITECTURE dependent
  int val = *ptr;
  ptr++;
  currentPosition = (char*) ptr;
  return val;
}

//inline 
char*
Transceiver::readMessage(int msgSize, char* inputBuf) {

  memcpy(inputBuf, currentPosition, msgSize);
  currentPosition = currentPosition + msgSize;
  return inputBuf;
}


//inline 
void
Transceiver::writeMsgSize(int msgSize) {

  int* ptr;
  ptr = (int*)currentPosition;
  //The following operation is REALLY COMPILER, ARCHITECTURE dependent
  *ptr = msgSize;
  ptr++;
  currentPosition = (char*) ptr;
}

//inline 
void
Transceiver::writeMessage(int msgSize, char* message) {

  memcpy(currentPosition, message, msgSize);
  currentPosition = currentPosition + msgSize;
}


//inline 
void
Transceiver::sendMessage() {

#ifdef LPDEBUG
  *gFileHandle << "Sending Message: " << numberOfMsgs << " messages" << " :lp id is " << lpId << endl;
#endif

#if defined(MESSAGE_AGGREGATION) && defined(STATS)
  if(numberOfMsgs < 1000){
    CommMgrMsgAgg* tempPtr = (CommMgrMsgAgg *)
      (messageManagerHandle->getCommManagerHandle());
    tempPtr->incrementStatsArray(numberOfMsgs); 
  }
  else {
    cout << "Number of messages is " << numberOfMsgs << endl;
  }
#endif

#ifdef MESSAGE_AGGREGATION
  commPhyLib->physicalSend((char*)&(this->numberOfMsgs), currentPosition - ((char *)&(this->numberOfMsgs)), lpId);
#endif
  numberOfMsgs = 0;
  currentPosition = buffer;
#ifdef PROBE_SEND_RECEIVE_MESSAGES
  outputSendInfo.leastReceiveTime = PINFINITY;
  maxReceiveTime = ZERO;
  leastRollbackTime = PINFINITY;
#endif
#ifdef STATS
  //#ifdef MESSAGE_AGGREGATION
  CommMgrInterface* tempPtr = (CommMgrInterface *)(messageManagerHandle->getCommManagerHandle());
  tempPtr->incrementSentPhysicalMessageCount();
  //#endif
#endif
}

#ifdef PROBE_SEND_RECEIVE_MESSAGES
bool Transceiver::canSend(VTime lVT) {
  if(numberOfMsgs <= 0) {
    return false;
  }
  if(outputSendInfo.leastReceiveTime <= inputReceiveInfo.receivedMessageSendTime) {
    inputReceiveInfo.receivedMessageSendTime = outputSendInfo.leastReceiveTime;
    return true;
  }
  else {
    if(channelState == AGGREGATING_MODE) {
      if((lVT - outputSendInfo.leastReceiveTime) > 2* avgTotalPeriod) {
	return true;
      }
      else {
	return false;
      }
    }
    else {
      VTime difference = maxReceiveTime - cycleStartTime;
      if(difference < avgUsefulPeriod) {
	return true;
      }
      else {
	channelState = AGGREGATING_MODE;
	return false;
      }
    }
    return false;
  }
}

#endif
//inline 
void
Transceiver::receiveMessage() {
  bool status = false;

  numberOfMsgs = 0;
  currentPosition = buffer;

  if (commPhyLib->physicalProbeRecvBuffer((char*)&(this->numberOfMsgs), INTERNAL_BUFFER_SIZE+sizeof(int), status)){
  if ( status == false ){
    cerr << "Transceiver::receiveMessage message size greater than INTERNAL_BUFFER_SIZE+sizeof(int):" << INTERNAL_BUFFER_SIZE+sizeof(int) << endl;
  }
#ifdef LPDEBUG
    *gFileHandle << "Receiving Message: " << numberOfMsgs << " messages" << " :lp id is " << lpId <<  endl;
#endif
    //#if defined(MESSAGE_AGGREGATION) && defined(STATS)
#ifdef STATS
  CommMgrInterface* tempPtr = (CommMgrInterface *)(messageManagerHandle->getCommManagerHandle());
  tempPtr->incrementReceivedPhysicalMessageCount();
#endif
  }
}

//inline 
bool
Transceiver::canWriteMessage(int size) {

  if((currentPosition + size) < ((char*)buffer + INTERNAL_BUFFER_SIZE)) {
    return true;
  }
  else {
    return false;
  }
}

//inline 
bool
Transceiver::canReceiveMessage(int) {
  if(numberOfMsgs == 0) {
    return true;
  }
  else {
    return false;
  }
}


//inline 
void
Transceiver::probeToSend() {
  if(numberOfMsgs > 0) {
    sendMessage();
  }
}

//inline 
void
Transceiver::probeToReceive() {
  if(numberOfMsgs == 0) {
    receiveMessage();
  }
}

void
Transceiver::setCommPhyInterface(CommPhyInterface *phyLib){
  commPhyLib = phyLib;
}
