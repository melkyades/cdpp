//-*-c++-*-
#ifndef RAIDPROCESSSTATE_CC
#define RAIDPROCESSSTATE_CC
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
// $Id: RAIDProcessState.cc
//
//
//
//---------------------------------------------------------------------------

#include "RAIDProcessState.hh"

RAIDProcessState::RAIDProcessState() {
  gen                  = new MLCG;
  diskOperationPending = false;
  stopProcessing       = false;
  size                 = 0;
  paritySize           = -1;
  numRequests          = 0;
  numReads             = 0;
  numWrites            = 0;
  read                 = false;
  beginningOfStrype    = -1;
  strypeSize           = -1;
  numDisks             = 0;
  diskRequests         = NULL;
  firstDiskId          = 0;
}

RAIDProcessState::~RAIDProcessState() {
  delete gen;
  delete [] diskRequests;
}

RAIDProcessState::RAIDProcessState( const RAIDProcessState& ) {
  cerr << "RAIDProcessState copy constructor has not been implemented" 
       << endl;
}

RAIDProcessState&
RAIDProcessState::operator=(RAIDProcessState& thisState){
  (BasicState &)*this = (BasicState &) thisState;
  *(this->gen) = *thisState.gen;
  this->numRequests = thisState.numRequests;
  this->diskOperationPending = thisState.diskOperationPending;
  this->read = thisState.read;
  this->stopProcessing = thisState.stopProcessing;
  this->size = thisState.size;
  this->paritySize = thisState.paritySize;
  this->numReads = thisState.numReads;
  this->numWrites = thisState.numWrites;
  this->beginningOfStrype = thisState.beginningOfStrype;
  this->strypeSize = thisState.strypeSize;
  this->numDisks = thisState.numDisks;
  if (this->diskRequests != NULL) {
    delete [] this->diskRequests;
  }
  this->diskRequests = new int[thisState.numDisks];
  int i;
  for (i = 0; i < thisState.numDisks; i++) {
    this->diskRequests[i] = thisState.diskRequests[i];
  }
  this->firstDiskId = thisState.firstDiskId;
  return *this;
}

void
RAIDProcessState::serialize(ofstream *ckFile, int) {
  unsigned count = 0;
  
  BasicState::serialize(ckFile, sizeof(BasicState));
  *ckFile << sizeof(BasicState) << CHECKPOINT_DELIMITER;
  
  *ckFile << numRequests << ' ' << diskOperationPending << ' '
	  << read << ' ' << stopProcessing << ' '
	  << size << ' ' << paritySize << ' '
	  << numReads << ' ' << numWrites << ' '
	  << beginningOfStrype << ' ' << strypeSize << ' '
	  << numDisks << ' ' << firstDiskId << ' ';
  
  for (count = 0; count < numDisks; count++) {
    *ckFile << diskRequests[count] << ' ';
  }

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

inline void
RAIDProcessState::deserialize(ifstream* inFile) {
  int mysize, count;
  char delimiter;
  
  BasicState::deserialize(inFile);

  *inFile >> mysize >> delimiter;
  
  if (delimiter != CHECKPOINT_DELIMITER) {
    cerr << "Checkpoint Alignment Error!\n";
    abort();
  }
  int tmpDiskPend, tmpRead, tmpProcessing = 0;
  
  *inFile >> numRequests  >> tmpDiskPend
	  >> tmpRead  >> tmpProcessing
	  >> size >> paritySize
	  >> numReads >> numWrites >> beginningOfStrype
	  >> strypeSize >> numDisks >> firstDiskId;

  diskOperationPending = INT_TO_BOOL(tmpDiskPend);
  read = INT_TO_BOOL(tmpRead);
  stopProcessing = INT_TO_BOOL(tmpProcessing);


  inputPos = NULL;
  outputPos = NULL;
#ifndef BUDDYLISTOFEVENTS 
  next = NULL; 
  prev = NULL;
#endif

  for (count = 0; count < numDisks; count++) {
    *inFile >> diskRequests[count];

  }

  int gensize;

  *inFile >> gensize;
  inFile->get(delimiter);

  if (delimiter != CHECKPOINT_DELIMITER) {
    cerr << " ALIGNMENT ERROR in StateManager::restoreCheckpoint" << endl;
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


ostream&
operator<< (ostream& os, const RAIDProcessState& rps) {
  
  os << "numRequests: " << rps.numRequests << "\t\t size = "<< rps.size
     << "\nnumReads: " << rps.numReads <<"\t\t parity size = "<< rps.paritySize
     << "\nnumWrites: " << rps.numWrites 
     << "\nstopped processing [";
  if (rps.stopProcessing == true) {
    os << "T]";
  }
  else {
    os << "F]";
  }
  os << "\t\tLastStrypeRequest = [" << rps.beginningOfStrype
     << "] of size = " << rps.strypeSize;
  if (rps.read) {
    os << " READ\n";
  }
  else {
    os << " WRITE\n";
  }
  int i;
  for (i = 0; i < rps.numDisks; i++) {
    os << i + rps.firstDiskId << ": " << rps.diskRequests[i] << "\t\t";
    if (i%4 == 0 && i !=0 ) os << "\n";
  }
  os << "\n";
  
  os << (BasicState& ) rps;
  
  return( os );
}

inline void 
RAIDProcessState::print(ostream& os) {
  os << *this;
}

inline void 
RAIDProcessState::print(ofstream& os) {
  int count = 0;
  os << "*"  << CHECKPOINT_DELIMITER;
  os << numRequests << ' ' << diskOperationPending << ' '
      << read << ' ' << stopProcessing << ' '
      << size << ' ' << paritySize << ' '
      << numReads << ' ' << numWrites << ' '
      << beginningOfStrype << ' ' << strypeSize << ' '
      << numDisks << ' ' << firstDiskId << ' ';
  os << "\nDisks requests\n";
  for (count = 0; count < numDisks; count++) {
    os << diskRequests[count] << ' ';
  }
  os << "\n";
  os << gen->seed1() << ' ' << gen->seed2();
  os << "\n\n";


}
void
RAIDProcessState::copyState(BasicState *rhs) {
  *this = *((RAIDProcessState *) rhs);
}

int
RAIDProcessState::getSize() const {
  return sizeof(RAIDProcessState);
}

#endif
