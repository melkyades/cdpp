//-*-c++-*-
#ifndef POLICESERVEROBJECT_HH
#define POLICESERVEROBJECT_HH
// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
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
// $Id: PoliceServerObject.hh,v 1.2 1998/05/07 22:43:37 gsharma Exp $
//
//
// 
//
//---------------------------------------------------------------------------

#include <iostream.h>
#include "QueuingTypes.hh"
#include "SidTypes.hh"
#include "SimulationObj.hh"
#include "PoliceServerState.hh"

#ifdef __GNUG__
#include <Normal.h>
#include <Poisson.h>
#include <Binomial.h>
#include <Uniform.h>
#include <NegExp.h>
#include <Random.h>
#else
#include "../rnd/Normal.h"
#include "../rnd/Poisson.h"
#include "../rnd/Binomial.h"
#include "../rnd/Uniform.h"
#include "../rnd/NegExp.h"
#include "../rnd/Random.h"
#endif


class PoliceServerObject : public SimulationObj  {
public :
    Uniform * uniform;
    Normal * normal ;
    Binomial * binomial ;
    Poisson * poisson ;
    NegativeExpntl * expo ;
    
    int serverdest ;
    PoliceServerObject(char *myname) { 
      name = new char[strlen(myname)+1];
      strcpy(name,myname);
#ifdef OBJECTDEBUG
      cout << " constucting Server object " << endl ;
#endif
    };
  
   ~PoliceServerObject() { 
#ifdef OBJECTDEBUG
       cout << " destructing Server object " << endl ;
#endif
       cout << "Received " << ((PoliceServerState *)state->current)->numPriorityCalls 
            << " priority calls " << endl ;
    };

  void initialize();
  void executeProcess();
  void setServerDistribution(distribution_t distribution, 
			     double firstParameter,
			     double secondParameter = 0, 
			     double (*func)() = NULL);
  BasicState* allocateState() {
    return new PoliceServerState;
  }
  
  int dest;
private:
  double delay;
  double first, second ;
  distribution_t serverDist;
  

  // User defined function which will be used for variable delays that
  // Depend upon some state variable.
  double (*userFunction)();
};
//#include"PoliceServerObject.cc"
#endif 
