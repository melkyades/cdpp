//-*-c++-*-
#ifndef SEQUENTIAL_LP_HH
#define SEQUENTIAL_LP_HH
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
// $Id: SequentialLP.hh,v 1.1 1998/12/22 20:35:47 dmadhava Exp $
//
//---------------------------------------------------------------------------

#include "SequentialObj.hh"
#include "BasicEvent.hh"
#include "DefaultVTime.hh"
#include "Stopwatch.hh"
#include <ctime>

class BaseSequential;
class SequentialObjectRecord;
class SequentialObj;

class SequentialLP {
  friend class BaseSequential;
public:
  // Arguments are: Total number of Objects, number of objects in this LP,
  // number of LPs, and time upto which to simulate.
  SequentialLP(int, int, int);
  
  virtual ~SequentialLP() ;
  
  int getNumObjects();            
  int getLPid();
  
  bool simulationFinished;
  
  void simulate(VTime simUntil = PINFINITY );  // Simulate until time
  
  SequentialObjectRecord* simArray;
  
  void allRegistered( void ); // called by application to tell the kernel 
                              // when all of the simulation objects in this
                              // LP have registered
  void registerObject( BaseSequential* handle );
  
#ifdef LPDEBUG
  ofstream lpFile;
#endif
  
#ifdef STATS
  ofstream commitStatsFile;
#endif
  
#ifdef STATEDEBUG
  ofstream outFile2;
#endif
  
  // Time upto which to simulate.  The definition of this variable is in
  // main.cc
  static const VTime SIMUNTIL;
  
private:
  bool initialized;     // flag that gets set if we've been initialized
  
  int numRegistered;
  int numObjects;       // number of objects on this LP
  int totalObjects;     // total number of sim objects
  int numLPs;           // the number of LPs
  int id;               // my LP id
#ifdef STATS
  int eventCount;
  Stopwatch stopwatch;
#endif
};

#endif






