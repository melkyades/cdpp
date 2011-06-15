// -*-c++-*-
#ifndef INFREQSTATEMANAGER_HH
#define INFREQSTATEMANAGER_HH
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
// $Id: InfreqStateManager.hh,v 1.4 1999/10/25 20:00:39 ramanan Exp $
//
//---------------------------------------------------------------------------
#include "StateManager.hh"
#include "FIR.cc"

class AdaptStateManager;

/** The InfrequentStateManager Class.
    Base class for AdaptiveStateManager.
*/
class InfreqStateManager: public StateManager {

friend class AdaptStateManager;

public:

  /**@name Public Class Methods */
  //@{

  /// Constructor
  InfreqStateManager(BasicTimeWarp *myProcessPtr);
  /// Destructor
  ~InfreqStateManager() {};
  /// create an initial state
  virtual void createInitialState();
  /// set the state saving interval/period
  void setStatePeriod(int);
  /// retrieve the state saving interval/period
  int getStatePeriod() { return (statePeriod); };
  /// save application state
  void saveState();
  /// restore the correct state during rollback
  VTime restoreState(VTime);
  // call garbage collect
  VTime gcollect(VTime, BasicEvent*&, Container<BasicEvent>*&);
  /// use time for gcollect ? ...
  bool useTimeForGcollect() {return (stateQ.front() )->dirty; };

  //@}
protected:
  /**@name Protected Class Attributes */
  //@{
  /// This is the time of the last call to save state. This is not equal to (stateQ.back() )->lVT.
  VTime timeAtLastCall;
  //@}


private:
  /**@name Protected Class Attributes */
  //@{
  /// the state period with which we save state infrequently
  int statePeriod;
  /// time since the last saved state
  int periodCounter;
  //@}
};

#endif
