#include <iostream.h>
#include "PingObject.hh"

#ifndef SEQUENTIAL
//we want a prallel run so include LogicalProcess.hh
#include "LogicalProcess.hh"
#else
//we want a sequential run so include SequentialLP.hh
#include "SequentialLP.hh"
#endif

#ifndef SEQUENTIAL
//we want a parallel run so include MPI files
#ifdef MPI
extern "C" {
#include "mpi.h"
}
#endif
#endif

//Number of Logical Processes hardcoded to two
const int NUMLPS = 2 ;

//set SIMUNTIL's in Logical Process classes
#ifndef SEQUENTIAL
const VTime LogicalProcess::SIMUNTIL = PINFINITY;
#else
const VTime SequentialLP::SIMUNTIL = PINFINITY;
#endif

int main(int argc, char **argv)
{

  int numPlayers , numBalls ;
  
  ifstream CONFIG_FILE;
  CONFIG_FILE.open("ping.config");
  
  CONFIG_FILE >> numPlayers >> numBalls ;
  cout <<"From ping.config: numPlayers is "
       <<numPlayers
       <<" and numBalls is "
       <<numBalls <<endl<<endl;
  
  int id = 0;
  
#ifndef SEQUENTIAL
#ifdef BEST_PARTITION
  cout <<"Simulating with best case partioning ." << endl << endl;
#else
  cout <<"Simulating with worst case partioning ." << endl << endl;
#endif
#endif

#ifndef SEQUENTIAL
#ifdef MPI
  physicalCommInit(&argc , &argv);
  id = physicalCommGetId();
#endif
#endif
  
  if(id < NUMLPS - 1)
    {
      //this is LP0
      int processesOnThisLP ;

#ifndef SEQUENTIAL      
      //distribute objects across LPs in half
      if(numPlayers % 2 == 0)
	{
	  processesOnThisLP = numPlayers / 2 ;
	}else{
#ifdef BEST_PARTITION
	  processesOnThisLP = numPlayers / 2 ;
#else
	  processesOnThisLP = numPlayers / 2 + 1 ;
#endif

	}
#else
      //we are running sequential so all simulation objects on 
      // this lp
      processesOnThisLP  =  numPlayers ;
#endif
      

      pingObject** pingObjectArray = new pingObject*[processesOnThisLP];

#ifndef SEQUENTIAL      
      //tell logical process about
      //totalobjects,local objects,number of Logical Processes
      LogicalProcess lp(numPlayers,processesOnThisLP,NUMLPS);
#else
      SequentialLP lp(numPlayers,processesOnThisLP,NUMLPS);
#endif

      int index ;
      char nameBuffer[15];
      int objectNumber = 0 ;
      int myID ;
      int myDest ;

      for (index = 0; index < processesOnThisLP; index++) 
	{
	  myID = objectNumber;
	  //give this simulation object a name
	  sprintf(nameBuffer,"object%d",objectNumber);
	
#ifndef SEQUENTIAL
#ifdef BEST_PARTITION
	  objectNumber++;
#else
	  //do worst case partioning
	  objectNumber += 2 ;
#endif
#else
	  objectNumber++;
#endif
	  
	  if(myID == numPlayers - 1)
	    {
#ifdef BEST_PARTITION
	      myDest = myID + 1; 
#else
	      myDest = 0 ;
#endif
	    }else{
	      myDest = myID + 1;
	    }
	  
	  pingObjectArray[index] = new 
	    pingObject(myID,myDest,numBalls,nameBuffer);
#ifdef APPL_DEBUG
	  cout <<nameBuffer 
	       <<" registering on LP0 " 
	       <<" id is " << myID
	       <<" dest is " << myDest
	       << endl;
#endif
	  //register this simulation object with it's Logical Process
	  lp.registerObject(pingObjectArray[index]);
	}
      
      //everybody registered. let's tell this to Logical Process
      lp.allRegistered();
      
      //OK done. let's start this Logical Process's simulation
      lp.simulate();
      
      for (index = 0; index < processesOnThisLP; index++) 
	{
	  delete pingObjectArray[index];
	}
    
      delete [] pingObjectArray ;
    
    
    }else{
      
#ifndef SEQUENTIAL
      //this is LP1
      //distribute objects across LPs in half
      int processesOnThisLP ;
      int objectNumber ;

#ifdef BEST_PARTITION
      if(numPlayers % 2 == 0)
	{
	  processesOnThisLP = numPlayers / 2 ;
	  objectNumber = numPlayers / 2 ;
	}else{
	  processesOnThisLP = numPlayers / 2 + 1 ;
	  objectNumber = numPlayers / 2 ;
	}
#else
      processesOnThisLP = numPlayers / 2 ;
      objectNumber = 1 ;
#endif

      pingObject** pingObjectArray = new pingObject*[processesOnThisLP];

      //tell logical process about
      //totalobjects,local objects,number of Logical Processes
      LogicalProcess lp(numPlayers,processesOnThisLP,NUMLPS);
      
      int index ;
      char nameBuffer[15];
      int myID ;
      int myDest ;
      
      for (index = 0; index < processesOnThisLP; index++) 
	{
	  
	  myID = objectNumber ;
	  //give this simulation object a name
	  sprintf(nameBuffer,"object%d",objectNumber);

#ifdef BEST_PARTITION
	  //do best case partioning
	  objectNumber ++;
#else	  
	  //do worst case partioning
	  objectNumber += 2 ;
#endif

	  
	  if(myID == numPlayers - 1)
	    {
	      myDest = 0 ;
	    }else{
	      myDest = myID + 1;
	    }
	  pingObjectArray[index] = new 
	    pingObject(myID,myDest,numBalls,nameBuffer);
#ifdef APPL_DEBUG
	  cout <<nameBuffer 
	       <<" registering on LP1 "
	       <<" id is " << myID
	       <<" dest is " << myDest
	       << endl;
#endif
	  //register this simulation object with it's Logical Process
	  lp.registerObject(pingObjectArray[index]);
	}
      //everybody registered. let's tell this to Logical Process
      lp.allRegistered();
      
      //OK done. let's start this Logical Process's simulation
      lp.simulate();
      
      for (index = 0; index < processesOnThisLP; index++) 
	{
	  delete pingObjectArray[index];
	}
      
      delete [] pingObjectArray ;
#endif      

    }

}
      
  
