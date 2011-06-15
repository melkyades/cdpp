#ifndef MESSAGE_MANAGER_HH
#define MESSAGE_MANAGER_HH

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
#include "Transceiver.hh"
#include "KernelMsgs.hh"
#include "BasicTimeWarp.hh"
#include "DefaultVTime.hh"
#include "LogicalProcessStats.hh"

class CommPhyInterface;
enum AggregationReceiveCriteria { RECEIVE, NO_NEED_TO_RECEIVE};

enum AggregationSendCriteria { SEND, WRITE, SEND_AND_WRITE, WRITE_AND_SEND, DO_NOT_SEND, SEND_WRITE_SEND, DO_NOT_KNOW};

class ofstream;
extern ofstream* gFileHandle;
extern double gTanTheta;
extern int gMaxReceiveDelay;
extern int gMaximumAge;
extern int gAggregateCtrlMsg;
extern int gFlushFlag;

class MessageManager {

private:

  const int numberOfLps;
  int lpId;
  BasicTimeWarp* communicationManager;
  Transceiver* receive_buffer;
  Transceiver* send_buffer;

  //The following are the variables need for deciding on the 
  //sending the aggregate

  //This records the number of sent messages to all the LP's
  int numberOfMessagesToBeSent;

  //This records the number of messages sent previously
  int numberOfMessagesSentPreviously;

  //This records the actual age of the messages in the aggregate
  int ageOfMessage;
  //This is used to increment the age of the message, by the factor 
  // that is desired. If there are no messages in the aggregate queue
  // then the ageIncrementor is zero. If there have been messages 
  //in the aggregate queue waiting to be sent, then the ageIncrementor 
  //is set to 1. The ageIncrementor can also be set to value other than
  //1, but needs a study to decide on this value
  int ageIncrementor;

  //This records the number of times we have entered the receive module
  int receiveCriterion;

  //The slope of the line, that we are using as threshold
  double tanTheta;

  //The maximum desired delay between two receive operations
  int maxReceiveDelay;

  //The maximum Allowable age, a message is allowed to wait
  int maximumAge;

  //The maximum Allowable age, a message is allowed to wait next time
  int newMaximumAge;

  //The maximum Allowable age, a message is waited previously
  int previousMaximumAge;

  //The boolean flags, that used  to determine if the curve characterizing
  //# of messages w.r.t. time is going up or down
  bool upCurve; 
  bool downCurve;

  double prevRate;

  //This boolean flag tells us if we should aggregate the control message
  int aggregateCtrlMsg;

  //This variable is set if the LP is doing the rollback operation
  bool rollingBack;

  //This is used to indicate if the message is not getting any 
  //input messages and this may be due to the fact that it is
  //holding some messages
  int waitingForMessage;

  //This function returns true, if the message that is passed to it 
  //is a control message otherwise false
  bool isControlMessage(BasicMsg* msg);

  //This function determines if the inputQ is empty
  bool isEmptyInputQ();
  
  //This function checks for following two things:
  //(1) If there is enough space in the buffer to write the message
  //(2) If this message is a high priority message that should not be
  //held in the aggregation buffer. The following messages are HIHG
  //priority messages.
  //(a) INITMSG, (b) STARTMSG, (3) TERMINATEMSG, (4)EVENTMSG with -ve sign
  //If the first and second condition returns TRUE then SEND_AND_SEND
  //is returned.
  //If first alone is true then SEND_AND_WRITE is returned
  //If second alone is true the WRITE_AND_SEND is returned
  //If both are false then WRITE is returned
  inline AggregationSendCriteria getSendCriteria(BasicMsg* msg, int size);
public:
  VTime lVT;
public:

  MessageManager(int numLPs, BasicTimeWarp* commManager, CommPhyInterface *phyLib);
  ~MessageManager();

  void initMessageManager(int lpId);

  void probeToSend();
  void probeToReceive();

  void writeMessage(char* message, int size, int lpId);

  char* readMessage();
  BasicMsg* receiveMessage();

  //This function sends the message to the aggregated buffer
  // The aggregated message is sent if the aggregation criterion is met
  void sendMessage(BasicMsg* msg, int size);
  
  //This functions sends the aggregated messages to be sent to different LPs.
  void sendMessage();

  inline int getLpId() { return lpId;}

  inline void flushIfAgeExceeded() {
    if(ageOfMessage > maximumAge) {
#ifdef STATS
#ifdef MESSAGE_AGGREGATION
      logicalProcessStats::increment_window_size_exhausted();
#endif
#endif
      sendMessage();
    }
  }

  //This function returns the criteria namely:
  // SEND_AND_WRITE, happens, if the current Message can't be held in the 
  //aggregation buffer.
  // WRITE_AND_SEND, buffer is not full, but send criterion has been met
  // WRITE, buffer is not full and sending criterion is not met
  // SEND, messages in this have delayed for a long time

  //These two functions are used in getting the criterion that decides 
  //based fixed slope curve.
  AggregationSendCriteria getSendCriteriaInFixedSlope(BasicMsg* msg, int size);
  AggregationSendCriteria getSendCriteriaInFixedSlope();

  //These two functions are used in getting the criterion that decides 
  //based on fixed pessimism, namely the age.
  AggregationSendCriteria getSendCriteriaInFixedPessimism(BasicMsg* msg, int size);
  AggregationSendCriteria getSendCriteriaInFixedPessimism();

  AggregationSendCriteria getSendCriteriaInFixedMsgCount(BasicMsg* msg, int size);
  AggregationSendCriteria getSendCriteriaInFixedMsgCount();

  //These two functions are used in getting the criterion that decides 
  //based fixed slope with error.
  AggregationSendCriteria getSendCriteriaInFixedSlopeWithError(BasicMsg* msg, int size);
  AggregationSendCriteria getSendCriteriaInFixedSlopeWithError();

  //These two functions are used in getting the criterion that decides 
  //based mean of previous aggregation factors.
  AggregationSendCriteria getSendCriteriaInMeanOfFactors(BasicMsg* msg, int size);
  AggregationSendCriteria getSendCriteriaInMeanOfFactors();

  //This function checks if the receiving messages from the network
  //has to be done.
  AggregationReceiveCriteria getReceiveCriteria();

  //This function checks to see, if the send criterion has been reached for
  // messages to be sent, and send them
  void checkToSend();

#if defined (ADAPTIVE_AGGREGATION) || defined (PROBE_SEND_RECEIVE_MESSAGES)
  void checkToSend(VTime nextRecvTime);
#endif
#ifdef PROBE_SEND_RECEIVE_MESSAGES
  inline void updateInputReceiveInfo(BasicMsg* msg) {
    EventMsg* eventMsg = NULL;
    if(msg != NULL) {
      switch(msg->type) {
      case EVENTMSG:
	eventMsg = (EventMsg*)msg;
	send_buffer[eventMsg->senderLP].inputReceiveInfo.receivedMessageSendTime = eventMsg->sendTime;
	send_buffer[eventMsg->senderLP].inputReceiveInfo.myLVT = lVT;
	break;
      };
    }
  }
  inline void calculateAggregationStatistics() {
    int i=0;
    for(i=0; i<numberOfLps; i++) {
      if((i != lpId) && (send_buffer[i].numberOfMsgs > 0)) {
	send_buffer[i].calculateAggregationStatistics();
      }
    }    
  }
#endif
  //This function checks to see, if the receive criterion has been reached 
  // to receive messages  and receive them
  void checkToReceive();

  //This function resets all the information that decides the send criterion
  void  resetSendCriterionInfo();  

  //This function resets all the information that decides the receive criterion
  void  resetReceiveCriterionInfo();  

  void incrementNumberOfMessagesToBeSent();

  inline void setAgeIncrementor(int value = 0) {
    ageIncrementor = value;
  }
  void incrementAgeOfMessage();
  void incrementReceiveCriterion();

  //This functions sets the rollingBack flag to true
  //  void setRollBackFlag();
  inline  void MessageManager::setRollBackFlag() {
    rollingBack = true;
  }
  //This functions sets the rollingBack flag to false
  //  void resetRollBackFlag();
  inline  void MessageManager::resetRollBackFlag() {
    rollingBack = false;
  }

  inline int hasMessagesToSend() {
    int i=0;
    int flag = 0;
    for(i=0; i<numberOfLps; i++) {
      if((i != lpId) && (send_buffer[i].numberOfMsgs > 0)) {
	flag = 1;
	break;
      }
    }
    return flag;
  }

  //This function is used to flush the aggregation buffer if we
  //Had been waiting to collect a fixed number of messages, but 
  //no messages have been collected during the wait time.
  inline void flushIfWaitedTooLong(int& waitTime) {
    if(waitTime > maximumAge) {
#ifdef STATS
#ifdef MESSAGE_AGGREGATION
      int i=0;
      int flag = 0;
      for(i=0; i<numberOfLps; i++) {
	if((i != lpId) && (send_buffer[i].numberOfMsgs > 0)) {
	  flag = 1;
	  break;
	}
      }
      if(flag == 1) {
	logicalProcessStats::increment_waited_too_long();
      }
      logicalProcessStats::increment_waited_long_trigger();
#endif
#endif
      sendMessage();
      waitTime = 0;
    }
  }

  BasicTimeWarp* getCommManagerHandle() {
    return communicationManager;
  }
};

extern ostream& operator<<(ostream &os, const AggregationSendCriteria& asc);

#endif //MESSAGE_MANAGER_HH
