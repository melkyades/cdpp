//-*-c++-*-
#ifndef KERNEL_MSGS_CC
#define KERNEL_MSGS_CC
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
// $Id: KernelMsgs.cc,v 1.3 1999/05/10 04:15:26 cyoung Exp $
//
//
//---------------------------------------------------------------------------

#include "KernelMsgs.hh"


ostream&
operator<<(ostream& os, const BasicMsg &e) {
  os << "sequence: " << e.sequence << " senderLP: " << 
    e.senderLP << " destLP: " << e.destLP << " type: ";
  switch(e.type) {
  case INITMSG:
    os << "INITMSG";
    break;
  case STARTMSG:
    os << "STARTMSG";
    break;
  case TERMINATEMSG:
    os << "TERMINATEMSG";
    break;
  case CHECKIDLEMSG:
    os << "CHECKIDLEMSG";
    break;
  case EVENTMSG:
    os << "EVENTMSG";
    break;
  case LGVTMSG:
    os << "LGVTMSG";
    break;
  case LOWERLGVTMSG:
    os << "LOWERLGVTMSG";
    break;
  case NEWGVTMSG:
    os << "NEWGVTMSG";
    break;
  case GVTACKMSG:
    os << "GVTACKMSG";
    break;
  case LPACKMSG:
    os << "LPACKMSG";
    break;
  case DUMMYMSG:
    os << "DUMMYMSG";
    break;
  case DUMMYREQUESTMSG:
    os << "DUMMYREQUESTMSG";
    break;
  case RESTORECKPTMSG:
    os << "RESTORECKPTMSG";
    break;
#ifdef MATTERNGVTMANAGER
  case MATTERNGVTMSG:
    os << "MATTERNGVTMSG";
    break;
#endif
#ifdef EXTERNALAGENT
  case EXTAGENTMSG:
    os << "EXTAGENTMSG";
    break;
#endif
  default:
    os << "UNKNOWN";
  }
  return os;
}



#ifdef MATTERNGVTMANAGER
ostream&
operator<<(ostream& os, const MatternGVTMsg &e) {
  os << (BasicMsg)e;
  os << " mClock : " << e.mClock << " mSend : " << e.mSend 
     << " token : " << e.tokenNum << " count : " << e.count;
  return os;
}
#endif

#ifdef EXTERNALAGENT
ostream&
operator<<(ostream& os, const ExtAgentMsg &e) {
  os << (BasicMsg)e;
  os << " events so far : " << e.eventsSoFar ;
  return os;
}
#endif

ostream&
operator<<(ostream& os, const InitMsg &e) {
  os << (BasicMsg)e;
  os << " objId : " << e.objId << " lpId " << e.lpId;
  return os;
}


ostream&
operator<<(ostream& os, const TerminateMsg &e) {
  os << (BasicMsg&)e;
  os << " error : " << e.error;

  return os;
}

ostream&
operator<<(ostream& os, const CheckIdleMsg &e) {
  os << (BasicMsg&)e;
  os << " tokenNum : " << e.tokenNum << " numCirculations : "
    << e.numCirculations << " cancel : " << e.cancel;
  return os;
}

int EventMsgCompare( EventMsg* a, EventMsg* b ) {
  int retval;

  if( ((BasicEvent*)a)->recvTime >
     ((BasicEvent*)b)->recvTime) {
    retval = 1;
  }
  else if ( ((BasicEvent*)a)->recvTime >
     ((BasicEvent*)b)->recvTime ) {
    retval = -1;
  }
  else {
    retval = 0;
  }
  return retval;
}

ostream&
operator<<(ostream& os, const EventMsg &e) {
  os << (BasicMsg)e << " " << (BasicEvent)e;
  return os;
}

ostream&
operator<<(ostream& os, const LGVTMsg &e) {
  os << (BasicMsg)e << " lGVT : " << e.lGVT << " lpIdle : " 
    << e.lpIdle << endl;
  return os;
}

ostream&
operator<<(ostream& os, const LowerLGVTMsg &e) {
  os <<  (BasicMsg)e << " lGVT : " << e.lGVT <<" lpIdle : " 
    << e.lpIdle << endl;
  return os;
}

ostream&
operator<<(ostream& os, const newGVTMsg &e) {
  os <<  (BasicMsg)e << " newGVT : " << e.newGVT << endl;
  return os;
}

ostream&
operator<<(ostream& os, const GVTAckMsg &e ) {
  os << (BasicMsg)e << " msgAcked : " << e.sequenceMsgAcked;
  return os;
}

ostream&
operator<<(ostream& os, const LPAckMsg &e ) {
  os << (BasicMsg)e << " mesgAcked : " << e.sequenceMsgAcked;
  return os;
}

ostream&
operator<<(ostream& os, const RestoreCkptMsg &e) {
  os <<  (BasicMsg)e << " restoring from : " << e.checkpointTime 
     << " circulations " << e.numCirculations << " tokenNum: " << e.tokenNum
     << " consensus: ";
  if (e.checkpointConsensus == true) {
    os << "true";
  }
  else {
    os << "false";
  }
  return os;
}

void printCasted( ostream &os, const BasicMsg *msg ){
  switch(msg->type) {
  case INITMSG:
    os << *(InitMsg *) msg << endl;
    break;
    
  case STARTMSG:
    os << *(StartMsg *) msg << endl;
    break;

  case TERMINATEMSG:
    os << *(TerminateMsg *) msg  << endl;
    break;
  
  case CHECKIDLEMSG:
    os << *(CheckIdleMsg *) msg  << endl;
    break;

  case EVENTMSG:
  { // braces enclose data declaration
    EventMsg *foo = (EventMsg*) msg;
    os << *foo  << endl;
    break;
  }

  case LGVTMSG:
    os << *(LGVTMsg*) msg  << endl;
    break;

  case LOWERLGVTMSG:
    os << *(LowerLGVTMsg*) msg  << endl;
    break;

  case NEWGVTMSG:
    os << *(newGVTMsg*) msg  << endl;
    break;

  case GVTACKMSG:
    os << *(GVTAckMsg *) msg  << endl;
    break;

  case LPACKMSG:
    os << *(LPAckMsg *) msg  << endl;
    break;

  case DUMMYMSG:
    os << *(DummyMsg *) msg  << endl;
    break;

  case DUMMYREQUESTMSG:
    os << *(DummyRequestMsg *) msg  << endl;
    break;

  case RESTORECKPTMSG:
    os << *(RestoreCkptMsg*) msg  << endl;
    break;

#ifdef MATTERNGVTMANAGER
  case MATTERNGVTMSG:
    os << *(MatternGVTMsg *) msg  << endl;
    break;
#endif
  default:
    os << "AAAAAAAAAAAAAAAAAAAAAHHHHHHHHHHHHHHHHH" << endl;
    break;
  }
}
#endif
