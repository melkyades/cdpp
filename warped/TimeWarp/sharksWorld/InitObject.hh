#ifndef INITOBJECT_HH
#define INITOBJECT_HH

#include "SimulationObj.hh" 
#include "SeaTypes.hh" 

#ifdef __GNUG__
#include <Uniform.h>
#include <MLCG.h>
#else
#include "../rnd/Uniform.h"
#include "../rnd/MLCG.h"
#endif

#include <fstream>
#include <math.h>
#include "InitObjectState.hh"
#include "OceanEvent.hh"

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
  int getVelocity(creatureType ct);
  int getXLocation(int sectorId);
  int getYLocation(int sectorId);


  // variables
  int noOfSectorsPerRow ;
  int totalNoOfSectors ;
  int sectorWidth ;
  int attackRange ; 
  int noOfFishes ;
  int noOfSharks ;
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





















