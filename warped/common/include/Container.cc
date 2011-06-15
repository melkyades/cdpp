//-*-c++-*-
#ifndef CONTAINERCC
#define CONTAINERCC
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
// $Id: Container.cc,v 1.1 1998/11/30 00:27:53 dmartin Exp $
//
//---------------------------------------------------------------------------
#include "Container.hh"

#ifdef REUSE
class BasicEvent;
class AckRecord;
class BasicTimeWarp;
class BasicState;
class FileData;
class InFileData;
class SigEvent;

#ifdef REUSESTATS
static int BEtotalAlloc = 0;
static int BEtotalRequested = 0;
static int BEtotalReused = 0;
static int BEtotalReleased = 0;

static int BTWtotalAlloc = 0;
static int BTWtotalRequested = 0;
static int BTWtotalReused = 0;
static int BTWtotalReleased = 0;

static int BStotalAlloc = 0;
static int BStotalRequested = 0;
static int BStotalReused = 0;
static int BStotalReleased = 0;

static int ARtotalAlloc = 0;
static int ARtotalRequested = 0;
static int ARtotalReused = 0;
static int ARtotalReleased = 0;

static int FDtotalAlloc = 0;
static int FDtotalRequested = 0;
static int FDtotalReused = 0;

#endif // REUSESTATS

// pointers for all the freepools
Container<BasicEvent>* Container<BasicEvent>::freepool = NULL;
unsigned Container<BasicEvent>::freesize = 0;
Container<AckRecord>* Container<AckRecord>::freepool = NULL;
unsigned Container<AckRecord>::freesize = 0;
Container<BasicTimeWarp>* Container<BasicTimeWarp>::freepool = NULL;
unsigned Container<BasicTimeWarp>::freesize = 0;
Container<BasicState>* Container<BasicState>::freepool = NULL;
unsigned Container<BasicState>::freesize = 0;
Container<FileData>* Container<FileData>::freepool = NULL;
unsigned Container<FileData>::freesize = 0;
Container<InFileData>* Container<InFileData>::freepool = NULL;
unsigned Container<InFileData>::freesize = 0;

static const unsigned containerChunk = 10;


void*
Container<BasicTimeWarp>::operator new(size_t size) {
  register Container<BasicTimeWarp> *newptr;

  if (Container<BasicTimeWarp>::freesize == 0) {
#ifdef REUSESTATS
    BTWtotalAlloc ++;
    BTWtotalRequested++;
#endif
    Container<BasicTimeWarp>::freesize = containerChunk;
    // allocate containerChunk new containers
    size_t chunk = containerChunk * size;
    Container<BasicTimeWarp>::freepool = 
      (Container<BasicTimeWarp>*) new char[chunk];

    // thread new memory together. 
    for (newptr =Container<BasicTimeWarp>:: freepool;
	 newptr != &(Container<BasicTimeWarp>::freepool[containerChunk - 1]);
	 newptr++) {
      newptr->next = newptr + 1;
    }
  }
#ifdef REUSESTATS
  else {
    BTWtotalReused++;
    BTWtotalRequested++;
  }
#endif
  newptr = Container<BasicTimeWarp>::freepool;
  Container<BasicTimeWarp>::freepool = 
    Container<BasicTimeWarp>::freepool->next;
  Container<BasicTimeWarp>::freesize--;
  return newptr;
}


void*
Container<BasicState>::operator new(size_t size) {
  register Container<BasicState> *newptr;

  if (Container<BasicState>::freesize == 0) {
#ifdef REUSESTATS
    BStotalAlloc ++;
    BStotalRequested++;
#endif
    Container<BasicState>::freesize = containerChunk;
    // allocate containerChunk new containers
    size_t chunk = containerChunk * size;
    Container<BasicState>::freepool = (Container<BasicState>*) new char[chunk];

    // thread new memory together. 
    for (newptr = Container<BasicState>::freepool;
	 newptr != &(Container<BasicState>::freepool[containerChunk - 1]);
	 newptr++) {
      newptr->next = newptr + 1; // pointer arithmetic!
    }
  }
#ifdef REUSESTATS
  else {
    BStotalReused++;
    BStotalRequested++;
  }
#endif
  newptr =Container<BasicState>:: freepool;
  Container<BasicState>::freepool = Container<BasicState>::freepool->next;
  Container<BasicState>::freesize--;

  return newptr;
}


void*
Container<BasicEvent>::operator new(size_t size) {
  register Container<BasicEvent> *newptr;

  if (Container<BasicEvent>::freesize == 0) {
#ifdef REUSESTATS
    BEtotalAlloc++;
    BEtotalRequested++;
#endif
    Container<BasicEvent>::freesize = containerChunk;
    // allocate containerChunk new containers
    size_t chunk = containerChunk * size;
    Container<BasicEvent>::freepool = (Container<BasicEvent>*) new char[chunk];

    // thread new memory together.
    for (newptr = Container<BasicEvent>::freepool;
	 newptr != &(Container<BasicEvent>::freepool[containerChunk - 1]); 
	 newptr++) {
      newptr->next = newptr + 1; // pointer arithmetic!
    }
  }
#ifdef REUSESTATS
  else {
    BEtotalReused++;
    BEtotalRequested++;
  }
#endif
  
  newptr = Container<BasicEvent>::freepool;
  Container<BasicEvent>::freepool = Container<BasicEvent>::freepool->next;
  Container<BasicEvent>::freesize--;

  return newptr;
}


void*
Container<AckRecord>::operator new(size_t size) {
  register Container<AckRecord> *newptr;

  if (Container<AckRecord>::freesize == 0) {
#ifdef REUSESTATS
    ARtotalAlloc ++;
    ARtotalRequested++;
#endif
    Container<AckRecord>::freesize = containerChunk;
    // allocate containerChunk new containers
    size_t chunk = containerChunk * size;
    Container<AckRecord>::freepool = (Container<AckRecord>*) new char[chunk];

    // thread new memory together.
    for (newptr = Container<AckRecord>::freepool;
	 newptr != &(Container<AckRecord>::freepool[containerChunk - 1]); 
	 newptr++) {
      newptr->next = newptr + 1;
    }
  }
#ifdef REUSESTATS
  else {
    ARtotalReused++;
    ARtotalRequested++;
  }
#endif
  newptr = Container<AckRecord>::freepool;
  Container<AckRecord>::freepool = Container<AckRecord>::freepool->next;
  Container<AckRecord>::freesize--;
  return newptr;
}


void*
Container<FileData>::operator new(size_t size) {
  register Container<FileData> *newptr;

  if (Container<FileData>::freesize == 0) {
#ifdef REUSESTATS
    FDtotalAlloc ++;
    FDtotalRequested++;
#endif
    Container<FileData>::freesize = containerChunk;
    // allocate containerChunk new containers
    size_t chunk = containerChunk * size;
    Container<FileData>::freepool = (Container<FileData>*) new char[chunk];

    // thread new memory together.
    for (newptr = Container<FileData>::freepool;
	 newptr != &(Container<FileData>::freepool[containerChunk - 1]); 
	 newptr++) {
      newptr->next = newptr + 1;
    }
  }
#ifdef REUSESTATS
  else {
    FDtotalReused++;
    FDtotalRequested++;
  }
#endif
  newptr = Container<FileData>::freepool;
  Container<FileData>::freepool = Container<FileData>::freepool->next;
  Container<FileData>::freesize--;
  return newptr;
}


void
Container<FileData>::operator delete(void* delptr) {
#ifdef REUSESTATS
  ARtotalReleased++;
#endif

  ((Container<FileData>*)delptr)->next = Container<FileData>::freepool;
  Container<FileData>::freepool = (Container <FileData>*)delptr;
  Container<FileData>::freesize++;
}

void*
Container<InFileData>::operator new(size_t size) {
  register Container<InFileData> *newptr;

  if (Container<InFileData>::freesize == 0) {
#ifdef REUSESTATS
    FDtotalAlloc ++;
    FDtotalRequested++;
#endif
    Container<InFileData>::freesize = containerChunk;
    // allocate containerChunk new containers
    size_t chunk = containerChunk * size;
    Container<InFileData>::freepool = (Container<InFileData>*) new char[chunk];

    // thread new memory together.
    for (newptr = Container<InFileData>::freepool;
	 newptr != &(Container<InFileData>::freepool[containerChunk - 1]); 
	 newptr++) {
      newptr->next = newptr + 1;
    }
  }
#ifdef REUSESTATS
  else {
    FDtotalReused++;
    FDtotalRequested++;
  }
#endif
  newptr = Container<InFileData>::freepool;
  Container<InFileData>::freepool = Container<InFileData>::freepool->next;
  Container<InFileData>::freesize--;
  return newptr;
}


void
Container<InFileData>::operator delete(void* delptr) {
#ifdef REUSESTATS
  ARtotalReleased++;
#endif

  ((Container<InFileData>*)delptr)->next = Container<InFileData>::freepool;
  Container<InFileData>::freepool = (Container <InFileData>*)delptr;
  Container<InFileData>::freesize++;
}


void
Container<AckRecord>::operator delete(void* delptr) {
#ifdef REUSESTATS
  ARtotalReleased++;
#endif

  ((Container<AckRecord>*)delptr)->next = Container<AckRecord>::freepool;
  Container<AckRecord>::freepool = (Container <AckRecord>*)delptr;
  Container<AckRecord>::freesize++;
}


void
Container<BasicEvent>::operator delete(void* delptr) {
#ifdef REUSESTATS
  BEtotalReleased++;
#endif

  ((Container<BasicEvent>*)delptr)->next = Container<BasicEvent>::freepool;
  Container<BasicEvent>::freepool = (Container <BasicEvent>*)delptr;
  Container<BasicEvent>::freesize++;
}


void
Container<BasicTimeWarp>::operator delete(void* delptr) {
#ifdef REUSESTATS
  BTWtotalReleased++;
#endif

  ((Container<BasicTimeWarp>*)delptr)->next = 
    Container<BasicTimeWarp>::freepool;
  Container<BasicTimeWarp>::freepool = (Container <BasicTimeWarp>*)delptr;
  Container<BasicTimeWarp>::freesize++;
}


void
Container<BasicState>::operator delete(void* delptr) {
#ifdef REUSESTATS
  BStotalReleased++;
#endif

  ((Container<BasicState>*)delptr)->next = Container<BasicState>::freepool;
  Container<BasicState>:: freepool = (Container <BasicState>*)delptr;
  Container<BasicState>::freesize++;
}


template <class data>
inline void
Container <data>::cleanup() {
  Container <data> *delptr;
  while (freepool) {
    delptr = freepool;
    freepool = freepool->next;
    ::delete delptr;
    freesize = 0;
  }
}


void*
Container<SigEvent>::operator new(size_t size) {
  register Container<FileData> *newptr;

  if (Container<FileData>::freesize == 0) {
#ifdef REUSESTATS
    FDtotalAlloc++;
    FDtotalRequested++;
#endif
    Container<FileData>::freesize = containerChunk;
    // allocate containerChunk new containers
    size_t chunk = containerChunk * size;
    Container<FileData>::freepool = (Container<FileData>*) new char[chunk];

    // thread new memory together.
    for (newptr = Container<FileData>::freepool;
	 newptr != &(Container<FileData>::freepool[containerChunk - 1]); 
	 newptr++) {
      newptr->next = newptr + 1;
    }
  }
#ifdef REUSESTATS
  else {
    FDtotalReused++;
    FDtotalRequested++;
  }
#endif
  newptr = Container<FileData>::freepool;
  Container<FileData>::freepool = Container<FileData>::freepool->next;
  Container<FileData>::freesize--;
  return (Container<SigEvent>*)newptr;
}


void
Container<SigEvent>::operator delete(void* delptr) {
#ifdef REUSESTATS
  ARtotalReleased++;
#endif

  ((Container<FileData>*)delptr)->next = Container<FileData>::freepool;
  Container<FileData>::freepool = (Container <FileData>*)delptr;
  Container<FileData>::freesize++;
}


#ifdef REUSESTATS
void
Container<BasicEvent>::printReuseStats(ostream& os) {
  int mallocCalls = BTWtotalAlloc + BStotalAlloc + ARtotalAlloc
    + BEtotalAlloc + FDtotalAlloc;
  int newCons = BTWtotalRequested + BEtotalRequested + BStotalRequested 
    + ARtotalRequested + FDtotalRequested;
  int reusedCons = BTWtotalReused + BEtotalReused + BStotalReused
    + ARtotalReused + FDtotalReused;
  int mallocBytes = 
    mallocCalls * containerChunk * sizeof(Container<BasicEvent>);
  int newBytes = newCons * sizeof(Container<BasicEvent>);
  int reusedBytes = reusedCons * sizeof(Container<BasicEvent>);
  float reusedPercent = ((float)reusedCons/(float)newCons) * 100;

  os << " Container reuse statistics:\n";
  os << " Total memory actually allocated: " << mallocBytes
     << " bytes in " << mallocCalls << " chunks of " 
     << containerChunk * sizeof(Container<BasicEvent>) << " bytes\n";

  os << " Total memory requested: " << newBytes << " bytes in " << newCons 
     << " chunks of " << sizeof(Container<BasicEvent>) << " bytes\n";

  os << " Total reused memory: " << reusedBytes << " bytes in " << reusedCons
     << " chunks of " << sizeof(Container<BasicEvent>) << " bytes\n";

  os << " Memory reuse = " << reusedPercent<< "%" << endl;
}
#endif // REUSESTATS
#endif // REUSE
#endif // CONTAINER_CC
