//-*-c++-*-
#ifndef KERNEL_MSGS_HH
#define KERNEL_MSGS_HH
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
//
// $Id: KernelMsgs.hh,v 1.5 1999/05/10 04:15:27 cyoung Exp $
//
// 
//
//---------------------------------------------------------------------------
#include "BasicEvent.hh"

// this is structure carries gVT messages from LP to LP
enum KernelMsgType { 

  INITMSG = 1,
  STARTMSG = 2,
  TERMINATEMSG = 3,
  CHECKIDLEMSG = 4,
  EVENTMSG = 5,
  LGVTMSG = 6,
  LOWERLGVTMSG = 7,
  NEWGVTMSG = 8,
  GVTACKMSG = 9,
  LPACKMSG = 10,
  DUMMYMSG = 11,
  DUMMYREQUESTMSG = 12,
  MATTERNGVTMSG = 13,
  RESTORECKPTMSG = 14,
  TIMEWARPOBJMSG = 15,
  TRANSFEROBJMSG = 16,
  OUTPUTQOBJMSG = 17,
  INPUTQOBJMSG = 18,
  STATEQOBJMSG = 19,
  EXTAGENTMSG = 20,
  AGGREGATEMSG = 21
};

class BasicMsg {
public:
  friend ostream & operator<<(ostream&, const BasicMsg & );
	
  BasicMsg(){};
  BasicMsg(KernelMsgType kMT) : type(kMT) {};
  SequenceCounter sequence;  // needed to avoid message acknowledgment in the
	// general case
  int senderLP;
  int destLP;
  KernelMsgType type;
};


#ifdef MATTERNGVTMANAGER
// basic control message in Mattern's algorithm
class MatternGVTMsg : public BasicMsg {
public:
  friend ostream & operator<<(ostream&, const MatternGVTMsg & );
  MatternGVTMsg() : BasicMsg(MATTERNGVTMSG){};
  // don't know if a control message needs a color field
  //  int color; // color of the message; can be 0, 1 or 2
  VTime mClock; // minimum of the local clocks
  VTime mSend; // minimum of timestamps
  SequenceCounter tokenNum;
  SequenceCounter count; // vector counts
};
#endif

#ifdef EXTERNALAGENT
// basic control message for external control of the dynamic optimizations
class ExtAgentMsg : public BasicMsg {
public:
  friend ostream & operator<<(ostream&, const ExtAgentMsg & );
  ExtAgentMsg() : BasicMsg(EXTAGENTMSG){};
  int eventsSoFar;
};
#endif

class InitMsg : public BasicMsg {
public:
  friend ostream & operator<<(ostream&, const InitMsg & );
  InitMsg() : BasicMsg(INITMSG){};
  int objId; // this is id of the object I'm telling you about
  int lpId;  // the LP that has it
};

class StartMsg : public BasicMsg {
public:
  StartMsg() : BasicMsg(STARTMSG){};
};

class TerminateMsg : public BasicMsg {
public:
  friend ostream & operator<<(ostream&, const TerminateMsg & );
  TerminateMsg() : BasicMsg(TERMINATEMSG){};
  char error[255];
};

class CheckIdleMsg : public BasicMsg {
public:
  friend ostream & operator<<(ostream&, const CheckIdleMsg & );
  CheckIdleMsg() : BasicMsg(CHECKIDLEMSG){};
  int tokenNum;
  int numCirculations;
  bool cancel;
};

class EventMsg : public BasicMsg, public BasicEvent {
public:
  friend ostream & operator<<(ostream&, const EventMsg& );
  EventMsg() : BasicMsg(EVENTMSG) {};
};

class LGVTMsg : public BasicMsg{
public:
  friend ostream & operator<<(ostream&, const LGVTMsg& );
  LGVTMsg() : BasicMsg(LGVTMSG){ };
  VTime lGVT;  // The estimate of that the LP is sending out
  bool lpIdle;
};

class LowerLGVTMsg : public BasicMsg{
  // this tells the gVT guy that I am lowering my estimate of gVT.  This 
  // means that I will immediately require an acknowledgement.
public:
  friend ostream & operator<<(ostream&, const LowerLGVTMsg& );
  LowerLGVTMsg() : BasicMsg(LOWERLGVTMSG){ };
  VTime lGVT;  // The estimate of that the LP is sending out
  bool lpIdle;
};

struct newGVTMsg : public BasicMsg{
public:
  friend ostream & operator<<(ostream&, const newGVTMsg& );
  newGVTMsg() : BasicMsg(NEWGVTMSG){};
  VTime newGVT;  // The estimate of that the LP is sending out
};

struct GVTAckMsg : public BasicMsg{
public:
  GVTAckMsg() : BasicMsg(GVTACKMSG){};
  SequenceCounter sequenceMsgAcked;
};

struct LPAckMsg : public BasicMsg{
public:
  LPAckMsg() : BasicMsg(LPACKMSG){};
  SequenceCounter sequenceMsgAcked;
};

struct DummyMsg : public BasicMsg{
public:
  DummyMsg() : BasicMsg(DUMMYMSG){};
};

struct DummyRequestMsg : public BasicMsg{
public:
  DummyRequestMsg() : BasicMsg(DUMMYREQUESTMSG){};
};

struct RestoreCkptMsg : public BasicMsg{
public:
  friend ostream & operator<<(ostream&, const RestoreCkptMsg& );
  RestoreCkptMsg() : BasicMsg(RESTORECKPTMSG){};
  VTime checkpointTime;  // The estimate of that the LP is sending out
  int numCirculations;
  bool checkpointConsensus;
  SequenceCounter tokenNum;

};

extern ostream & operator<<(ostream&, const InitMsg& );
extern ostream & operator<<(ostream&, const BasicMsg& );
extern ostream & operator<<(ostream&, const TerminateMsg& );

#endif
