#ifndef TABLESECTOR_HH
#define TABLESECTOR_HH

#include <fstream.h>
#include "SimulationObj.hh"
#include "tableSectorState.hh"
#include "tableEvent.hh"
#include "UtilityFunc.hh"

#ifdef __GNUG__
#include <builtin.h>
#else
#include "../rnd/builtin.h"
#endif

#include <strstream.h>
#include <math.h>

#include "config.hh"

#ifdef SEQUENTIAL
#include "SequentialLP.hh"
#else
#include "LogicalProcess.hh"
#endif

#include "tableSectorState.hh"

class tableSector : public SimulationObj {

public:
  tableSector(int identifier , char *myName , int *dest);
   //Constructor gives sector name such as sector1 etc.id is sec no and an 
   // array of nieghbouring sectors.
   
     ~tableSector();
      

     void executeProcess();
           

 tableEvent *isPuckInSector(tableEvent *chkPuck);

 void addPuckToState(tableEvent *te); //adds ball to que if the event so needs
 
 void projectPuck(tableEvent *te); //projects puck movemement to neighbours


 void removePuckFromState(tableEvent *te); //removes ball from queue

 void checkForCollisions(); 
    
 void calculateMinDist(tableEvent *te , int yBound , int xBound , int quadrant);

 void generateCollisionEvent(tableEvent* Puck1 , tableEvent* Puck2);

 void generateEdgeCollisionEvent(tableEvent* Puck);

 int getXIndex() ;
 int getYIndex() ;


  BasicState* allocateState() {
    return new tableSectorState;
  }
  
  int totalNoOfSectors;
  int noOfSectorsPerRow;
  int sectorWidth;

  int westBound;
  int eastBound;
  int northBound;
  int southBound;

 int neighbourArray[8]; // array of ids of the adjacent sectors


};

#endif  

