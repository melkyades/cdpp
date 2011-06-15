// -*-c++-*-
#ifndef COSTFUNCSTATEMANAGER_HH
#define COSTFUNCSTATEMANAGER_HH
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
// $Id: CostFuncStateManager.hh,v 1.2 1999/10/25 14:23:22 ramanan Exp $
//
//
//---------------------------------------------------------------------------
#include "AdaptStateManager.hh"
#include <sys/time.h>

extern "C" {
double ceil(double);
double sqrt(double);
}

const int MAXDEFAULTINTERVAL = 50;

class CostFuncStateManager: public AdaptStateManager{

public:

  CostFuncStateManager(BasicTimeWarp *myProcessPtr);
  ~CostFuncStateManager() {};

  void calculatePeriod();
  void committedEvents(long, int);
  void coastForwardTiming(double);
  double getCoastForwardTime() { return(CoastForwardTimeWeighted.getAvg()); };
  void saveState();
  void setAdaptiveParameters(long, long);

private:

  // event number that marks the next time we will adapt the checkpoint 
  // interval
  long eventToRecalc;

  // number of events between adapting the checkpoint.
  long eventsBetweenRecalc;

  // events executed since last checkpoint interval recalculation.
  long forwardExecutionLength;  

  // the last checkpoint interval
  int lastCheckpointInterval;

  // cost Function for Periodic State Saving
  double filteredCostIndex;

  // old cost function
  double oldCostIndex;

  // adaption value for period (magnitude of 1 and sign gives direction)
  int adaptionValue;

};

#endif
