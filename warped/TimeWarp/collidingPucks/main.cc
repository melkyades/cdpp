#include <fstream.h>
#include <iostream.h>
#include <string.h>

#include "config.hh"

#ifdef SEQUENTIAL
#include "SequentialLP.hh"
#else
#include "LogicalProcess.hh"
#endif

#include "SimulationTime.hh"
#include "InitObject.hh"
#include "tableSector.hh"
#include "tableEvent.hh"

#include <stdio.h>
#include <stdlib.h>

#include "CommMgrInterface.hh"

#ifdef SEQUENTIAL
const VTime SequentialLP::SIMUNTIL = 50 ;
#else
const VTime LogicalProcess::SIMUNTIL = 50 ;
#endif

//void
//setSimEndTime(int time){
//  SIMUNTIL = time ;
//}

char *
getSectorName(int k ){
  strstream sectorName ;
  sectorName << "sector_" << k << ends ;
  return sectorName.str()  ;
}


int *
getNeighbourList(int sectorNo , int sectorsPerRow  ){
  int *destarr;
  destarr = new int[8];
  int rno = 0 ;
  int cno = 0 ;
  int r = 0 ;
  int c = 0 ;
  int n = 0 ;

  rno = (sectorNo - 1)/sectorsPerRow;
  cno = (sectorNo - 1) % sectorsPerRow ;
  n = sectorsPerRow;

   c= cno ;
   r = rno -1;

   if(r<0){     
     destarr[0] = -10;
   }

   else {
     destarr[0] = r*n +c+1;
   }

    r = rno  - 1;
    c = cno  + 1;
    
    if(( r < 0) || ( c >= 0)){
      destarr[1] = -10;
    }
    
    else {
      destarr[1] = r*n +c+1;
    }

    r = rno ;
    c = cno + 1;

    if(c>= n){
      destarr[2] = -10;
    }
   
    else {
      destarr[2] = r*n +c+1;
    }

     r = rno + 1 ;
     c = cno + 1;

     if( r>=n || c>=n){
       destarr[3] = -10;
     }

     else {
       destarr[3] = r*n +c+1;
     }

     r = rno + 1;
     c = cno ;

     if(r>=n){
       destarr[4] = -10;
     }

     else {
       destarr[4] = r*n + c + 1;
     }

     r = rno + 1;
     c = cno - 1;

     if(c<0 || r>=n){
        destarr[5] = -10 ;
     }

     else {
       destarr[5] = r*n + c + 1;
     }

     r = rno ;
     c = cno - 1;
     if(c<0){
       destarr[6] = -10;
     }

     else {
       destarr[6] = r*n + c + 1;
     }

     r = rno - 1;
     c = cno - 1;
     if(c<0 || r<0){
       destarr[7] = -10;
     }
    
     else {
        destarr[7] = r*n + c + 1;
     }

     return destarr;
};

int main(int argc , char *argv[]){

  int id;


#ifdef SEQUENTIAL
  id = 0 ;
#endif

#ifndef SEQUENTIAL
  physicalCommInit(&argc , &argv);
  id = physicalCommGetId();
#endif
  
  int noOfSectorsPerRow ;
  int sectorWidth ;
  int noOfPucks ;
  int numLPs ;

  ifstream infile("collidingPucks.config");

  if (infile.good()){
    infile >> noOfSectorsPerRow >> sectorWidth >> noOfPucks >> numLPs ;
    infile.close();
  }

  else {
    cerr << "Cannot find file collidingPucks.config" << endl;
    exit(-1);
  }

  // if (argc < 2){
  //  cout << "Format : PUCK SIMUNTIL" << endl ;
  //  exit(-1);
  // }

  // setSimEndTime(atoi(argv[1]));

  int totalNoOfSectors =  (noOfSectorsPerRow * noOfSectorsPerRow) ;
  int numSectorsPerLP  ;
  int overflow ;
  int *neighbour=NULL ;
  char *sectorName ;

  numSectorsPerLP   =  totalNoOfSectors /numLPs;
  overflow = totalNoOfSectors  % numLPs ;
#ifdef MESSAGE_AGGREGATION
  getMessageManagerParameters(argc, argv);
#endif
  if ( id == 0){

#ifdef SEQUENTIAL
    SequentialLP lp(1+totalNoOfSectors , 1+numSectorsPerLP+overflow , numLPs);
#else
    LogicalProcess lp(1+totalNoOfSectors , 1+numSectorsPerLP+overflow , numLPs);
#endif

    InitObject *initObject = new InitObject ( 0 ,"InitObject_0");
    lp.registerObject(initObject);

    tableSector **sector = new tableSector*[numSectorsPerLP + overflow] ;
    
    for (int k = 0 ; k <  numSectorsPerLP + overflow  ; k ++){
      sectorName  = getSectorName(k+1) ;
      neighbour = getNeighbourList( k+1 , noOfSectorsPerRow ) ;
      sector[k] = new tableSector( k+1 , sectorName , neighbour );
      lp.registerObject(sector[k]);
      delete []neighbour;
    }

  
    lp.allRegistered();
    lp.simulate();

    delete initObject;
    for ( int index1 = 0 ; index1 < numSectorsPerLP + overflow ; index1 ++){
      delete sector[index1];
    }
     delete []sector;
  }
    else if (id > 0 && id < numLPs  ){

#ifdef SEQUENTIAL
      SequentialLP lp(1+ totalNoOfSectors , numSectorsPerLP  , numLPs);
#else
      LogicalProcess lp(1+ totalNoOfSectors , numSectorsPerLP  , numLPs);
#endif

       int startId = (1 + numSectorsPerLP + overflow)+(id - 1)*numSectorsPerLP;
       tableSector **sector = new tableSector*[numSectorsPerLP] ;
       for (int index2 = 0   ; index2 < numSectorsPerLP   ; index2 ++){
         sectorName = getSectorName(startId + index2) ;
         neighbour = getNeighbourList(startId +  index2 , noOfSectorsPerRow) ;
         sector[index2]   = new tableSector( startId + index2 , sectorName, neighbour );
         lp.registerObject(sector[index2]);
	 delete []neighbour;
       }
       
       lp.allRegistered();
       lp.simulate();
        for ( int index3 = 0  ; index3 < numSectorsPerLP  ; index3 ++){
          delete sector[index3];
	}
	delete [] sector;
    }
    else {
      cout << "Invalid LP id " << endl ;
      exit(-1);
    }
}







