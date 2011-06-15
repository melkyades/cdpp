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
// $Id: SequentialBasicState.hh,v 1.1 1998/12/22 20:35:44 dmadhava Exp $
//
//
//---------------------------------------------------------------------------
#include "warped.hh"
#include "BasicEvent.hh"
#include "Container.hh"
#include <fstream.h>
#include <stdlib.h>

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
  
  inline BasicState() {
    next = NULL;
    prev = NULL;
    
    inputPos = NULL;
    outputPos = NULL;
    myLastOutput = true;
  }
  
  virtual ~BasicState(){};
  virtual BasicState& operator=(BasicState&);
  virtual void print(ostream& os);
  
  // The following functions needs to know the size of the object
  // to serialize.
  
  virtual void serialize(ofstream* ckFile, int);
  virtual void deserialize(ifstream* inFile);
  
  
  virtual void copyState( BasicState * );
  virtual int getSize() const;
};

#endif
