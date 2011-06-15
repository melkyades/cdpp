//-*-c++-*-
#ifndef COMM_MGR_MSG_AGG_HH
#define COMM_MGR_MSG_AGG_HH
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
// $Id: CommMgrMsgAgg.hh,v 1.7 1999/10/24 23:25:51 ramanan Exp $
//
//
//---------------------------------------------------------------------------
#include "CommMgrInterface.hh"
#include "MessageManager.hh"

class CommPhyInterface;

class CommMgrMsgAgg : public CommMgrInterface {
public:
  CommMgrMsgAgg(int numberOfLPs, CommPhyInterface *phyLib);
  ~CommMgrMsgAgg();
  virtual void initCommManager(ObjectRecord *sim, FOSSIL_MANAGER *gvt,
			       int lpNum, LogicalProcess* lpHandle);
  virtual BasicMsg * recvMPIMsg();
  virtual void remoteSend(BasicMsg *, int);
  virtual int recvMPI(int);
  void incrementAgeOfMessage(){
    msgMgr.incrementAgeOfMessage();
  }
  inline void setRollingBackFlagInMessageManager(){
    msgMgr.setRollBackFlag();
  }
  inline void resetRollingBackFlagInMessageManager(){
    msgMgr.resetRollBackFlag();
  }
  inline void flush(){
#ifdef PROBE_SEND_RECEIVE_MESSAGES
    msgMgr.calculateAggregationStatistics();
#endif
    msgMgr.sendMessage();
#ifdef STATS
#ifdef MESSAGE_AGGREGATION
    if(msgMgr.hasMessagesToSend() == 1) {
      logicalProcessStats::increment_waited_too_long();
    }
#endif
#endif
  }
  inline void flushIfAgeExceeded(){
    msgMgr.flushIfAgeExceeded();
  }
#if defined(ADAPTIVE_AGGREGATION) || defined(PROBE_SEND_RECEIVE_MESSAGES)
  inline void checkToSend(VTime nextRecvTime){
    msgMgr.checkToSend(nextRecvTime);
  }
#endif
#ifdef FIXED_MSG_COUNT
  inline void flushIfWaitedTooLong(){
    msgMgr.flushIfWaitedTooLong(waitedWithNoInputMessages);
  }
#endif

#ifdef STATS
  void incrementStatsArray(int numberOfMsgs);
#endif
  
private:
  MessageManager msgMgr;
#ifdef FIXED_MSG_COUNT
  int waitedWithNoInputMessages;
#endif
};

extern void getMessageManagerParameters(int argc, char **argv);



#endif // #ifndef COMM_MGR_DEFAULT_HH
