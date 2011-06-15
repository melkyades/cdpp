// -*-c++-*-
#ifndef BASICEVENT_HH
#define BASICEVENT_HH
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
// $Id: TimeWarpBasicEvent.hh,v 1.4 1999/10/24 23:26:25 ramanan Exp $
//
//---------------------------------------------------------------------------
#include <stddef.h>
#include "warped.hh"
#include "config.hh"
#include "bool.hh"

class BasicEvent;
extern "C" {
  int deleteIfUnreferenced(BasicEvent*);
}

class BasicEvent {
public:
  friend ostream& operator<<(ostream&, const BasicEvent &);
  
  BasicEvent() { 
    sign = POSITIVE; 
    next = prev = NULL;
    eventId = 0;
    Qptrs = 0;
    incarnationNumber = 0;
    alreadyProcessed = false ; // flag used during rollback
    nextObj = NULL;
    prevObj = NULL;
  };

  BasicEvent(const BasicEvent*);
  BasicEvent(VTime, VTime, unsigned, unsigned, SIGN, SequenceCounter, 
	     unsigned);

  virtual ~BasicEvent() {};

  virtual void print(){};

  virtual bool lazyCmp(BasicEvent*);

  // Class Data
public:
  BasicEvent *next;
  BasicEvent *prev;
  VTime sendTime;                  // send time of the event
  VTime recvTime;                  // receive time of the event
  int sender;                      // id of sending object
  int dest;                        // id of receiving object
  SequenceCounter eventId;         // tuple of this and sender marks
                                   // a message as unique
  SIGN sign;                       // message or antimessage
  int incarnationNumber;
  // The number of inputQ and outputQ pointers referencing this event.
  int Qptrs;
  unsigned size;                   // size of this message

  BasicEvent *nextObj;
  BasicEvent *prevObj;

  bool alreadyProcessed; // flag used during rollback
#ifdef MATTERNGVTMANAGER
  int color; //color of the message - can be 0, 1, 2, etc(Mattern's alg.)
#endif
};

// For LTSF Scheduling, i.e. with the inputQ using a MultiList, we want to
// sort the list first by time, and then by object id.  This prevents us
// from needing a currentPos pointer for each object and allows us to
// simply progress in a linear fashion through the inputQ. This implies
// that, for all objects with events at a given time, the lower-id'd ones
// will be scheduled first (arbitrarily).
// !!! 26/01/2001 This function was changed to order events at the same time
// and to the same dest by arrival order.
inline
int BasicEventCompareRecv(const BasicEvent *a, const BasicEvent *b) { 
#ifdef USE_USER_VTIME
  if ( a->recvTime > b->recvTime ||
       (a->recvTime == b->recvTime && a->dest >= b->dest)) {
    return 1;
  }
  else if ( a->recvTime < b->recvTime ||
       (a->recvTime == b->recvTime && a->dest < b->dest)) {
    return -1;
  }
  return 0;
#else
  register int retval = a->recvTime - b->recvTime;
  if (retval == 0) {
    if (a->dest > b->dest) {
      retval = 1;
    }
    else if (a->dest < b->dest) {
      retval = -1;
    }
    // else retval remains 0
  }
  return retval;
#endif
}

inline
int BasicEventCompareSend(const BasicEvent *a, const BasicEvent *b) {
#ifdef USE_USER_VTIME
  if ( a->sendTime > b->sendTime ) {
    return 1;
  }
  else if ( a->sendTime < b->sendTime ) {
    return -1;
  }
  return  0;
#else
  return  ( a->sendTime - b->sendTime );
#endif
}

extern ostream& operator<<(ostream& os, const BasicEvent& e);

#endif

