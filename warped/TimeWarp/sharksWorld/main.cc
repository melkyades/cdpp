#include <fstream>
#include <iostream.h>
#include <string>

#include "config.hh"

#ifdef SEQUENTIAL
#include "SequentialLP.hh"
#else
#include "LogicalProcess.hh"
#endif


#include "SimulationTime.hh"
#include "InitObject.hh"
#include "OceanSector.hh"
#include "OceanEvent.hh"

#include <stdio.h>
#include <cstdlib>

#include "CommMgrInterface.hh"

#ifdef SEQUENTIAL
const VTime SequentialLP::SIMUNTIL = 75;
#else
const VTime LogicalProcess::SIMUNTIL = 75;
#endif

void 
setSimEndTime(int ){
//  SIMUNTIL = time ; 
}

char *
getSectorName(int k ){
  strstream sectorName ;  
  sectorName << "sector_" << k << ends ;
  return sectorName.str()  ; 
}

int *
getNeighbourList(int sectorNo , int sectorsPerRow  ){
  int *destarr;
  int rno = 0 ;
  int cno = 0 ; 
  int r = 0 ; 
  int c = 0 ; 
  int n = 0 ;

  destarr = new int[8];
  rno = (sectorNo - 1)/sectorsPerRow;
  cno = (sectorNo - 1) % sectorsPerRow ;
  n = sectorsPerRow;
  
  c= cno ; 
  r = rno -1;
  if(r<0){
    r = n  - 1;
  }
  destarr[0] = r*n +c+1;

  r = rno  - 1; 
  c = cno  + 1;
  if (r < 0) {
    r = n -1 ;
  }
  if(c>=n){
    c = 0 ;
  }
  destarr[1] = r*n +c+1;
 
  r = rno ;
  c = cno + 1;
  if(c>= n){
    c = 0;
  }
  destarr[2] = r*n +c+1;
  
  r = rno + 1 ;
  c = cno + 1;
  if (r >= n){
    r = 0 ; 
  }
  if(c>=n){
    c = 0;
  }
  destarr[3] = r*n +c+1;
  
  r = rno + 1;
  c = cno ;
  if(r>=n){
    r = 0 ;
  }
  destarr[4] = r*n + c + 1;
  
  r = rno + 1;
  c = cno - 1;
  if(c<0){
    c = n - 1;
  }
  if(r>=n){
    r = 0;
  }
  destarr[5] = r*n + c + 1;

  r = rno ;
  c = cno - 1;
  if(c<0){
    c = n -1;
  }
  destarr[6] = r*n + c + 1;

  r = rno - 1;
  c = cno - 1;
  if(c<0){
    c =  n - 1;
  }
  if(r<0){
    r = n - 1;
  }
  destarr[7] = r*n + c + 1;
  //for (int o = 0 ; o < 8 ; o ++){
  //  cout << destarr[o] << " " ;
  // }
  //cout << endl ;
  return destarr;
};


int main(int argc , char *argv[]){
  
  int id  ;

#ifndef SEQUENTIAL
  physicalCommInit(&argc , &argv);
  id = physicalCommGetId();
#endif
  
#ifdef SEQUENTIAL
  id = 0 ;
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

  
  //if (argc <   2 ){
  //  cout <<"Format: SHARK SIMUNTIL" << endl ;
  //  exit(-1);
  //}
  //setSimEndTime(atoi(argv[1]));
 
  
  int totalNoOfSectors =  (noOfSectorsPerRow * noOfSectorsPerRow) ;
  int numSectorsPerLP  ;
  int overflow ;
  int *neighbour ; 
  char *sectorName ; 
  
  numSectorsPerLP   =  totalNoOfSectors /numLPs;
  overflow = totalNoOfSectors  % numLPs ;
  //cout << numSectorsPerLP << " " << overflow << endl ;

#ifdef MESSAGE_AGGREGATION
  getMessageManagerParameters(argc, argv);
#endif

  if ( id == 0){
    
#ifdef SEQUENTIAL
    SequentialLP lp (1+ totalNoOfSectors  , 1 + numSectorsPerLP + overflow
		     , numLPs);
#else
    LogicalProcess lp (1+ totalNoOfSectors  , 1 + numSectorsPerLP + overflow 
		       , numLPs);
#endif
    
    InitObject *initObject = new InitObject ( 0 ,"InitObject_0");
    lp.registerObject(initObject);

    OceanSector **sector = new OceanSector*[numSectorsPerLP + overflow] ;
    
    for (int k = 0 ; k <  numSectorsPerLP + overflow  ; k ++){
      
      sectorName  = getSectorName(k+1) ;
      neighbour = getNeighbourList( k+1 , noOfSectorsPerRow ) ; 
      // for (int i = 0 ; i < 8 ; i ++){
      //	cout << neighbour[i] << " " ;
      //}
     
      sector[k]   = new OceanSector( k+1 , sectorName , neighbour );
      lp.registerObject(sector[k]);  
    }
           
    lp.allRegistered();
    lp.simulate();
    
    delete initObject;

    for ( int index2 = 0 ; index2 < numSectorsPerLP + overflow ; index2 ++){
      delete sector[index2];
    }
    
  }
  else if (id > 0 && id < numLPs  ){
    
#ifdef SEQUENTIAL
    SequentialLP lp(1+ totalNoOfSectors , numSectorsPerLP  , numLPs);
#else
    LogicalProcess lp(1+ totalNoOfSectors , numSectorsPerLP  , numLPs);
#endif
    
    int startId = (1 + numSectorsPerLP + overflow)+(id - 1)*numSectorsPerLP  ;
    OceanSector **sector = new OceanSector*[numSectorsPerLP] ;
    for (int k = 0   ; k < numSectorsPerLP   ; k ++){
      //  cout << "L " << k << endl ;
      sectorName = getSectorName(startId + k) ;
      neighbour = getNeighbourList(startId +  k , noOfSectorsPerRow) ; 
      sector[k]   = new OceanSector( startId + k , sectorName, neighbour );
      lp.registerObject(sector[k]);
    }

    lp.allRegistered();
    lp.simulate();
    
    for ( int index3 = 0  ; index3 < numSectorsPerLP  ; index3 ++){
	delete sector[index3];
    }
  
  }
  else {
    cout << "Invalid LP id " << endl ;
    exit(-1);
  }
  
}










