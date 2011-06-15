//-*-c++-*-
#ifndef RAIDTYPES_HH
#define RAIDTYPES_HH
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
//
// $Id: RAIDTypes.hh,v 1.1 1998/03/07 21:04:03 dmadhava Exp $
//
// 
//
//---------------------------------------------------------------------------


#include <stdlib.h>
#include "BasicEvent.hh"

enum DISK_TYPE {FUJITSU, FUTUREDISK, LIGHTNING};

class RAIDRequest : public BasicEvent {
  friend ostream& operator<<(ostream& os, RAIDRequest& rq);
public:
  // Who created me.
  int sourceId;
  // Who where I'm going.
  int serverId;
  // Who's going to consume me.
  int sinkId;
  // The starting location for our stripe request
  int startStripe;
  int logicalSector;
  int startCylinder;
  int startSector;
  int beginningOfStrype;
  int tokenNumber;
  // Size of the read or write
  int sizeRead;
  // How many parity messages the process should expect.
  int sizeParity;
  // If this is a parity message or not.
  bool parityMsg;
  // Is this request for a read or write.
  bool read;

  RAIDRequest(): sourceId(0), serverId(0), sinkId(0), startStripe(0),
                 logicalSector(0), startCylinder(0), startSector(0), 
                 beginningOfStrype(0), tokenNumber(0), sizeRead(0),  
                 sizeParity(0), parityMsg(false), read(true) {
  }
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
    cout << " startStripe: " << startStripe << " LogicalSector: " 
	 << logicalSector << " startCyl: " << startCylinder 
	 << " startSector: " << startSector << " sizeRead: " << sizeRead
	 << " sizeParity: " << sizeParity << " parityMsg: " << parityMsg 
	 << " read: " << read << endl;
  };

#if defined(LAZYCANCELLATION) || defined(LAZYAGGR_CANCELLATION)
  bool lazyCmp(BasicEvent* rhs){
    register RAIDRequest* tmp = (RAIDRequest *)rhs;
    if((tmp->startStripe == this->startStripe) &&
       (tmp->logicalSector == this->logicalSector) &&
       (tmp->startCylinder == this->startCylinder ) && 
       (tmp->startSector == this->startSector ) &&
       (tmp->sizeRead == this->sizeRead ) && 
       (tmp->sizeParity == this->sizeParity ) &&
       (tmp->parityMsg == this->parityMsg ) && 
       (tmp->read == this->read )){
      //cout << "comparison is true" << endl;
      return BasicEvent::lazyCmp(rhs);
    }
    else {
      //cout << "comparison is false" << endl;
      return false;
    }
  };
#endif
};

inline ostream&
operator<< (ostream& os, RAIDRequest& rq) {

  os << "beginningOfStrype [" << rq.beginningOfStrype
     << "] stripe[ " << rq.startStripe
     << " ] sizeRead [" << rq.sizeRead
     << "] " << " token#[" << rq.tokenNumber <<"] ParityMsg[";
  if (rq.parityMsg == true) {
    os << "T]\n";
  }
  else {
    os << "F]\n";
  }
  os << (BasicEvent& ) rq << endl;

  return(os);

}

#endif
