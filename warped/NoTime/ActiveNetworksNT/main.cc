#include <fstream>
#include <iostream.h>
#include <string>

#include "LogicalProcess.hh"

#include "SimulationTime.hh"
#include "ActiveNode1.hh"

#include <stdio.h>
#include <cstdlib>

const VTime LogicalProcess::SIMUNTIL = 50;

#ifdef MPI
extern "C" {
#include "mpi.h"
}
#endif

char *
getNodeName(int k ){
  strstream NodeName ;
  NodeName << "node_" << k << ends ;
  return NodeName.str()  ;
}


int *
getNeighbourList(int nodeNo , int totalNodes , int cnt  ){
  int *destarr;
  destarr = new int[cnt];

  for (int i = 0; i < cnt; i++){
     destarr[i] = ((i+1)*nodeNo)%totalNodes + 1;
     if (destarr[i] >= totalNodes  ){
       destarr[i] = destarr[i]%totalNodes;
     }
  }
     
     return destarr;
};

int main(int argc , char *argv[]){

   int id = 0;

#ifdef MPI
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &id);
#endif

  int totalNoOfNodes = 0;
  int noOfNeighbors = 0;
  int arrivalInterval = 0;
  int numLPs = 0;

  ifstream infile("ActiveNetworks.config");

  if (infile.good()){
    infile >> totalNoOfNodes >> arrivalInterval >>noOfNeighbors  >> numLPs ;
     infile.close();
  }
  else {
    cerr << "Cannot find file ActiveNetworks.config" << endl;
    exit(-1);
  }
 
  int numNodesPerLP  ;
  int overflow ;
  int *neighbour = NULL ;
  char *nodeName ;

  numNodesPerLP = totalNoOfNodes /numLPs;
  overflow = totalNoOfNodes  % numLPs ;
  //#ifdef MESSAGE_AGGREGATION
  //  getMessageManagerParameters(argc, argv);
  //#endif
  if ( id == 0){
     //LogicalProcess lp(1+totalNoOfNodes , 1+numNodesPerLP+overflow, numLPs);

    LogicalProcess lp(totalNoOfNodes, numNodesPerLP+overflow, numLPs);

    // CollectStats *statsObject = new CollectStats(totalNoOfNodes+1);
    //lp.registerObject(statsObject);

    ActiveNode1 **node = new ActiveNode1*[numNodesPerLP + overflow] ;
    
    for (int k = 0 ; k <  numNodesPerLP + overflow  ; k ++){
      nodeName  = getNodeName(k) ;
      neighbour = getNeighbourList( k , totalNoOfNodes,noOfNeighbors ) ;
      node[k] = new ActiveNode1( k , nodeName , neighbour , noOfNeighbors );
      lp.registerObject(node[k]);
      delete []neighbour;
    }
    lp.allRegistered();
    lp.simulate();

    for(int index1 = 0 ; index1 < numNodesPerLP + overflow ; index1++){
      delete node[index1];
    }
     delete []node; 
  }
    else if (id > 0 && id < numLPs  ){
      //LogicalProcess lp(1+ totalNoOfNodes , numNodesPerLP  , numLPs);
      LogicalProcess lp(totalNoOfNodes , numNodesPerLP , numLPs);

      // int startId = (1+overflow+numNodesPerLP)+(id-1)*numNodesPerLP ;
       int startId = (overflow+numNodesPerLP)+(id-1)*numNodesPerLP;
       ActiveNode1 **node = new ActiveNode1*[numNodesPerLP] ;
       for (int index2 = 0 ; index2 < numNodesPerLP ; index2++){
         nodeName = getNodeName(startId + index2) ;
         neighbour = getNeighbourList(startId+index2,totalNoOfNodes,noOfNeighbors) ;
         node[index2]=new ActiveNode1( startId+index2,nodeName,neighbour,noOfNeighbors);
         lp.registerObject(node[index2]);
	 delete []neighbour;
       }
   
    lp.allRegistered();
    lp.simulate();
    for ( int index3 = 0  ; index3 < numNodesPerLP  ; index3++){
      delete node[index3];
    }
    delete [] node;
    }
  else {
    cout << "Invalid LP id " << endl ;
    exit(-1);
  }
}











