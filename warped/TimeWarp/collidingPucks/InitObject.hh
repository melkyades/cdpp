#ifndef INITOBJECT_HH
#define INITOBJECT_HH

#include "SimulationObj.hh"
#include "tableTypes.hh"
#include "tableEvent.hh"
#include "InitObjectState.hh"

#ifdef  __GNUG__
#include <Uniform.h>
#include <MLCG.h>
#else
#include "../rnd/Uniform.h"
#include "../rnd/MLCG.h"
#endif

#include <fstream>
#include <math.h>


class InitObject : public SimulationObj {

public:
  InitObject(int id , char *myName );
  ~InitObject();

   void initialize();
   void executeProcess();

  BasicState* allocateState() {
    return new InitObjectState;
  }
  
private:
  //private methods
  int getVelocity();
  int getXLocation(int sectorId);
  int getYLocation(int sectorId);

// variables
  int noOfSectorsPerRow ;
  int totalNoOfSectors ;
  int sectorWidth ;
  int noOfPucks ;
  MLCG mlcg ;
  Uniform *getSector ;
  Uniform *getTheta ;
  Uniform *setVelocity ;
  Uniform *setXLoc ;
  Uniform *setYLoc ;
  // files
  ofstream initConfig; 
};

#endif
