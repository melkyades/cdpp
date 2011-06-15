//-*-c++-*-
#ifndef TIMEWARP_HH
#define TIMEWARP_HH

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
//
// $Id: TimeWarp.hh,v 1.16 1999/10/24 23:26:24 ramanan Exp $
//
//---------------------------------------------------------------------------
#include <new.h>
#include "BasicTimeWarp.hh"
#include "DefaultVTime.hh"
#include "FileQueue.hh"
#include "InFileQueue.hh"
#include "KernelMsgs.hh"
#include "Stopwatch.hh"

#include "LogicalProcess.hh" // needed for MESSAGE_AGGREGATION, 
                             // ONE_ANTI_MESSAGE, and for stats collection

//#include "SortedList.hh" // Needed for Lazy Cancellation

class BasicEvent;

class TimeWarp : public BasicTimeWarp {
friend class MatternGVTManager;
friend class GVTManager;
public:
  TimeWarp();
  virtual ~TimeWarp();
  
  void executeSimulation();
  void simulate();
  void saveState();  // Save our current state.

  int inputGcollect(VTime, BasicEvent *ptr = NULL);
  virtual VTime stateGcollect(VTime){ return ZERO; }
  virtual VTime stateGcollect(VTime, BasicEvent*&,
			      Container<BasicEvent>*&);
  void stateClear();
  void clearInitState();
  void outputGcollect(VTime, Container<BasicEvent>*&);
  
  void outputGcollect(VTime);   // Dummy function. Don't call this one!!!

  virtual BasicEvent *getEvent();       // gets an event from the input queue

  virtual void sendEvent(BasicEvent *); // put an event on the output queue
  void recvEvent(BasicEvent *); // how to handle incoming messages
  
  BasicEvent* findPositionFromState(); // get outputQ position from the state
  
  // this calls the application to execute its code for one simulation cycle
  void executeProcess() = 0;

  // the application overrides these two functions, if desired, to
  // contain code to be executed before and after simulation starts
  // and ends.
  
  virtual void initialize() {};
  virtual void finalize() {};
  void finalGarbageCollectOutputQueue();
  
  void finalGarbageCollect();
  
  // this method is for the LP to query whether or not we are within
  // our time window...
  bool withinTimeWindow();
  
  // this method allows our time window to be set.
  void setTimeWindow(VTime newWindow);
  void setLPHandle(LogicalProcess* lpHandle);

  // this method returns the time of the earliest unexecuted event.
  VTime calculateMin(); 
  
  // returns this object's current simulation time
  inline VTime getLVT() const { return state->current->lVT; }
  
  void rollbackFileQueues(VTime time);

#if defined(OBJECTDEBUG) && !defined(LPDEBUG)
  void openFile() {
    BasicTimeWarp::openFile();
    lpFile = &outFile;
  };
#endif
  
  void setFile(ofstream *outfile);

#ifdef STATEDEBUG
  void setStateFile(ofstream *outfile);
#endif

#ifdef DC_DEBUG
  ofstream dcFile;
  void openDCFile(char* dcFilename);
#endif
  
  // data structures for file access
  int numOutFiles;
  int numInFiles;

  // We instantiate a out file queue for writing output to stdout.  This
  // is done as follows: FileQueue is newed with the constructor having
  // the argument FileQueue::standardOut.  This tells the file queue to
  // output all the data that it recieves to stdout and not to a file.
  // The file id is also accessed as FileQueue::standardOut.

  FileQueue stdoutFileQ;
  FileQueue   *outFileQ;
  InFileQueue *inFileQ;
  
  enum MSGSUPPRESSION { NONE,          // No Message Supression in effect.
			LAZYCANCEL,    // Lazy Cancellation Suppression.
			COASTFORWARD,  // Infrequent State Saving Suppression
			LAZYAGGRCANCEL // For dynamic cancellation
  };
  
#if defined(ONE_ANTI_MESSAGE)
  BasicEvent **alreadySentAntiMessage;
#endif


  LogicalProcess* getLPHandle() const {
    return lpHandle;
  }
  
  void terminateSimulation(char *msg);
  
#if defined(LAZYCANCELLATION) || defined(LAZYAGGR_CANCELLATION)
  VTime getLazyQMinTime();
  int getLazyQSize();
#endif

#ifdef STATS

  void setStatsFile (ofstream *statsfile) {
    fileHandle = statsfile;
    state->setStatsFile(fileHandle);
  };

#endif  

protected:
  // rollback to the time specified
  virtual void rollback(VTime);

  // Flag to determine when to suppress a message
  int suppressMessage; 
  
  virtual bool suppressLazyMessages(BasicEvent*);
  
#ifdef LAZYAGGR_CANCELLATION
  virtual bool suppressMessageTest(BasicEvent*) { return false ;} ;
#endif
  
#if defined(LAZYCANCELLATION) || defined(LAZYAGGR_CANCELLATION)

  // Queue used for lazy cancellation.
  OutputQueue lazyCancelQ;
  
  // Should we dispatch a message during lazy cancellation or not? 
  // basically was the comparision a "hit" or a "miss"
  bool lazyCancel(BasicEvent*);

  // Invoked during lazy cancellation
  void moveMessagesToLazyQueue(const VTime);

#endif

  // Same as sendEvent(), but event is sent out without performing any checks.
  void sendEventUnconditionally(BasicEvent*);

private:
  Stopwatch stopWatch;
  // this defines how far this object can execute into the future.
  VTime timeWindow;
  
  // increments each time we send an event to produce a unique id for 
  // events (the id, eventID tuple is unique)
  SequenceCounter eventCounter;
  
  // handle to this object's LP, to access terminateSimulation
  LogicalProcess* lpHandle;
  
  // for statistical use only
  int rollbackCount;

  bool timingsNeeded;
  
  void coastForward(VTime );
  
  // Invoked during aggressive cancellation
  void cancelMessagesAggressively(const VTime);

  // Used only during lazy cancellation
  void fillEventInfo(BasicEvent* );
  
  void cancelMessages(const VTime) {};
  
  const int MAXDIFF;

#ifdef STATS
  SIGN lastMessageSign;
#endif
  
  // NEW_STATE_MANAGEMENT
  
public:
  // The necessary functions for the new style of state management.
  // In the new style, the templates will be rippled oof and all the
  // state saving goes via the process. These functions let the user do this

  virtual void deAllocateState(BasicState*);
  
  // In the new style of state management, state->current will be protected.
  // and will not be available for access everywhere. If you need the current
  // state you have to use this method to get it and then you will have to
  // type cast it to the right state.

  virtual STATE_MANAGER* getStateManager() const {
    return state;
  }
  
  inline BasicState* getCurrentState() const {
    return getStateManager()->getCurrentState();
  }
  
private:
  // This method is the one that is going to be called by logical process
  // to initialize the simulation objects henceforth.
  
  void timeWarpInit();

};

#endif
