// -*-c++-*-
#ifndef STATEMANAGER_HH
#define STATEMANAGER_HH
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
// $Id: StateManager.hh,v 1.8 1999/10/24 23:26:19 ramanan Exp $
//
//---------------------------------------------------------------------------

#include <iostream.h>
#include <fstream>
#include "warped.hh"
#include "config.hh"

#include "StateQueue.hh"

class BasicTimeWarp;

class StateManager {
public:
  // The stuff needed for the new style of state management.
  // In the new style, the templates have been ripped off and all the state
  // saving should go via the process to which this sate manager is associated
  // with. These routines will be exercised by TimeWarp and hece these routines
  // should be transparent to the user
  
  // This is going to be the new interface to the state manager. When the
  // state manager needs a new state it is going to ask the process to allocate
  // a new state. Do do that it needs the pointer to the process. 
  
  StateManager(BasicTimeWarp*);
  
  // This function provides the start for the new interface.
  // Quote !! : When you need something don't take it. Ask for it!!
  
  inline BasicState* getCurrentState() const {
    return (BasicState *) current;
  }

  // This function is needed for the new style of state management
  // This function will be called by timeWarpInit() to allocate the
  // initial state for the process in the system.

  virtual void createInitialState();
  
  virtual ~StateManager();
  int queueSize();
  VTime timeDiff() { return stateQ.timeDiff(); };
  virtual int getStatePeriod();
  virtual void saveState();              // save current onto the queue
  virtual VTime restoreState(VTime);     // restore the state at this time
  // garbage collect any states before this time
  virtual VTime gcollect(VTime);
  virtual VTime gcollect(VTime, BasicEvent*&, Container<BasicEvent>*&);
  void clear(); // Do not remove : error check function
  VTime timeOfStateFromCurrent(int);
  int statesRolledBack;
  virtual bool useTimeForGcollect();
  
  // the follwoing bool is to store myLastOutput of the state at the 
  // head of the statequeue.
  bool returnMyLastOutput ;
  
  int maxQsize;

  inline bool getHeadMyLastOutput(){
    return returnMyLastOutput ;
  }
  
  // clears state pointers
  void clearHead();
  // Set state pointers
  void setTail(Container<BasicEvent>* outpos, bool lastOutput);
  
#ifdef STATEDEBUG
  //Do not remove - Vital error checking function
  void checkEvent(Container<BasicEvent> *checkEvent, int flag);
#endif
  
  virtual void startStateTiming() {};
  virtual void stopStateTiming() {};
  virtual double getStateSavingTime() { return -1.0; };
  
  virtual void startRollbackTiming() {};
  virtual void finishRollbackTiming() {};
  
  virtual void startEventTiming() {};
  virtual bool stopEventTiming() { return false; };
  virtual void pauseEventTiming() {};
  virtual void resumeEventTiming() {};
  virtual double getEventExecutionTime() { return -1.0; };
  
  virtual void committedEvents(long, int ) {};
  virtual void calculatePeriod() {};
  virtual void rollbackAtEvent( SequenceCounter ) {};
  virtual void coastedForwardEvents( int ) {};
  virtual void coastForwardTiming(double) {};
  virtual double getCoastForwardTime() {return -1.0;};
  virtual void setAdaptiveParameters( long, long ) {};

  char *nameOfObject;		// Name of the Object
  
  void printQ(){stateQ.print();};
  
  ofstream *lpFile;
  void setFile(std::ofstream*);
  
#ifdef STATS
  ofstream *statsFile;
  void setStatsFile (std::ofstream *statsfile) { statsFile = statsfile;};
#endif

  BasicState*    current;	// the object's current state
  
  int memoryUseage;
#ifdef CHECKASSERTIONS
  void checkRestorationAssertions() {
    assert(stateQ.listsize == 1 && stateQ.head == stateQ.tail && 
	   stateQ.head->next == stateQ.tail->prev && 
	   stateQ.tail->prev == NULL);
  };

  BasicState* getTail() { return stateQ.getTail(); };
  BasicState* getHead() { return stateQ.getHead(); };
#endif

  virtual VTime findMinimumState(const VTime &);

  StateQueue* getStateQueue() {
    return &stateQ;
  }
  
protected:
  StateQueue stateQ;  
  // Some of the access methods that should be useful. For now they Bitch!!
  
  inline BasicTimeWarp* getProcessPointer() const {
    return processPointer;
  };

  // The process pointer needed by the new style of state management.
  

  BasicTimeWarp* processPointer;
  
private:
  void removeStatesAfterCurrent();
};


#endif
