// -*-c++-*-
#ifndef SEQUENTIALOBJ_HH
#define SEQUENTIALOBJ_HH

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
// $Id: SequentialObj.hh,v 1.2 1999/10/28 04:55:47 mal Exp $
//
//---------------------------------------------------------------------------

#include "DefaultVTime.hh"
#include "SimulationTime.hh"
#include "BaseSequential.hh"
#include "BasicEvent.hh"
#include "SortedList.hh"
#include "SequentialObjectRecord.hh"
#include "FileQueue.hh"
#include "InFileQueue.hh"
#include "BasicState.hh"
#include "warped.hh"
#include "SequentialInputQueue.hh"

#include <new>

class BasicEvent;

#define getLVT() (state->current->lVT)

class SequentialObj : public BaseSequential {
public:
  
  SequentialObj();
  SequentialObj(int); // id
  virtual ~SequentialObj();
  
  void simulate();
  BasicEvent *getEvent();       // gets an event from the input queue
  
  void sendEvent(BasicEvent *); // put an event on the output queue
  
  // this calls the application to execute its code for one simulation cycle
  virtual void executeProcess() = 0;
  
  // the user overloads these two functions, if desired, to contain code
  // to be executed before and after simulation starts and ends.
  virtual void initialize() {};
  virtual void finalize() {};
#ifdef LPDEBUG
  void setFile(std::ofstream *outFile){
    lpFile = outFile ; // lpfile is declared in BaseSequential
  }
#endif
  
  struct SequentialState {
    BasicState *current;
    char *nameOfObject;
    
    ~SequentialState() {};
  } *state;

  inline void recvEvent(BasicEvent* newEvent) {
    inputQ.insert(newEvent);
  }
  
  int numOutFiles;
  int numInFiles;
  FileQueue   *outFileQ;
  InFileQueue *inFileQ;
  FileQueue   stdoutFileQ;
  
  virtual void deAllocateState(BasicState *);
  
private:
  long long eventCounter; // increments each time we send an event
  
  // This method is the one that is going to be called by logical process
  // to initialize the simulation objects henceforth.
  
  void timeWarpInit();
  
public:
  virtual void outputGcollect(VTime) ;
  virtual int inputGcollect(VTime) ;
  
  BasicState* getCurrentState() const ;
  
  SequentialInputQueue outputQ;
  
  long long eventCount  ;
};

#endif








