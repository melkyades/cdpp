//-*-c++-*-
#ifndef BASICEVENT_CC
#define BASICEVENT_CC
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
// $Id: TimeWarpBasicEvent.cc,v 1.5 1999/10/24 23:26:24 ramanan Exp $
//
//---------------------------------------------------------------------------
#include "BasicEvent.hh"

ostream&
operator<<(ostream& os, const BasicEvent& e) {
  os << "sTime: " << e.sendTime << " rTime: " << e.recvTime << " sendID: " 
     << e.sender << " dest: " << e.dest << " Processed: " << e.alreadyProcessed 
     << " sign: " << e.sign << " eventId: " << e.eventId;
#ifdef MATTERNGVTMANAGER
  os << " color: " << e.color;
#endif

return os;
}

// generic lazy compare function
bool
BasicEvent::lazyCmp(BasicEvent* rhs) {
  if((rhs->recvTime == this->recvTime) && 
     (rhs->sendTime == this->sendTime) && 
     (rhs->sender == this->sender) && (rhs->dest == this->dest) && 
     (rhs->sign == this->sign)){
    return(true);
  }
  else {
    return false;
  }
}

BasicEvent::BasicEvent(VTime sTime, VTime rTime, unsigned send, 
		       unsigned recv, SIGN sgn, SequenceCounter evId, 
		       unsigned sz) {
  next = NULL;
  prev = NULL;
  sendTime = sTime;
  recvTime = rTime;
  sender = send;
  dest = recv;
  eventId = evId;
  sign = sgn;
  incarnationNumber = 0;
  Qptrs = 0;
  size = sz;
  alreadyProcessed = false;
  nextObj = NULL;
  prevObj = NULL;
#ifdef MATTERNGVTMANAGER
  color = 0;
#endif
}

BasicEvent::BasicEvent(const BasicEvent *be) {
  next = NULL;
  prev = NULL;
  sendTime = be->sendTime;
  recvTime = be->recvTime;
  sender = be->sender;
  dest = be->dest;
  eventId = be->eventId;
  sign = be->sign;
  incarnationNumber = be->incarnationNumber;
  Qptrs = 0;
  size = be->size;
  alreadyProcessed = false;
  nextObj = NULL;
  prevObj = NULL;
#ifdef MATTERNGVTMANAGER
  color = 0;
#endif
}

#endif






