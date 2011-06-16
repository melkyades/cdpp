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
#include "MessageManager.hh"
#include <fstream>
#include <cstdlib>
#include "CommMgrInterface.hh"

ofstream* gFileHandle;

//The global variables that will filled by the command line arguments
double gTanTheta = 0.0;
int gMaxReceiveDelay = 15;
int gMaximumAge = 20;
int gAggregateCtrlMsg = 0;
int gFlushFlag = 1;

//inline
MessageManager::MessageManager(int numLPs, BasicTimeWarp* commManager, CommPhyInterface *phyLib): numberOfLps(numLPs) {
  communicationManager = commManager;
  send_buffer = new Transceiver[numberOfLps];
  int i =0;
  for(i=0; i<numLPs; i++) {
    send_buffer[i].setLpId(i);
    send_buffer[i].setMessageManagerHandle(this);
    send_buffer[i].setCommPhyInterface(phyLib);
  }
  receive_buffer = NULL;
  numberOfMessagesToBeSent = 0;
  numberOfMessagesSentPreviously = 0;
  ageOfMessage = 0;
  receiveCriterion = 0;
  rollingBack = false;
  waitingForMessage = 0;
// The following lines have been commented.
// The following inputs namely tanTheta, maxReceiveDelay, 
//maximumAge and aggregateCtrlMsg are passed as commandline arguments 
//and they are initialized bythem

//   ifstream configFile("MessageManager.config");
//   configFile >> tanTheta;
//   configFile >> maxReceiveDelay;
//   configFile >> maximumAge;
//   configFile >> aggregateCtrlMsg;
  ageIncrementor = 0;
  tanTheta = gTanTheta;
  maxReceiveDelay = gMaxReceiveDelay;
  maximumAge = gMaximumAge;
  aggregateCtrlMsg = gAggregateCtrlMsg;
  newMaximumAge = gMaximumAge;
  previousMaximumAge = gMaximumAge;
  upCurve = true;
  downCurve = false;
  prevRate = 0.0;
  lVT = ZERO;
#ifdef FIXED_MSG_COUNT
       cout << "USING Fixed Msg count" << endl;
#endif
}

//inline
MessageManager::~MessageManager() {
  delete [] send_buffer;
}


//inline 
void 
MessageManager::initMessageManager(int mylpId) {
  lpId = mylpId;
  receive_buffer = &send_buffer[lpId];
}

inline void
MessageManager::probeToSend() {
  int i=0;
  for(i=0; i<numberOfLps; i++) {
    if(i != lpId) {
      send_buffer[i].probeToSend();
    }
  }
}

inline void
MessageManager::probeToReceive() {
  receive_buffer->probeToReceive();
}

inline bool
MessageManager::isEmptyInputQ() {
  if (this->communicationManager->inputQ.get() == NULL) {
    return true;
  }
  else {
    return false;
  }
}


// //This functions sets the rollingBack flag to true
// inline
// void MessageManager::setRollBackFlag() {
//   rollingBack = true;
// }

// //This functions sets the rollingBack flag to true
// inline
// void MessageManager::resetRollBackFlag() {
//   rollingBack = false;
// }

inline bool
MessageManager::isControlMessage(BasicMsg* msg) {
  switch(msg->type) {
  case INITMSG:
  case STARTMSG:
  case TERMINATEMSG:
  case CHECKIDLEMSG:
    return true;
    break;
  case LGVTMSG:
  case LOWERLGVTMSG:
  case NEWGVTMSG:
  case GVTACKMSG:
  case LPACKMSG:
  case DUMMYMSG:
  case DUMMYREQUESTMSG:
  case RESTORECKPTMSG:
#ifdef MESSAGE_AGGREGATION
  case TIMEWARPOBJMSG:
  case TRANSFEROBJMSG:
  case OUTPUTQOBJMSG:
  case INPUTQOBJMSG:
  case STATEQOBJMSG:
  case AGGREGATEMSG:
#endif
    return ((aggregateCtrlMsg == 0) ? true : false);
    break;

#ifdef MATTERNGVTMANAGER
  case MATTERNGVTMSG:
  case EXTAGENTMSG:
    return ((bool)(aggregateCtrlMsg == 0));
    break;
#endif
  case EVENTMSG:
    return false;
    break;
  default:
    cerr << "Unknown Message Type found in Messagae Aggregator" << endl;
    return true;
    break;
  };
}

inline void
MessageManager::incrementNumberOfMessagesToBeSent() {
  numberOfMessagesToBeSent++;
}

inline void
MessageManager::incrementReceiveCriterion() {
  receiveCriterion++;
}

inline 
void 
MessageManager::resetSendCriterionInfo() {
  //reset the age count
  //reset the number of messages
#ifdef MEAN_OF_FACTORS
  if(ageOfMessage == 0) {
    ageOfMessage = 1;
  }
  float currentRate = ((float)numberOfMessagesToBeSent) /
                      ((float)ageOfMessage);
  int newAge =0;
  if(currentRate < prevRate) {
    newAge = int(newMaximumAge - (prevRate - currentRate)*newMaximumAge);
    downCurve = true;
  }
  else {
    newAge = int(newMaximumAge + (currentRate - prevRate)*newMaximumAge);
    upCurve = true;
  }
  if((newAge <= (maximumAge/3.0)) || (10*maximumAge <= newAge)) {
    newMaximumAge = maximumAge;
  }
  else {
    newMaximumAge = newAge;
  }
  prevRate = currentRate;
  numberOfMessagesSentPreviously = numberOfMessagesToBeSent;
#endif
  ageOfMessage = 0;
  numberOfMessagesToBeSent = 0;
}


inline 
void 
MessageManager::resetReceiveCriterionInfo() {
  //reset the age count for receiving the messages
  receiveCriterion = 0;
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInFixedPessimism() {
  return ((ageOfMessage > maximumAge) ? SEND : DO_NOT_SEND );
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInFixedSlope() {
  //Check with Threshold curve to see, what has to be done with the message

  if((ageOfMessage > 10) &&
     (numberOfMessagesToBeSent + 1 < (ageOfMessage * ageOfMessage * tanTheta))) {
    return SEND;
  }
  return DO_NOT_SEND;
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInFixedSlopeWithError() {

  if(rollingBack == true) {
    return DO_NOT_SEND;
  }
  if(isEmptyInputQ()) {
#ifdef LPDEBUG
  *gFileHandle << "InputQ is empty and so Flushing the Q: " <<  endl;
#endif
    return SEND;
  }
  return getSendCriteriaInFixedSlope();
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInMeanOfFactors() {
  return ((ageOfMessage > newMaximumAge) ? SEND : DO_NOT_SEND );
}

inline
AggregationSendCriteria
MessageManager::getSendCriteria(BasicMsg* msg, int size) {
  
  if(!send_buffer[msg->destLP].canWriteMessage(size)) {
#ifdef LPDEBUG
    *gFileHandle << "Aggregation Buffer FULL and so sending the message: " <<  endl;
#endif
    if(isControlMessage(msg)) {
#ifdef LPDEBUG
      *gFileHandle << "Got a control Message: " << msg->type  << endl;
#endif
      return SEND_WRITE_SEND;
    }
    else {
      return SEND_AND_WRITE;
    }
  }
  else {
    if(isControlMessage(msg)) {
#ifdef LPDEBUG
      *gFileHandle << "Got a control Message: " << msg->type  << endl;
#endif
      return WRITE_AND_SEND;
    }
    else {
      return WRITE;
    }
  }
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInFixedMsgCount() {
  if (numberOfMessagesToBeSent >= maximumAge) {
    return SEND;
  }
  else {
    return DO_NOT_SEND;
  }
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInFixedMsgCount(BasicMsg* msg, int size) {
  AggregationSendCriteria criteria = getSendCriteria(msg, size);
  if(criteria == WRITE) {
    return getSendCriteriaInFixedMsgCount();
  }
  else {
    return criteria;
  }
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInFixedPessimism(BasicMsg* msg, int size) {
  AggregationSendCriteria criteria = getSendCriteria(msg, size);
  if(criteria == WRITE) {
    return getSendCriteriaInFixedPessimism();
  }
  else {
    return criteria;
  }
}

//This function should be able to call a set of functions depending on the 
//criteria. currently, the ifdef's will be used
inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInFixedSlope(BasicMsg* msg, int size) {
  //Check with Threshold curve to see, what has to be done with the message
  AggregationSendCriteria criteria = getSendCriteria(msg, size);
  if(criteria == WRITE) {
    return getSendCriteriaInFixedSlope();
  }
  else {
    return criteria;
  }
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInFixedSlopeWithError(BasicMsg* msg, int size) {
  //(1)See if we can write the message
  //(2)See if it is a control message, then flush it
  //(3)See if the LP is rolling back,
  //   Then aggregate till the roll back is over.
  //(4)See if the inputQ is empty. If it is empty then flush it
  //(5)See if the message rate is the expected message rate
  //Step (1) & (2)
  AggregationSendCriteria criteria = getSendCriteria(msg, size);
  if(criteria == WRITE) {
    //step (3), (4) & (5)
    return getSendCriteriaInFixedSlopeWithError();
  }
  else {
    return criteria;
  }
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInMeanOfFactors(BasicMsg* msg, int size) {
  //The strategy is as follows
  //(1) send if there is no space
  //(2) If it is a control message do accordingly
  //(3) If the time is less then the calculated maximum age then aggregate
  //Step (1) and (2)
  AggregationSendCriteria criteria = getSendCriteria(msg, size);
  if(criteria == WRITE) {
    //step (3)
    return getSendCriteriaInMeanOfFactors();
  }
  else {
    return criteria;
  }
}

inline 
AggregationReceiveCriteria
MessageManager::getReceiveCriteria() {
  //Check with the age count to see when to receive the message

  if((receive_buffer->numberOfMsgs <= 0) &&
     (receiveCriterion >= maxReceiveDelay)) {
#ifdef LPDEBUG
    *gFileHandle << "Reached the receive criteria and so receiving: "  << endl;
#endif
    return RECEIVE;
  }
  else {
    return NO_NEED_TO_RECEIVE;
  }
}

inline void 
MessageManager::writeMessage(char* msg, int size, int mylpId) {
#ifdef ADAPTIVE_AGGREGATION
  send_buffer[mylpId].setReceiveTimes((BasicMsg*)msg);
#endif
#ifdef PROBE_SEND_RECEIVE_MESSAGES
  send_buffer[mylpId].setReceiveTimes((BasicMsg*)msg);
#endif
  send_buffer[mylpId].numberOfMsgs++;
  send_buffer[mylpId].writeMsgSize(size);
  send_buffer[mylpId].writeMessage(size, msg);    

  incrementNumberOfMessagesToBeSent();
  setAgeIncrementor(1);
}

inline 
void 
MessageManager::checkToReceive() {
  AggregationReceiveCriteria whatToDo;
  
  whatToDo = getReceiveCriteria();
  switch(whatToDo) {
  case RECEIVE:
    receive_buffer->receiveMessage();
    resetReceiveCriterionInfo();
    break;
  case NO_NEED_TO_RECEIVE:
    break;
  };
}

//inline 
void 
MessageManager::sendMessage() {
#ifdef LPDEBUG
  *gFileHandle << "Sending Messages in the Aggregated queue: " <<  endl;
  *gFileHandle << "Number of Messages is : " << numberOfMessagesToBeSent << endl;
  *gFileHandle << "Age is: " << ageOfMessage << endl;
#endif
  int i=0;
  for(i=0; i<numberOfLps; i++) {
    if((i != lpId) && (send_buffer[i].numberOfMsgs > 0)) {
      send_buffer[i].sendMessage();
    }
  }
  resetSendCriterionInfo();
}

inline 
void 
MessageManager::checkToSend() {

  AggregationSendCriteria whatToDo;
  
#ifdef FIXED_PESSIMISM
  whatToDo = getSendCriteriaInFixedPessimism();
#elif defined( FIXED_SLOPE)
  whatToDo = getSendCriteriaInFixedSlope();
#elif defined( FIXED_SLOPE_WITH_ERROR)
  whatToDo = getSendCriteriaInFixedSlopeWithError();
#elif defined( MEAN_OF_FACTORS)
  whatToDo = getSendCriteriaInMeanOfFactors();
#elif defined( FIXED_MSG_COUNT)
  whatToDo = getSendCriteriaInFixedMsgCount();
#else
  whatToDo = getSendCriteriaInFixedPessimism();
#endif

  switch(whatToDo) {
  case SEND:
    sendMessage();
    break;
  default:
    break;
  }

}

#if defined (ADAPTIVE_AGGREGATION) || defined(PROBE_SEND_RECEIVE_MESSAGES)
void 
MessageManager::checkToSend(VTime nextRecvTime) {
  int i=0;
#ifdef ADAPTIVE_AGGREGATION
  for(i=0; i<numberOfLps; i++) {
    if((i != lpId) && (send_buffer[i].hasMessagesInFuture(nextRecvTime))) {
      send_buffer[i].sendMessage();
      send_buffer[i].setLeastReceiveTime(send_buffer[i].getMaxReceiveTime());
    }
  }
#endif
#ifdef PROBE_SEND_RECEIVE_MESSAGES
  lVT = nextRecvTime;
  for(i=0; i<numberOfLps; i++) {
    if((i != lpId) && (send_buffer[i].canSend(lVT))) {
      send_buffer[i].sendMessage();
    }
  }
#endif
}
#endif

//inline 
void
MessageManager::incrementAgeOfMessage() {
  ageOfMessage = ageOfMessage + ageIncrementor;
  //  checkToSend();
}

//inline 
void 
MessageManager::sendMessage(BasicMsg* msg, int size) {

  AggregationSendCriteria whatToDo;
  
#ifdef FIXED_PESSIMISM
  whatToDo = getSendCriteriaInFixedPessimism(msg, size);
#elif defined( FIXED_SLOPE)
  whatToDo = getSendCriteriaInFixedSlope(msg, size);
#elif defined( FIXED_SLOPE_WITH_ERROR)
  whatToDo = getSendCriteriaInFixedSlopeWithError(msg, size);
#elif defined( MEAN_OF_FACTORS)
  whatToDo = getSendCriteriaInMeanOfFactors(msg, size);
#elif defined( FIXED_MSG_COUNT)
  whatToDo = getSendCriteriaInFixedMsgCount(msg, size);
#elif defined( ADAPTIVE_AGGREGATION)
  whatToDo = getSendCriteria(msg, size);
#elif defined( PROBE_SEND_RECEIVE_MESSAGES)
  whatToDo = getSendCriteria(msg, size);
#else
  whatToDo = getSendCriteriaInFixedPessimism(msg, size);
#endif

  switch(whatToDo) {
  case SEND_AND_WRITE:
#ifdef STATS
#ifdef MESSAGE_AGGREGATION
    logicalProcessStats::increment_write_buffer_full();
#endif
#endif
    sendMessage();
    //    send_buffer[msg->destLP].sendMessage();
    writeMessage((char*)msg, size, msg->destLP);
    break;
  case WRITE_AND_SEND:
#ifdef STATS
#ifdef MESSAGE_AGGREGATION
    logicalProcessStats::increment_received_control_message();
#endif
#endif
  case SEND:
    writeMessage((char*)msg, size, msg->destLP);
#ifdef STATS
#ifdef MESSAGE_AGGREGATION
    logicalProcessStats::increment_window_size_exhausted();
#endif
#endif
    //    send_buffer[msg->destLP].sendMessage();
    sendMessage();
    break;
  case WRITE:
    writeMessage((char*)msg, size, msg->destLP);
    break;
  case SEND_WRITE_SEND:
#ifdef STATS
#ifdef MESSAGE_AGGREGATION
    logicalProcessStats::increment_write_buffer_full();
    logicalProcessStats::increment_received_control_message();
#endif
#endif
    sendMessage();
    writeMessage((char*)msg, size, msg->destLP);
    sendMessage();
    break;
  case DO_NOT_SEND:
    writeMessage((char*)msg, size, msg->destLP);
    break;
  default:
    cerr << "Invalid Aggregattion Criterion got, Writing the Message Anyway";
    cerr << " " << whatToDo << endl;
    writeMessage((char*)msg, size, msg->destLP);
    break;
  }

#ifdef LPDEBUG
  *gFileHandle << "Got the criteria:(S=0, W =1, SW =2, WS=3 ):" << whatToDo << endl;
#endif
  incrementReceiveCriterion();
  checkToReceive();
}

inline char*
MessageManager::readMessage() {
  receive_buffer->numberOfMsgs--;
  int size = receive_buffer->readMsgSize();
  char* message = new char[size];
  receive_buffer->readMessage(size,message);
  //#if defined(MESSAGE_AGGREGATION) && defined(STATS)
#ifdef STATS
  CommMgrInterface* tempPtr = (CommMgrInterface *)(communicationManager);
  tempPtr->incrementReceivedApplicationMessageCount();
#endif

  return message;
}

//inline 
BasicMsg* 
MessageManager::receiveMessage() {

  BasicMsg* msg = NULL;

  if(receive_buffer->numberOfMsgs > 0) {
    msg = (BasicMsg*)readMessage();
  }
  else {
    receive_buffer->receiveMessage();
    if(receive_buffer->numberOfMsgs > 0) {
      msg = (BasicMsg*)readMessage();
      resetReceiveCriterionInfo();
    }
    else {
      checkToSend();
    }
  }
#ifdef PROBE_SEND_RECEIVE_MESSAGES
  updateInputReceiveInfo(msg);
#endif
  return msg;
}

ostream&
operator<<(ostream &os, const AggregationSendCriteria& asc) {
  os << " ";
  switch (asc) {
  case SEND:
    os << "Send";
    break;
  case WRITE:
    os << "Write";
    break;
  case SEND_AND_WRITE:
    os << "SendAndWrite";
    break;
  case WRITE_AND_SEND:
    os << "WriteAndSend";
    break;
  case DO_NOT_SEND:
    os << "DoNotSend";
    break;
  case SEND_WRITE_SEND:
    os << "SendWriteSend";
    break;
  case DO_NOT_KNOW:
    os << "DoNotKnow";
    break;
  default:
    os << "[Error!!]";
  }

  os << " ";

  return os;
}
    
