//-*-c++-*-
#ifndef TIMEWARP_HANDLE_HH
#define TIMEWARP_HANDLE_HH
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
// $Id: BasicTimeWarp.hh,v 1.14 1999/10/25 20:00:38 ramanan Exp $
//
//---------------------------------------------------------------------------

#include "config.hh"
#include "LTSFInputQueue.hh"
#include "BasicEvent.hh"
#include "OutputQueue.hh"
#include "ObjectRecord.hh"
#include "KernelMsgs.hh"
#include <fstream>
#include <cstdlib>
#include "StateManagerInclude.hh"

/// forward declaration of the LTSFScheduler class
class LTSFScheduler;

/** The BasicTimeWarp Base Class.
    Base class for all simulation objects:
    \begin{itemize}
     \item defines the object interface that the kernel & the application see.
     \item cannot be instantiated as it contains several virtual functions.
     \item contains the static input queue object, the output queue object, 
           and the state manager.
    \end{itemize}
*/
class BasicTimeWarp {
  friend class GVTManager;
  friend class MatternGVTManager;
  friend class LogicalProcess;
  friend ostream& operator<< (ostream&, const BasicTimeWarp&);  

public:

  /**@name Public Class Methods */
  //@{

  /// Constructor
  BasicTimeWarp();

  /// Destructor
  virtual ~BasicTimeWarp() {}  

  /// gets an event from the input queue
  virtual BasicEvent* getEvent() = 0;

  /// put an event on the output queue (and send out event)
  virtual void sendEvent(BasicEvent*) = 0;
  
  /// send a negative event
  virtual void sendNegEvent(BasicEvent*);
  
  /// identify whether or not this object is local to this LP 
  virtual inline bool isCommManager(){
    return false;
  }
  
  /// this function will be executed before simulation starts
  virtual void initialize() {}
  /// this function will be executed after simulation ends
  virtual void finalize() {}

  /// calls the last fossil collection cycle before simulation termination  
  virtual void finalGarbageCollect() {}
  virtual void finalGarbageCollectOutputQueue() {}
  
  /// call to application to execute its code for one simulation cycle
  virtual void executeProcess() = 0;  

  /// called by Communication Manager(or by sender) to deliver event 
  virtual void recvEvent(BasicEvent*) = 0;

  /// method to set the communication handle in this object, so it knows where to find the other simulation objects on this LP
  void setCommHandle(ObjectRecord *tempHandle);

  /// set the handle to the LogicalProcess
  virtual void setLPHandle(LogicalProcess* );

  /// retrieve the handle to the LogicalProcess
  virtual LogicalProcess* getLPHandle() const;
  
  /// return rollback time if we are rolling back else return lVT
  virtual VTime calculateMin() = 0; 

  /// return current local virtual time
  virtual VTime getLVT() const;
  
  /// execute the simulation
  virtual void executeSimulation();

  /// call state queue garbage collect (with gvt)
  virtual VTime stateGcollect(VTime gtime);
  /// call state queue garbage collect (with state pointers)
  virtual VTime stateGcollect(VTime gtime, BasicEvent*& inputPtr,
			      Container<BasicEvent>*& outputptr);
  /// call output queue garbage collect (with gvt)
  virtual void outputGcollect(VTime gtime);
  /// call output queue garbage collect (with state pointers)
  virtual void outputGcollect(VTime gtime, Container<BasicEvent>*& outputQptr);
  /// call input queue garbage collect
  virtual int  inputGcollect(VTime gtime, BasicEvent* inputQptr = NULL);
  /// return global virtual time
  virtual VTime getGVT();
  /// am i within this time window ?
  virtual bool withinTimeWindow();
  /// simulate ...
  virtual void simulate();
  /// save application state
  virtual void saveState();
  /// coastforward from the given rollback time
  virtual void coastForward(VTime);
  /// start the cancellation procedure
  virtual void cancelMessages(const VTime);
  /// clear the pointers in the state
  virtual void stateClear();
  /// clear the initial state
  virtual void clearInitState();

  /// set the handle to my scheduler
  void setSchedulerHandle(LTSFScheduler* myScheduler);
  /// retrieve a handle to my scheduler
  inline LTSFScheduler* retSchedulerHandle() { return schedulerHandle; }
  
#if defined(LAZYCANCELLATION) || defined(LAZYAGGR_CANCELLATION)
  // currently used for GVT calculations
  virtual void openLCFile(char*);
  virtual void openDCStatsFile(char*);
  virtual VTime getLazyQMinTime();
  virtual int getLazyQSize();
#endif

  /// rollback all the file queues that exist
  virtual void rollbackFileQueues(VTime);
  /// look inside the input queue and return the first element
  inline BasicEvent* peekInputQ(){ return inputQ.get();}
  
  // NEW_STATE_MANAGEMENT:  
  // The functionnality needed for the new style of state management. The
  // state saving will now go via the process to which the state is associated
  // to. So every TimeWarp object should overload this virtual method to do
  // the job of allocating a new state.
  /// call application to allocate state
  virtual BasicState* allocateState() = 0;
  /// call application to deallocate state
  virtual void deAllocateState(BasicState *);
  /// retrieve the current state
  virtual BasicState* getCurrentState() const;
  
  /// open a file
  virtual void openFile();
  /// set the output file stream
  virtual void setFile(std::ofstream *);

#ifdef STATEDEBUG
  virtual void setStateFile(std::ofstream *);
#endif

  //@} // end of Public Class Methods

  /**@name Public Class Attributes */
  //@{
  
  /// name of this object
  char *name;
  /// unique ID number 
  int    id;       
  /// flag to indicate idle status
  bool iRanLastTime;
  /// flag for print status
  bool printIt;
  /// debug output file handle
  ofstream *lpFile;
  /// handle to this object for the GVT Manager
  BasicTimeWarp *gVTHandle;

#ifdef STATEDEBUG
  ofstream *lpFile2;
#endif

#ifdef MATTERNGVTMANAGER
  int color; // color of this process 0-white, 1-red, etc
  int currentColorMessages; // the number of white messages
  int redMessages;
  int whiteMessages;
  VTime tMin; // the minimal timestamp of red messages
#endif

#ifdef STATS
  ofstream *fileHandle;
  virtual void setStatsFile(std::ofstream *statsfile){
    cerr << "Error: This function shouldn't be called" << std::endl;
  }
#endif  

  ofstream outFile;

  /// local id for each lp
  int localId;                  

  /// static Input Queue
  static LTSFInputQueue inputQ; 

  /// State Manager
  STATE_MANAGER* state;

  //@} // end of Public Class Attributes
  
protected:

  /**@name Protected Class Methods */
  //@{

  /// causes the creation of initial state for every object
  virtual void timeWarpInit();

  //@}

  /**@name Protected Class Attributes */
  //@{

  /// handle to the scheduler
  LTSFScheduler* schedulerHandle;
  /// the output queue object that is associated with every object
  OutputQueue outputQ;
  /// a handle to the record about simulation objects
  ObjectRecord *commHandle;

  //@}
};

#endif
