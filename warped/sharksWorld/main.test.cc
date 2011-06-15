
#include <iostream.h>
#include <string.h>

#include "LogicalProcess.hh"

#include "SimulationTime.hh"
#include "InitObject.hh"
#include "OceanSector.hh"
#include "OceanEvent.hh"

#include <stdio.h>
#include <stdlib.h>
#ifdef MPI
extern "C" {
#include "mpi.h"
}
#endif
VTime SIMUNTIL ;

void 
setSimEndTime(int time){
  SIMUNTIL = time ; 
}

int main(int argc , char *argv[]){
  
  int id  ; 
#ifdef MPI
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &id);
#endif
  



  int noOfSectorsPerRow ;
  int sectorWidth ; 
  int attackRange ; 
  int noOfFishes ; 
  int noOfSharks ; 
  int numLPs ; 

  ifstream infile("sharksWorld.config");
  
  if (infile.good()){
    infile >> noOfSectorsPerRow >> sectorWidth >> attackRange >> noOfFishes 
	   >> noOfSharks >> numLPs  ;
    infile.close();
  }
  else {
    cerr << "Cannot find file sharksWorld.config" << endl;
    exit(-1);
  }

  if (sectorWidth <= 2*attackRange ){
    cout << "attack ranges is too large i.e bigger than sector width" <<
	  "hey is is suppposed to be a ocean " << endl;
    exit(-1);
  }

  
  if (argc <   2 ){
    cout <<"Format: SHARK SIMUNTIL" << endl ;
    exit(-1);
  }
  setSimEndTime(atoi(argv[1]));
 
  
  if ( id == 0){
    

    LogicalProcess lp (5 ,3 ,2 );
    
    InitObject *initObject = new InitObject ( 0 ,"InitObject_0");
    lp.registerObject(initObject);

      
      int dest1[] = { 3, 4, 2, 4 , 3 ,4, 2, 4};
      
      OceanSector *sector_1 = new OceanSector( 1 , "sector_1" , dest1 );
      lp.registerObject(sector_1);
      
      int dest2[] = { 4,3,1,3,4,3,1,3 };
      OceanSector *sector_2 = new OceanSector( 2 , "sector_2" , dest2);
      lp.registerObject(sector_2);
      
      lp.allRegistered();
      lp.simulate();
      
      delete initObject;
      delete sector_1;
      delete sector_2;
    }
  else if (id == 1 ){
    LogicalProcess lp(5 ,2 ,2);
    
    int dest3[] = { 1,2,4,2,1,2,4,2};
    OceanSector *sector_3 = new OceanSector( 3 , "sector_3" , dest3);
    lp.registerObject(sector_3);

    int dest4[] = { 2,1,3,1,2,1,3,1};
    OceanSector *sector_4 = new OceanSector( 4 , "sector_4" , dest4);
    lp.registerObject(sector_4);

    lp.allRegistered();
    lp.simulate();
    
  
    delete sector_3;
    delete sector_4;
     
  }
}





