// -*-c++-*-
#ifndef NASHSTATEMANAGER_HH
#define NASHSTATEMANAGER_HH
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
// $Id: NashStateManager.hh,v 1.2 1998/12/01 14:53:36 dmartin Exp $
//
//
//---------------------------------------------------------------------------
#include "IIR.cc"
#include "FIR.cc"
#include <sys/time.h>
#include "AdaptStateManager.hh"

extern "C" {
double ceil(double);
double sqrt(double);
}

const int MAXDEFAULTINTERVAL = 50;

class NashStateManager: public AdaptStateManager {

public:

  NashStateManager(BasicTimeWarp *myProcessPtr);
  ~NashStateManager();

  void calculatePeriod(int);
  void committedEvents(long, int );
  void coastedForwardEvents( int );
  void saveState();
  VTime restoreState( VTime );
  void setAdaptiveParameters( long, long );
  
private:
  int numberOfRollbacks;
    // Name of the Object

  IIR<int> numberOfRollbacksWeighted;
  IIR<int> eventsCommittedWeighted;

  // event number that marks the next time we will adapt the checkpoint 
  // interval
  long eventToRecalc;
  // number of events between adapting the checkpoint.
  long eventsBetweenRecalc;
  // events executed since last checkpoint interval recalculation.
  long forwardExecutionLength;  
  // checkpoint interval of the last cycle
  int lastCheckpointInterval;

};

#endif
