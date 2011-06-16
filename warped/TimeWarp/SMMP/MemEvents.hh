//-*-c++-*-
#ifndef MEMEVENTS_HH
#define MEMEVENTS_HH
// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.
//
// BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY 
// FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT 
// PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, 
// EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE 
// PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME 
// THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION. 
//
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
// $Id: MemEvents.hh,v 1.1 1998/03/07 21:04:54 dmadhava Exp $
//
//
//---------------------------------------------------------------------------


#include "BasicEvent.hh"

class MemRequest : public BasicEvent {
public:
  MemRequest(){
    startTime = 0;
    processor = 0;
    size = sizeof(MemRequest);
  };
  ~MemRequest(){};
  void print(){
    cout << "sTime: " << sendTime << " rTime: " << recvTime << " sendID: " 
	 << sender << " dest: " << dest << " sign: " << sign
	 << " eventId: " << eventId;
#if defined(LTSFSCHEDULER) && defined(MATTERNGVTMANAGER)
    cout << " color: " << color << " Processed: " << alreadyProcessed;
#elif defined (LTSFSCHEDULER) || defined(SEQUENTIAL)
    cout << " size: " << size << " Processed: " << alreadyProcessed;
#else
    cout << " size: " << size;
#endif
    cout << " startTime: " << startTime << " processor: " << processor;
    cout << std::endl;
  };
  
#if defined(LAZYCANCELLATION) || defined(LAZYAGGR_CANCELLATION)
  bool lazyCmp(BasicEvent* rhs){
    register MemRequest* tmp = (MemRequest *)rhs;
    if((tmp->startTime == this->startTime) &&
       (tmp->processor == this->processor)){
      //cout << "comparison is true" << std::endl;
      return BasicEvent::lazyCmp(rhs);
    }
    else {
      //cout << "comparison is false" << std::endl;
      return false;
    }
  };
#endif

  VTime startTime;
  int processor;
};

#endif
