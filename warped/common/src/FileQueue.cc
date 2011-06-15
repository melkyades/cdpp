//-*-c++-*-
#ifndef FILEQUEUE_CC
#define FILEQUEUE_CC
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
// $Id: FileQueue.cc,v 1.2 1999/09/22 00:31:22 tmcbraye Exp $
//
//-----------------------------------------------------------------------
#include "FileQueue.hh"

int FileQueue::standardOut = -1;

FileQueue::FileQueue() : outFile(NULL), outFileName(NULL) {
  setFunc( FileDataCompareTime );
  outFile = new ofstream();
  statusBit = 0;
}


FileQueue::FileQueue(const char* fileName) {
  statusBit = 0;
  setFunc( FileDataCompareTime );
  outFileName = new char[strlen(fileName)+1];
  strcpy(outFileName, fileName);

  outFile = new ofstream();

  ((ofstream *)outFile)->open(fileName, ios::out);
  if (!outFile->good()) {
    cout << "FileQueue: error opening file " << fileName << "\n";
    exit(-41);
  }
}

FileQueue::FileQueue(int ): outFileName(NULL) {
  // We instantiate a out file queue for writing output to stdout.  This
  // is done as follows: FileQueue is newed with the constructor having
  // the argument FileQueue::standardOut.  This tells the file queue to
  // output all the data that it recieves to stdout and not to a file.
  // The file id is also accessed as FileQueue::standardOut.
  statusBit = 1;
  setFunc( FileDataCompareTime );

  outFile = &cout;
  if (!outFile->good()) {
    cout << "FileQueue: error opening standard out " << "\n";
    exit(-41);
  }
}


FileQueue::~FileQueue() {
  gcollect(PINFINITY);
  if(statusBit == 0){
//     outFile->close();
    delete outFile;
    delete [] outFileName;
  }
}

void
FileQueue::close() {
  gcollect(PINFINITY);

  if (statusBit == 0) {
    ((ofstream *) outFile)->close();
    // delete outFile;
    // delete [] outFileName;
  }
}

void
FileQueue::open(const char* fileName) {
  outFileName = new char[strlen(fileName)+1];
  strcpy(outFileName, fileName);
  statusBit = 0;
  outFile = new ofstream;
  ((ofstream *)outFile)->open(fileName, ios::out);
  if (!outFile->good()) {
    cout << "FileQueue: error opening file " << fileName << "\n";
    exit(-41);
  }
}

void
FileQueue::open(const char *fileName, ios::open_mode _mode) {
  outFileName = new char[strlen(fileName) + 1];
  strcpy(outFileName, fileName);
  statusBit = 0;
  outFile = new ofstream;
  
  ((ofstream *) outFile)->open(fileName, _mode);
  
  if (!outFile->good()) {
    cout << "FileQueue: error opening file " << fileName << "\n";
    exit(-41);
  }
}

void
FileQueue::insert(FileData* newdata) {
  if (outFileName == NULL && outFile != &cout) {
    cout << "FileQueue error: writing data to an unopened file!" << "\n";
    exit(-41);
  }

#ifdef SEQUENTIAL
  // We don't need to insert and stuff in sequential. Just write it straight
  // to the file and get out.
  if (statusBit == 0) {
    outFile->seekp(0, ios::end);
  }
  *outFile << newdata->line << flush;
  delete newdata;
#else
  register Container< FileData >* newelem = NULL;
  newelem = new Container<FileData>;
  newelem->object = newdata;
  // Initialization necessary here since it is removed from Container()
  newelem->next = NULL;
  newelem->prev = NULL;

    // Mainlist update
  if (listsize == 0) {
    // empty list
    head = newelem;
    head->prev = NULL;
    tail = newelem;
    tail->next = NULL;
  }
  else {
    // insertPos can not be NULL because at least one element exists
    if(compare(newelem->object, insertPos->object) >= 0) {
      // after insertPos
      
      // two cases to consider here
      //[a] newelem has to be inserted after insertPos as compare
      //    returned value > 0
      //[b] compare returned 0. In this case, we have to search
      //    for the first element with the same id and time
      
      while(insertPos != NULL &&
	    compare(newelem->object, insertPos->object) >= 0) {
	insertPos = insertPos->next; 
      }
      if(insertPos==NULL){
	// tail reached
	newelem->prev = tail;
	tail->next = newelem;
	tail = newelem;
	newelem->next = NULL;
      }
    }
    else {
      // before insertPos
      while(insertPos != NULL &&
	    compare(newelem->object, insertPos->object) < 0) {
	insertPos = insertPos->prev; 
      }
      if(insertPos == NULL){
	// head reached
	newelem->next = head;
	head->prev = newelem;
	head = newelem;
	newelem->prev = NULL;
      }
      else {
	insertPos = insertPos->next;
      }
    }
  }
  // insertPos is pointing to NULL if tail or head
  // and otherwise it is pointing to the element behind insertion point
  if( insertPos != NULL ){
    newelem->next = insertPos;
    newelem->prev = insertPos->prev;
    insertPos->prev->next = newelem;
    insertPos->prev = newelem;
  }
  
  insertPos = newelem;
  listsize++;
#endif  
}


void 
FileQueue::gcollect(const VTime& gtime){
  FileData *delptr;

  delptr = seek(0, START);
  while(delptr != NULL && delptr->time < gtime) {
    delptr = removeCurrent();
    outFile->seekp(0, ios::end);
    *outFile << delptr->line << flush;
    delete delptr;
    delptr = get(); // get next element in the queue
  }

  if (delptr == NULL) { 
    currentPos = NULL;
  }
}


void
FileQueue::rollbackTo(const VTime& rollbackToTime){
  FileData *delptr;
  
  delptr = seek(0, END);
  while(delptr != NULL && delptr->time >= rollbackToTime) {
#ifdef FILEDBG
    cout << "FileQ: rollbackTo--deleted " << delptr << endl;
#endif
    delptr = removeCurrent(); 
    delete delptr;
    delptr = seek(0, END); // reset current to end of list
  }
}
#endif
