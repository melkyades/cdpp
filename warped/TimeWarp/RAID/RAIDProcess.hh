//-*-c++-*-
#ifndef RAIDPROCESS_HH
#define RAIDPROCESS_HH
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
//
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
// $Id: RAIDProcess.hh,v 1.1 1998/03/07 21:03:59 dmadhava Exp $
//
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
#include <DiscUnif.h>
#else
#include "../rnd/MLCG.h"
#include "../rnd/Normal.h"
#include "../rnd/Poisson.h"
#include "../rnd/Binomial.h"
#include "../rnd/Uniform.h"
#include "../rnd/NegExp.h"
#include "../rnd/DiscUnif.h"
#endif

#include "RAIDTypes.hh"
#include "SimulationObj.hh"

enum distribution_t {UNIFORM, POISSON, EXPONENTIAL, NORMAL, BINOMIAL, FIXED,
                     ALTERNATE, ROUNDROBIN, CONDITIONAL, ALL};

class RAIDProcess : public SimulationObj {
private:
  int MAXDISKS;
  int MAXCYLINDER;
  int MAXTRACK;
  int MAXSECTOR;
  int TOTALSTRIPESIZE;
  int MAXSTRIPESIZE;

  int maxRequests;
  int dest;
  int firstDiskId;

  DISK_TYPE diskType;
  
  distribution_t sourcedist;
  double first;
  double second ;

  void newConfiguration(int,const char*, int, int, DISK_TYPE, int, int);

  int msgDelay();
  void newMsg();

public:
  RAIDProcess(){};
  
  RAIDProcess(int,const char*, int, int, DISK_TYPE, int, int);
  ~RAIDProcess();

  // for UNIFORM we need the minimum time and maximum time
  // for NORMAL we need mean, variance
  // for BINOMIAL we need the number of items in the pool, and the probability
  // that each item is drawn
  // for POISSON we need the mean
  // for EXPONENTIAL we need the mean
  // for FIXED we need the time between one token and the next

  void setDistribution(distribution_t distribution, double firstParameter,
		       double secondParameter = 0.0);
  
  void configureRAIDProcess(int,const char*, int, int, DISK_TYPE, int, int);
  void executeProcess();
  void initialize();

  BasicState* allocateState();
};

#endif
