//-*-c++-*-
#ifndef BASIC_STATE_CC
#define BASIC_STATE_CC
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
// $Id: SequentialBasicState.cc,v 1.1 1998/12/22 20:35:44 dmadhava Exp $
//
//
//---------------------------------------------------------------------------

#include "BasicState.hh"

using namespace std;

BasicState& 
BasicState::operator=(BasicState& bs) {
  lVT          = bs.lVT;
  dirty        = bs.dirty;
  outputPos    = bs.outputPos;
  inputPos     = bs.inputPos;
  myLastOutput = bs.myLastOutput;
  
  return *this;
}

// NEW_STATE_MANAGEMENT
void
BasicState::copyState(BasicState *sourceState) {
  lVT          = sourceState->lVT;
  dirty        = sourceState->dirty;
  outputPos    = sourceState->outputPos;
  inputPos     = sourceState->inputPos;
  myLastOutput = sourceState->myLastOutput;
}

int
BasicState::getSize() const {
  return sizeof(BasicState);
}

void
BasicState::serialize(ofstream *ckFile, int size) {
  char *charPtr;
  int count = 0;
  
  *ckFile << size << CHECKPOINT_DELIMITER;
  
  charPtr = (char*) this;
  while (count < size) {
    ckFile->put(charPtr[count]);
    count++;
  }
}

void
BasicState::deserialize(ifstream *inFile) {
  char delimiter;
  int size, count;

  *inFile >> size;
  inFile->get(delimiter);

  if (delimiter != CHECKPOINT_DELIMITER) {
    cerr << " ALIGNMENT ERROR in StateManager::restoreCheckpoint" << endl;
    exit(-12);
  }
  
  char* charPtr = (char*) this;
  
  // Copy in new event;
  count = 0;
  while (count < size) {
    inFile->get(charPtr[count]);
    count++;
  }
}

ostream&
operator<<(ostream &os, const BasicState &e) {
  os << "lVT:" << e.lVT << " outputPos: " << e.outputPos 
     << " inputPos: " << e.inputPos << " flagBit: " << e.myLastOutput;
  
  return os;
}

void
BasicState::print( ostream& os ) {
  os << *this;
}

#endif
