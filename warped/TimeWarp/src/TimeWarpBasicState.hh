//-*-c++-*-
#ifndef BASIC_STATE_HH
#define BASIC_STATE_HH
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
// $Id: TimeWarpBasicState.hh,v 1.3 1999/10/24 23:26:26 ramanan Exp $
//
//
//---------------------------------------------------------------------------
#include "warped.hh"
#include "config.hh"

#include "BasicEvent.hh"
#include "Container.hh"
#include <fstream>
#include <cstdlib>

class BasicTimeWarp;

class BasicState {
  friend ostream &operator<<(ostream &, const BasicState &);
public:
  // these ptrs are put in, because Containers are no longer used.

  BasicState* next; 
  BasicState* prev;

  // The last event in the input queue that effected this state.
  
  BasicEvent* inputPos; // pointer to event in inputQ
  // Excluding the first state, outputPos points to the last event
  // created by the previous simulation cycle (events emitted before
  // this state was saved). *special cases noted in 
  // TimeWarp::cancelMessagesAggressively.
  
  Container<BasicEvent>* outputPos; // pointer to event in outputQ
  bool myLastOutput; // if true, outputPos pnts to last output
                     // event of this state.

  VTime lVT;
  bool dirty;
  
  BasicState();
  virtual ~BasicState(){};
  virtual BasicState& operator=(BasicState&);
  virtual void print(ostream& os);
  virtual void print(ofstream& os);
  
  // The following functions needs to know the size of the object
  // to serialize.
  
  virtual void serialize(ofstream* ckFile, int);
  virtual void deserialize(ifstream* inFile);
  virtual void deserialize(ifstream* inFile, BasicTimeWarp*) {
    deserialize(inFile);
  };
  
  
  // NEW_STATE_MANAGEMENT
  
  // The necessary state functions for the new interface. These functions are
  // overloaded in the user defined states to do the right jobs.
  
  // The copyState copies the contents of the state passed to itself.
  // It returns the pointer to itself .ie. this
  
  virtual void copyState( BasicState * );
  
  virtual int getSize() const;
};

inline
int BasicStateCompare(const BasicState* a, const BasicState* b) {
  if (a->lVT < b->lVT) {
    return -1;
  } else if (a->lVT == b->lVT) {
    return 0;
  }
  return 1;
}

inline int
BasicState::getSize() const {
  return sizeof(BasicState);
}

inline void
BasicState::serialize(ofstream *ckFile, int size) {
  char *charPtr;
  
  *ckFile << size << CHECKPOINT_DELIMITER;
  
  charPtr = (char*) this;
  ckFile->write(charPtr,size);
}

inline void
BasicState::deserialize(ifstream *inFile) {
  char delimiter;
  int size;

  *inFile >> size;
  inFile->get(delimiter);

  if (delimiter != CHECKPOINT_DELIMITER) {
    cerr << " ALIGNMENT ERROR in BasicState::deserialize" << std::endl;
    abort();
  }
  
  char* charPtr = (char*) this;
  
  // Copy in new event;
  inFile->read(charPtr,size);

  inputPos = NULL;
  outputPos = NULL;
#ifndef BUDDYLISTOFEVENTS 
  next = NULL; 
  prev = NULL;
#endif

}

#endif
