//-*-c++-*-
#ifndef MEMSOURCESTATE_CC
#define MEMSOURCESTATE_CC

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
// $Id: MemSourceState.cc,v 1.3 1998/11/30 21:21:56 dmartin Exp $
//
//---------------------------------------------------------------------------

#include "MemSourceState.hh"

MemSourceState::MemSourceState() : filter (0.5, 0.5){
  gen            = new MLCG;
  numMemRequests = 0;
  numTokens      = 0;
  maxTokens      = 5;
  oldDelay       = 0;
  localdelay     = 0 ; 
};

MemSourceState::~MemSourceState(){
  delete gen;
}

void
MemSourceState::serialize(ofstream* ckFile, int mysize) {
  
  BasicState::serialize(ckFile,sizeof(BasicState));
  
  *ckFile << mysize << CHECKPOINT_DELIMITER << ' ' << maxMemRequests
	  << ' ' << numMemRequests << ' ' << oldDelay << ' '
	  << localdelay << ' ' << filter.get_old_weight() << ' '
	  << filter.get_new_weight() << ' ' << filter.getAvg() << ' ';
  
  char *charPtr;
  unsigned count = 0;
  *ckFile << sizeof(MLCG) << CHECKPOINT_DELIMITER;
  
  charPtr = (char*) gen;
  while (count < sizeof(MLCG)) {
    ckFile->put(charPtr[count]);
    count++;
  }

  *ckFile << CHECKPOINT_DELIMITER;
}

void
MemSourceState::deserialize(ifstream* inFile)  {
  int mysize;
  char delimiter;
  float old_weight, new_weight;
  double avg;
  
  BasicState::deserialize(inFile);
  
  *inFile >> mysize >> delimiter;
  
  if (delimiter != CHECKPOINT_DELIMITER) {
    cerr << "Checkpoint Alignment Error! in MemSourceState\n";
    abort();
  }
  
  *inFile >> maxMemRequests >> numMemRequests >> oldDelay
	  >> localdelay >> old_weight
	  >> new_weight >> avg;
  
  filter.set_weights_and_avg(old_weight,new_weight,avg);
  
  int gensize;

  *inFile >> gensize;
  inFile->get(delimiter);

  if (delimiter != CHECKPOINT_DELIMITER) {
    cerr << " ALIGNMENT ERROR in MemSourceState::restoreCheckpoint" << endl;
    exit(-12);
  }
  
  char* charPtr = (char*) gen;
  
  // Copy in new event;
  int count = 0;
  while (count < gensize) {
    inFile->get(charPtr[count]);
    count++;
  }

  *inFile >> delimiter;
  if (delimiter != CHECKPOINT_DELIMITER) {
    cerr << "Checkpoint Alignment Error! in MemSourceState\n";
    abort();
  }
}

MemSourceState&
MemSourceState::operator=(MemSourceState& thisState) {
  (BasicState &)*this  = (BasicState &) thisState;
  *(this->gen)         = *thisState.gen;
  this->maxMemRequests = thisState.maxMemRequests;
  this->numMemRequests = thisState.numMemRequests;
  this->oldDelay       = thisState.oldDelay;
  this->localdelay     = thisState.localdelay;
  
  return *this;
}

int
MemSourceState::getSize() const {
  return sizeof(MemSourceState);
}

void
MemSourceState::copyState(BasicState *rhs) {
  *this = *((MemSourceState *) rhs);
}

#endif
