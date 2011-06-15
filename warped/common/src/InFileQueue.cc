//-*-c++-*-
#ifndef INFILEQUEUE_CC
#define INFILEQUEUE_CC
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
// $Id: InFileQueue.cc,v 1.2 1999/09/22 00:31:23 tmcbraye Exp $
//
//-----------------------------------------------------------------------
#include "InFileQueue.hh"

InFileQueue::InFileQueue() : inFileName(NULL) {
  setFunc( FileDataCompareTime );
}


InFileQueue::InFileQueue(const char* fileName) {

  inFileName = new char[strlen(fileName)+1];
  strcpy(inFileName, fileName);

  setFunc( FileDataCompareTime );

  inFile.open(fileName, ios::in);
  if (!inFile.good()) {
    cout << "InFileQueue: error opening file " << fileName << "\n";
    exit(-41);
  }
}

void 
InFileQueue::Open(const char* fileName) {
  inFileName = new char[strlen(fileName)+1];
  strcpy(inFileName, fileName);

  inFile.open(fileName, ios::in);
  if (!inFile.good()) {
    cout << "InFileQueue: error opening file " << fileName << "\n";
    exit(-41);
  }
}


InFileQueue::~InFileQueue() {
  gcollect(PINFINITY);
  inFile.close();
  delete[] inFileName;
}

void
InFileQueue::close() {
  gcollect(PINFINITY);
  inFile.close();
  // delete[] inFileName;
}

void 
InFileQueue::gcollect(const VTime& gtime){
  InFileData *delptr;

  delptr = seek(0, START);
  while(delptr != NULL && delptr->time < gtime) {
    delptr = removeCurrent();
    delete delptr;
    delptr = get(); // get next element in the queue
  }

  if (delptr == NULL) { 
    currentPos = NULL;
  }
}


void
InFileQueue::rollbackTo(const VTime& rollbackToTime){
  InFileData *delptr;
  
  delptr = seek(0, END);
  while(delptr != NULL && delptr->time >= rollbackToTime) {
#ifdef FILEDBG
    cout << "FileQ: rollbackTo--deleted " << delptr << endl;
#endif
    delptr = removeCurrent(); 
    delete delptr;
    delptr = seek(0, END); // reset current to end of list
  }
  if(delptr == NULL) {
    inFile.seekg(ios::beg);
  }
  else {
    inFile.seekg(delptr->pos);
  }
}

void 
InFileQueue::storePos(const VTime &time, streampos pos) {
  if(back() == NULL ||time != back()->time) {
    InFileData *inPos = new InFileData;

    inPos->pos = pos;
    inPos->time = time;
    
    insert(inPos);
  }
}
    

#endif

