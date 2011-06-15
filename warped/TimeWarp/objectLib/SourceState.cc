//-*-c++-*-
#ifndef SOURCESTATE_CC
#define SOURCESTATE_CC

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
// $Id: SourceState.cc,v 1.3 1998/11/30 21:22:03 dmartin Exp $
//
//---------------------------------------------------------------------------

#include "SourceState.hh"

inline
SourceState& SourceState::operator=(SourceState& thisState){
  (BasicState &)*this = (BasicState &) thisState;
  *(this->gen) = *thisState.gen;
  this->maxTokens = thisState.maxTokens;
  this->numTokens = thisState.numTokens;
  this->numRedTokens = thisState.numRedTokens;
  this->oldDelay = thisState.oldDelay;
  this->localdelay = thisState.localdelay;
  return *this;
}

inline
SourceState::SourceState() {
  gen = new MLCG;
  numTokens = 0;
  maxTokens = 5;
  oldDelay = 0;
  localdelay = 0 ; 
}

void
SourceState::copyState(BasicState *rhs) {
  *this = *((SourceState *) rhs);
}

int
SourceState::getSize() const {
  return sizeof(SourceState);
}

void
SourceState::serialize(ofstream *ckFile, int mysize) {
  unsigned count = 0;
  
  BasicState::serialize(ckFile, sizeof(BasicState));
  *ckFile << sizeof(BasicState) << CHECKPOINT_DELIMITER;

  *ckFile << maxTokens << ' ' << numTokens << ' ' << numRedTokens
	  << oldDelay << ' ' << localdelay << ' ';

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
SourceState::deserialize(ifstream* inFile) {
  int mysize, count;
  char delimiter;
  
  BasicState::deserialize(inFile);

  *inFile >> mysize >> delimiter;
  
  if (delimiter != CHECKPOINT_DELIMITER) {
    cerr << "Checkpoint Alignment Error in SourceState!\n";
    abort();
  }

  *inFile >> maxTokens >> numTokens >> numRedTokens
	  >> oldDelay >> localdelay;

  int gensize;

  *inFile >> gensize;
  inFile->get(delimiter);

  if (delimiter != CHECKPOINT_DELIMITER) {
    cerr << " ALIGNMENT ERROR in SourceState::restoreCheckpoint" << endl;
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
    cerr << "Checkpoint Alignment Error!\n";
    abort();
  }
}
#endif
