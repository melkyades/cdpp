//-*-c++-*-
#ifndef FORKOBJECT_HH
#define FORKOBJECT_HH
// Copyright (c) 1994,1995 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.
//
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
//

// $Id: ForkObject.hh,v 1.1 1998/03/07 21:06:33 dmadhava Exp $
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
#else
#include "../rnd/MLCG.h"
#include "../rnd/Normal.h"
#include "../rnd/Poisson.h"
#include "../rnd/Binomial.h"
#include "../rnd/Uniform.h"
#include "../rnd/NegExp.h"
#include "../rnd/Random.h"
#endif

#include "SidTypes.hh"
#include "SimulationObj.hh"

class ForkObject : public SimulationObj {
public:
  
  ForkObject(char *myName);
  
  ~ForkObject();
  
  void initialize();  
  void executeProcess();
  void setForkDistribution(distribution_t choice, int numberOut, 
			   int *outputIds, double firstParamter = 0, 
			   double secondParameter = 0);
  
  BasicState*  allocateState();
private:
  VTime newTime;
  
  // first and second parameter for the distributions
  double first, second ;
  
  // choice of routing function
  distribution_t forkType ;
  
  // number of outputs
  int numFanOut ;
  
  // IDs for the outputs
  int *fanOutIds;
  
  // destination ID
  int dest;
};

#endif



