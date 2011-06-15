//-*-c++-*-
#ifndef FORKSTATE_CC
#define FORKSTATE_CC
// Copyright (c) 1994,1995 Ohio Board of Regents and the University of
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
// $Id: ForkState.cc
//
//
//---------------------------------------------------------------------------

#include "ForkState.hh"

ForkState::ForkState() {
  // initialization
  gen        = new MLCG;
  lastOutput = 0;
  localDelay = 0 ;
} 

ForkState::ForkState(const ForkState&) { 
  cerr << "ERROR: Fork Copy Constructor has not been defined" << endl;
}

ForkState::~ForkState() {
  delete gen;
}

ForkState&
ForkState::operator=(ForkState& thisState) {
  (BasicState &)*this = (BasicState &) thisState;
  *(this->gen)        = *thisState.gen;
  this->lastOutput    = thisState.lastOutput;
  this->localDelay    = thisState.localDelay;
  return *this;
}

void
ForkState::copyState(BasicState *rhs) {
  *this = *((ForkState *) rhs);
}


void
ForkState::serialize(ofstream *ckFile, int mysize) {
  unsigned count = 0;
  
  BasicState::serialize(ckFile, sizeof(BasicState));
  *ckFile << sizeof(BasicState) << CHECKPOINT_DELIMITER;

  *ckFile << lastOutput << ' ' << localDelay << ' ';

  char *charPtr;
  count = 0;
  *ckFile << sizeof(MLCG) << CHECKPOINT_DELIMITER;
  
  charPtr = (char*) gen;
  while (count < sizeof(MLCG)) {
    ckFile->put(charPtr[count]);
    count++;
  }
  
  *ckFile << CHECKPOINT_DELIMITER;

}

void
ForkState::deserialize(ifstream* inFile) {
  int mysize, count;
  char delimiter;
  
  BasicState::deserialize(inFile);

  *inFile >> mysize >> delimiter;
  
  if (delimiter != CHECKPOINT_DELIMITER) {
    cerr << "Checkpoint Alignment Error! in ForkState\n";
    abort();
  }

  *inFile >> lastOutput >> localDelay;

  int gensize;

  *inFile >> gensize;
  inFile->get(delimiter);

  if (delimiter != CHECKPOINT_DELIMITER) {
    cerr << " ALIGNMENT ERROR in ForkState" << endl;
    exit(-12);
  }
  
  char* charPtr = (char*) gen;
  
  // Copy in new event;
  count = 0;
  while (count < gensize) {
    inFile->get(charPtr[count]);
    count++;
  }

  
  *inFile >> delimiter;
  if (delimiter != CHECKPOINT_DELIMITER) {
    cerr << "Checkpoint Alignment Error! in ForkState\n";
    abort();
  }
}

int
ForkState::getSize() const {
  return sizeof(ForkState);
}

#endif
