#ifndef TRANSCEIVER_HH
#define TRANSCEIVER_HH

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
#include "config.hh"
#include "bool.hh"
#include "DefaultVTime.hh"
#include "KernelMsgs.hh"

#define INTERNAL_BUFFER_SIZE 50000
#define MAXIMUM_AGE 10

class MessageManager;
class CommPhyInterface;

enum AggregationMode { AGGREGATING_MODE, AGGRESSIVE_SEND, AGGREGATION_ERROR };

struct Transceiver {
public:
  struct InputMessageInfo {
    VTime receivedMessageSendTime;
    VTime myLVT;
  };
  struct OutputMessageInfo {
    VTime leastReceiveTime;
  };

private:
#ifdef ADAPTIVE_AGGREGATION
  VTime leastReceiveTime;
  VTime maxReceiveTime;
#endif

  int lpId;
  char* currentPosition;
  MessageManager* messageManagerHandle;
  //communication interface
  CommPhyInterface *commPhyLib;

#ifdef PROBE_SEND_RECEIVE_MESSAGES
public:
  InputMessageInfo inputReceiveInfo;
  OutputMessageInfo outputSendInfo;
  AggregationMode channelState;

  VTime avgTotalPeriod;
  VTime avgUsefulPeriod;
  VTime leastRollbackTime;
  VTime maxReceiveTime;
  VTime totalPeriod;
  VTime usefulPeriod;
  VTime uselessPeriod;
  VTime cycleStartTime;
  int numberOfPeriods;

#endif
public:

  int numberOfMsgs;
  char buffer[INTERNAL_BUFFER_SIZE];

  Transceiver();
  ~Transceiver();

  void setLpId(int id);
  void setMessageManagerHandle(MessageManager* msgMgrPtr);
  int readMsgSize();
  char* readMessage(int msgSize, char* inputBuf);
  void writeMsgSize(int msgSize);
  void writeMessage(int msgSize, char* message);
  void sendMessage();
  void receiveMessage();
  bool canWriteMessage(int size);
  bool canReceiveMessage(int);
  void probeToSend();
  void probeToReceive();
  void setCommPhyInterface(CommPhyInterface *phyLib);

#ifdef ADAPTIVE_AGGREGATION
  inline bool hasMessagesInFuture(VTime nextRecvTime) {
    if((nextRecvTime <= leastReceiveTime) && (numberOfMsgs > 0)) {
      return true;
    }
    else {
      return false;
    }
  }

  inline VTime getLeastReceiveTime() {
    return leastReceiveTime;
  }

  inline VTime getMaxReceiveTime() {
    return maxReceiveTime;
  }

  inline void setLeastReceiveTime(VTime recvTime) {
    leastReceiveTime = recvTime;
  }

  inline void setMaxReceiveTime(VTime recvTime) {
    maxReceiveTime = recvTime;
  }
  inline void setReceiveTimes(BasicMsg* msg) {
    if(msg->type == EVENTMSG) {
      EventMsg* eventMsg = (EventMsg*)msg;
      if(eventMsg->recvTime < leastReceiveTime) {
	leastReceiveTime = eventMsg->recvTime;
      }
      if(eventMsg->recvTime >= maxReceiveTime) {
	maxReceiveTime = eventMsg->recvTime;
      }
    }
  }
#endif
#ifdef PROBE_SEND_RECEIVE_MESSAGES
  inline void calculateAggregationStatistics() {
    if(leastRollbackTime == PINFINITY) {
      return;
    }
    numberOfPeriods++;
    channelState = AGGRESSIVE_SEND;
    if(cycleStartTime < leastRollbackTime) {
      usefulPeriod += (leastRollbackTime - cycleStartTime);
      avgUsefulPeriod = usefulPeriod/numberOfPeriods;
      totalPeriod += maxReceiveTime - cycleStartTime;
      avgTotalPeriod = totalPeriod/numberOfPeriods;
    }
    cycleStartTime = leastRollbackTime;
  }

  inline void setReceiveTimes(BasicMsg* msg) {
    if(msg->type == EVENTMSG) {
      EventMsg* eventMsg = (EventMsg*)msg;
      if(eventMsg->recvTime < outputSendInfo.leastReceiveTime) {
	outputSendInfo.leastReceiveTime = eventMsg->recvTime;
	if(eventMsg->sign == NEGATIVE) {
	  if(eventMsg->recvTime < leastRollbackTime) {
	    leastRollbackTime = eventMsg->recvTime;
	  }
	}
      }
      if(eventMsg->recvTime > maxReceiveTime) {
	maxReceiveTime = eventMsg->recvTime;
      }
    }
  }
  bool canSend(VTime lVT);
#endif
};

#endif //INTERNAL_BUFFER_HH
