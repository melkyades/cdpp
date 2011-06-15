// -*-c++-*-
#ifndef ADAPTSTATEMANAGER_HH
#define ADAPTSTATEMANAGER_HH
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
// $Id: AdaptStateManager.hh,v 1.3 1999/10/25 20:00:38 ramanan Exp $
//
// 
//
//---------------------------------------------------------------------------
#include "InfreqStateManager.hh"
#include "IIR.cc"
#include "Stopwatch.hh"

#ifdef LPDEBUG
#ifdef SHOWTIMINGS
#define DEBUGTIMINGS
#endif
#endif

/** The AdaptStateManager Base Class.
    Base class for all adaptive State Managers:
    \begin{itemize}
     \item LinStateManager (Lin's adaptive State Saving algorithm)
     \item NashStateManager (Palaniswamy's adaptive State Saving algorithm)
     \item CostFuncStateManager (Fleischman's heuristic State Saving algorithm)
    \end{itemize}
*/
class AdaptStateManager: public InfreqStateManager{

public:

  /**@name Public Class Methods */
  //@{

  /// Constructor
  AdaptStateManager(BasicTimeWarp *myProcessPtr);
  
  /// Destructor.
  ~AdaptStateManager() {};
  
  /**@name basic timing operators */
  //@{ 
  /// start StopWatch to time event execution
  void startEventTiming();
  /// stop StopWatch
  bool stopEventTiming();
  /// pause StopWatch
  void pauseEventTiming();
  /// resume StopWatch
  void resumeEventTiming();
  //@}

  /**@name rollback timing operators */
  //@{ 
  /// start StopWatch to time rollback
  void startRollbackTiming();
  /// stop rollback StopWatch
  void finishRollbackTiming();
  //@}

  /**@name state saving timing operators */
  //@{ 
  /// start StopWatch to time state saving
  void startStateTiming();
  /// stop state StopWatch
  void stopStateTiming();
  //@}

  /// save the simulation object's state
  void saveState();

  /**@name retrieve time operators */
  //@{ 
  /// return time taken for state saving
  double getStateSavingTime();

  /// return time taked for event execution
  double getEventExecutionTime();

  /// return time taken for coast forwarding
  double getCoastForwardTime();
  //@}

  //@} // end of Public Class Methods

  /**@name Public Class Attributes */
  //@{

  /// filtered time to save one state
  FIR<double> stateSaveTime;

  /// filtered time to execute one Event
  FIR<double> eventExecutionTime;

  /// summation of the event execution time
  double sumEventTime;

  /// filtered time to coast forward
  FIR<double> coastForwardTime;

  /// filtered rollback length;
  FIR<int> rollbackLength;

  // weighted filtered time to save one state
  IIR<double> StateSaveTimeWeighted;

  // weighted filtered time to execute one Event
  IIR<double> EventExecutionTimeWeighted;

  // weighted filtered time to coast forward
  IIR<double> CoastForwardTimeWeighted;

  // weighted filtered rollback length;
  IIR<int> rollbackLengthWeighted;

  //@} // end of Public Class Attributes

protected:

  /**@name Protected Class Methods */
  //@{

  /// add execution time to the total time 
  inline bool executionTiming(double execTime);

  //@}

  /**@name Protected Class Attributes */
  //@{

  /**@name StopWatch time variables */
  //@{ 

  /// variable used to store the starting time of save state operation
  double stateStartTime;

  /// variable used to store the stop time of save state operation
  double stateStopTime;

  /// variable used to store the time of event execution
  double eventTime;

  /// variable used to store the time of rollback operation
  double rollbackTime;

  /// variable used to store the time of event operation
  double pausedTime;
  //@}

  /// rollback count of last iteration
  int lastRollbackCount;

  /// number of events committed
  long userEventsCommitted;

  /// number of events committed in last iteration
  long lastUserEventsCommitted;

  /// flag that says we are in steady state, so no timings are needed
  bool steadyState;

  /// flag to turn off adaptive algorithm
  bool nonoptCheckpointInterval;

  //@} // end of Protected Class Attributes

private:

  /**@name Private Class Attributes */
  //@{

  /// The StopWatch object
  Stopwatch stopWatch;

  /// The message StopWatch object
  Stopwatch messageStopWatch;

  /// The rollback StopWatch object
  Stopwatch rollbackStopWatch;

  //@}
};

#endif
