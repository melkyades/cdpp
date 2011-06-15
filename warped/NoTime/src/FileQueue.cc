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
// $Id: 
//
//-----------------------------------------------------------------------

#include "FileQueue.hh"

int FileQueue::standardOut = -1;

FileQueue::FileQueue() : outFile(NULL), outFileName(NULL) {
  outFile = new ofstream();

  statusBit = 0;
}


FileQueue::FileQueue(const char* fileName) {
  statusBit = 0;

  outFileName = new char[strlen(fileName)+1];
  strcpy(outFileName, fileName);

  outFile = new ofstream();

  ((ofstream *)outFile)->open(fileName, ios::out);

  if (!outFile->good()) {
    cout << "FileQueue: error opening file " << fileName << endl;
    exit(-41);
  }
}

FileQueue::FileQueue(int): outFileName(NULL) {
  // We instantiate a out file queue for writing output to stdout.  This
  // is done as follows: FileQueue is newed with the constructor having
  // the argument FileQueue::standardOut.  This tells the file queue to
  // output all the data that it recieves to stdout and not to a file.
  // The file id is also accessed as FileQueue::standardOut.
  statusBit = 1;

  outFile = &cout;
  if (!outFile->good()) {
    cout << "FileQueue: error opening standard out " << endl;
    exit(-41);
  }
}

FileQueue::~FileQueue() {
  if (statusBit == 0) {
    delete outFile;
    delete [] outFileName;
  }
}

void
FileQueue::close() {
  if (statusBit == 0) {
    ((ofstream *) outFile)->close();
  }
}

void FileQueue::open(const char* fileName) {
  outFileName = new char[strlen(fileName) + 1];
  strcpy(outFileName, fileName);
  statusBit = 0;

  if (outFile == NULL) {
    outFile   = new ofstream;
  }

  ((ofstream *)outFile)->open(fileName, ios::out);
  
  if (!outFile->good()) {
    cout << "FileQueue: error opening file " << fileName << endl;
    exit(-41);
  }
}

void
FileQueue::insert(FileData* newdata) {
  if ((outFileName == NULL) && (outFile != &cout)) {
    cout << "FileQueue error: writing data to an unopened file!" << endl;
    exit(-41);
  }
  
  *outFile << newdata->line << flush;
  
  delete newdata;
}

#endif
