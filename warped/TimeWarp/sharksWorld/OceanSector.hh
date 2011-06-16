#ifndef OCEANSECTOR_HH
#define OCEANSECTOR_HH

#include <fstream>
#include "SimulationObj.hh"
#include "OceanSectorState.hh"
#include "OceanEvent.hh"
#include "UtilityFunc.hh"
#ifdef __GNUG__
#include <builtin.h>
#else
#include "../rnd/builtin.h"
#endif

#include <strstream>
#include <math.h>
#include "config.hh"

#ifdef SEQUENTIAL
#include "SequentialLP.hh"
#else
#include "LogicalProcess.hh"
#endif

class OceanSector : public SimulationObj {

public:
  OceanSector(int id , char* myName , int *neighbour); 
  ~OceanSector();

  void executeProcess();
  void finalize();

  BasicState* allocateState() {
    return new OceanSectorState;
  }
  
private:

  OceanEvent *isFishInSector(OceanEvent *chkFish);
  OceanEvent *isSharkInSector(OceanEvent *chkShark);
  // private methods 
  void generateKillEvent(OceanEvent *killFish);
  void projectShark(OceanEvent *oe);// send message to the adjacent sectors when this guys 
  //reaches their SI (sphere of influence) crosses the boundary
  void projectFish(OceanEvent *oe); // send messages when this chap will reach the inner 
  //  boundary of this sector
  
  void projectDeadFish(OceanEvent *o);// does the following 
  // check if the fish is still there 
  // if yes the cancel all arrival message sent 
  // cancel the exit event sent for itself <-- basically ignore this 
  
  void checkForAttacks() ; // go through the fish list and shark list in current
  //state and find when the next attack will occur and schedule an event to 
  //cause a kill event

  bool isFishDead(OceanEvent *oe);
  void addSharkToState(OceanEvent *oe);
  void addFishToState(OceanEvent *oe);
  OceanEvent * removeFishFromState(OceanEvent *oe);
  void removeSharkFromState( OceanEvent *);
  void addCreatureToNeighbour(OceanEvent *oe);

  void calculateMinDist(OceanEvent *oe , int yBound , int xBound , int quadrant);// this function can be vastly imporved ..... this basically calcutes the min
  // distances to the boundary and project the fish/shrak to that boundary
  

  int getXIndex() ; 
  int getYIndex() ;

  // global information for all sectors 
  int totalNoOfSectors ;
  int noOfSectorsPerRow ;
  int sectorWidth ;
  int attackRange ;

  // local info 

  int innerWestBound , innerEastBound ; 
  int outerWestBound , outerEastBound ;
  int innerNorthBound , innerSouthBound ;
  int outerNorthBound , outerSouthBound ; 

  int neighbourArray[8]; // array of ids of the adjacent sectors

};

#endif 

