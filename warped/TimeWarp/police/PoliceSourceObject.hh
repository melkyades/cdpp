//-*-c++-*-
#ifndef POLICESOURCEOBJECT_HH
#define POLICESOURCEOBJECT_HH
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
//
// $Id: PoliceSourceObject.hh,v 1.2 1998/05/07 22:43:40 gsharma Exp $
//
//
//---------------------------------------------------------------------------

#include <iostream.h>

#ifdef __GNUG__
#include <MLCG.h>
#include <Normal.h>
#include <Poisson.h>
#include <Binomial.h>
#include <Uniform.h>
#include <NegExp.h>
#include <Random.h>
#include <RndInt.h>
#else
#include "../rnd/MLCG.h"
#include "../rnd/Normal.h"
#include "../rnd/Poisson.h"
#include "../rnd/Binomial.h"
#include "../rnd/Uniform.h"
#include "../rnd/NegExp.h"
#include "../rnd/Random.h"
#include "../rnd/RndInt.h"
#endif


#include "SidTypes.hh"
#include "QueuingTypes.hh"
#include "PoliceSourceState.hh"
#include "SimulationObj.hh"

// A PoliceSourceObject dispatched calls that must be serviced by the
// server.   
// assume that each PoliceSourceObject is a local police precinct which is
// transfering calls to police units (police cars/ police motorcyles)

class PoliceSourceObject : public SimulationObj  {
private:
  int numberOfCars;
  int *callsRecord;
  double first;
  double second;
  distribution_t sourcedist ;    
public :
  Uniform  * uniform;
  Normal   * normal ;
  Binomial * binomial ;
  Poisson  * poisson ;
  NegativeExpntl * expo ;

  // for UNIFORM we need the minimum time and maximum time
  // for NORMAL we need mean, variance
  // for BINOMIAL we need the number of items in the pool, and the probability
  // that each item is drawn
  // for POISSON we need the mean
  // for EXPONENTIAL we need the mean
  // for FIXED we need the time between one token and the next

  int    dest;
  
  int setDelay();
  void newMsg(int);

  PoliceSourceObject(int myid, const char* myname, int numCars);
  ~PoliceSourceObject();

  void executeProcess();
  void initialize();
  void setDistribution(distribution_t distribution, double firstParameter,
		       double secondParameter = 0.0);

  BasicState* allocateState() {
    return new PoliceSourceState();
  }

};

#endif




