#ifndef LTSFINPUTQUEUE_HH
#define LTSFINPUTQUEUE_HH
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
// $Id: LTSFInputQueue.hh,v 1.7 1999/10/24 23:26:00 ramanan Exp $
//---------------------------------------------------------------------------
#include "config.hh"
#include <fstream.h>
#include <stdlib.h>
#include "BasicEvent.hh"
#include "MultiList.hh"

extern int BasicEventCompareRecv(const BasicEvent*, const BasicEvent*);

class LTSFInputQueue : public MultiList<BasicEvent> {
public:
  friend ostream& operator<< (ostream&, const LTSFInputQueue& );
  LTSFInputQueue();
  ~LTSFInputQueue();

  bool insert(BasicEvent *toInsert, int id);
  BasicEvent* getAndSeek(int id, int localId);
  BasicEvent* getAndSeekObj(int id, int localId);
  BasicEvent* find(VTime&, int, findMode_t = EQUAL, int id=0);
  BasicEvent* getEventToSchedule();
  void saveCurrent() {oldCurrentPtr = currentPos; };
  void restoreCurrent() {currentPos = oldCurrentPtr; };
  int gcollect(VTime&, int);
  int gcollect(VTime, int, BasicEvent*);

  int getNumberOfCommittedEvents(){ return noOfEventsCommitted; };

  VTime calculateMin();
  void print(ostream& = cout) const;
  void reset();
  void printMiniList(int);

  VTime getNextUnprocessedEventTime();

  bool initialized;
  BasicEvent *lastInserted;
  VTime lastGVT;
  int memoryUseage;

#ifdef STATEDEBUG
	ofstream *outFile;
	void setStateFile (ofstream *outfile) {
		outFile = outfile;
	}
#endif

protected:
  bool miniListRollback(BasicEvent* , int );
  void miniListUnprocessMessages(BasicEvent* , int );
  VTime getLVTForThisObj(int id);
  BasicEvent* oldCurrentPtr;
  BasicEvent* tmpCurrentPtr;
  BasicEvent *unprocessPtr;
  int noOfEventsCommitted;

};


inline VTime
LTSFInputQueue::calculateMin(){
  register VTime retval = PINFINITY;
  register BasicEvent *ptr;

  ptr = MultiList<BasicEvent>::get();
  if(ptr != NULL){
    retval = ptr->recvTime;
  }
  return retval;

}

inline VTime
LTSFInputQueue::getNextUnprocessedEventTime() {
  register VTime retval = PINFINITY;
  register BasicEvent* ptr = get();

  while((ptr != NULL) &&(ptr->alreadyProcessed == true)) {
    ptr = ptr->next;
  }
  if(ptr == NULL) {
    return retval;
  }
  else {
    return ptr->recvTime;
  }
}
#endif // LTSFINPUTQUEUE_HH
