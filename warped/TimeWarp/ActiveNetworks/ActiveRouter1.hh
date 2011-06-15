#ifndef ACTIVENODE1_HH
#define ACTIVENODE1_HH

#include <fstream.h>
#include "SimulationObj.hh"
#include "ActiveNodeState.hh"
#include "SmartPacket.hh"

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

class ActiveNode1 : public SimulationObj {

public:
  ActiveNode1(int identifier , char *myName , int *dest , int noOfNeighbors);
   //Constructor gives node name such as node1 etc.id is node no an 
   // array of nieghbouring node and the number of neighboring nodes.
   
  ~ActiveNode1(){};
      
   void executeProcess();

   void NodeOS(SmartPacket *recvPacket);   
   
   int validatePacket(SmartPacket *recvPacket);

   void callEnv1(SmartPacket *recvPacket);
 
   void callEnv3(SmartPacket *recvPacket);

   BasicState* allocateState() {
     return new ActiveNodeState;
  }
  
  int totalNoOfNodes;
  int neighbourArray[8]; // array of ids of the neighboring Nodes
  int numNeighbors;
  int messageGenRate;
  int timeToService;
  int statsDest;
};

#endif  







