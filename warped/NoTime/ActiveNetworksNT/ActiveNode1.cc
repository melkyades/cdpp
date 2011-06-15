#ifndef ACTIVENODE1_CC
#define ACTIVENODE1_CC

#define NumEnvs 3

#include "ActiveNode1.hh"
#include "ActiveNodeState.hh"
#include "LogicalProcess.hh"

int lastSent;
int MAXMEM = 50;
int MaxHops = 12;

ActiveNode1::ActiveNode1(int identifier , char *myName , int *dest , int noOfNeighbors){

  name = new char[strlen(myName)+1];
  strcpy(name,myName);

  id = identifier ;
  numNeighbors = noOfNeighbors;

  timeToService = 0;

  ifstream inputFile("ActiveNetworks.config");
  inputFile >> totalNoOfNodes >> messageGenRate;

  //statsDest = totalNoOfNodes + 1;

  for (int i = 0; i < noOfNeighbors; i++){
     neighbourArray[i] = dest[i];
  }

  delete [] myName;
}

void
ActiveNode1::initialize() {
  SmartPacket *genPacket = (SmartPacket *) new char [sizeof(SmartPacket)];
  new (genPacket) SmartPacket();
  genPacket->eventType = 0;
  genPacket->dest = id;
  genPacket->recvTime = getLVT() + 1; 
  genPacket->size = sizeof(SmartPacket);
  sendEvent(genPacket);
}
void 
ActiveNode1::executeProcess(){
  
SmartPacket *recvPacket = (SmartPacket *)getEvent();
if(recvPacket != NULL && getLVT() != LogicalProcess::SIMUNTIL ){
 if(recvPacket->eventType == 1){ //The event is a smart Packet
   if(recvPacket->sinkId == id){ // The packet has reached destination
#ifdef PRINTSTUFF
    cout << "Packet from source " << recvPacket->sourceId 
         << " reached destination " << id
         << "at time = " << recvPacket->recvTime << endl;
#endif
    //SmartPacket *statsPacket = (SmartPacket *)new char[sizeof(SmartPacket)];
    //new (statsPacket) SmartPacket(recvPacket);
    //statsPacket->dest = statsDest;
    //statsPacket->energy = MaxHops - (recvPacket->energy - 1);
    //sendEvent(statsPacket);
  }
  else {
    NodeOS(recvPacket); // If its not destination pass control to NodeOS
  }
}
else {// eventType = 0 means it a traffic generation event
  Tgen(recvPacket); // Call traffic generator
  //SmartPacket *statsPacket = (SmartPacket *)new char[sizeof(SmartPacket)];
  //new (statsPacket) SmartPacket(recvPacket);
  //statsPacket->dest = statsDest;
  //statsPacket->eventType = 0;
  //sendEvent(statsPacket);
}
}
}

void
ActiveNode1::NodeOS(SmartPacket *recvPacket){
  //This is the NodeOS function
  int test = validatePacket(recvPacket); //Validate packet

if (test == 0){
#ifdef PRINTSTUFF
  cout << "Packet failed security check at node " << id
       << " at time = " << recvPacket->recvTime << endl;
#endif
  /* SmartPacket *statsPacket = (SmartPacket *)new char[sizeof(SmartPacket)];
  new (statsPacket) SmartPacket(recvPacket);
  statsPacket->dest = statsDest;
  sendEvent(statsPacket); */
}

else {
#ifdef PRINTSTUFF
  cout << "Packet passed security check at node " << id
       << " at time = " << recvPacket->recvTime << endl;
#endif
  // check and pass packet to correct environment 
  // This node only has envs 1 and 3
  if(recvPacket->recvEnvType == 1){ 
    callEnv1(recvPacket);
  }

  else if(recvPacket->recvEnvType == 3){
     callEnv3(recvPacket);
  }
  
  else{ // Desired env is not there at node - forward or discard packet
    if (recvPacket->forward == 0){
#ifdef PRINTSTUFF
      cout << "Packet deleted at node " << id
           << " at time = " << recvPacket->recvTime 
           << " as requested env was not avialbale and forwrding not requested"
           << endl;
#endif
      /* SmartPacket *statsPacket = (SmartPacket *)new char[sizeof(SmartPacket)];
      new (statsPacket) SmartPacket(recvPacket);
      statsPacket->dest = statsDest;
      sendEvent(statsPacket); */
    }
   
    else{
      if(recvPacket->energy > 0){
#ifdef PRINTSTUFF
        cout << "Packet forwarded at node " << id
             << " at time = " << recvPacket->recvTime 
             << endl;
#endif
       
        SmartPacket *newPacket = (SmartPacket *)new char[sizeof(SmartPacket)];
        new (newPacket) SmartPacket(recvPacket);
        newPacket->eventType = 1;
        newPacket->sendEnvType = 0;
        newPacket->recvEnvType = recvPacket->recvEnvType;
        newPacket->energy = recvPacket->energy - 1;
        newPacket->dest = neighbourArray[getLVT()%numNeighbors];
        int latency = (int)((abs(id-newPacket->dest))/(totalNoOfNodes*.1)) + 1;
        if(((ActiveNodeState *)state->current)->elapsedTime <= recvPacket->recvTime){
          newPacket->recvTime = recvPacket->recvTime+recvPacket->execTime+latency;
        }
        else {
          newPacket->recvTime = ((ActiveNodeState *)state->current)->elapsedTime + recvPacket->execTime + latency;
	}
        ((ActiveNodeState *)state->current)->elapsedTime = recvPacket->recvTime + recvPacket->execTime;
        newPacket->size = sizeof(SmartPacket);
	if(newPacket->recvTime <= LogicalProcess::SIMUNTIL){
	  sendEvent(newPacket);
	}
      }
      else {
#ifdef PRINTSTUFF
        cout << " Packet ranout of hops/energy" << endl;
#endif
        /*SmartPacket *statsPacket= (SmartPacket *)new char[sizeof(SmartPacket)];
        new (statsPacket) SmartPacket(recvPacket);
        statsPacket->dest = statsDest;
        sendEvent(statsPacket); */
      }
      
    }
  }
}
}

int
ActiveNode1::validatePacket(SmartPacket *recvPacket){
  //This guy does a security check on the packet 0- failed 1 - passed

  if (recvPacket->sourceId != recvPacket->sinkId) {
    if (recvPacket->sourceVendor == 2){
      if (recvPacket->memReq < MAXMEM){
        return 1;
      }
      else {
        return 0;
      }
    }
    else if (recvPacket->sourceVendor == 1){
      if (recvPacket->memReq < MAXMEM/2){
        return 1;
      }
      else {
        return 0;
      }
    }
    else {
      if (recvPacket->memReq < MAXMEM/2){
        return 1;
      }
      else {
        return 0;
      }
    }
  }
  else {
    return 0;
  }
}

void
ActiveNode1::callEnv1(SmartPacket *recvPacket){
  //This guy is processing environment 1 
 //This guy adds/reduces data from a packet depending on its size
  //The Memory available at the node is also accordingly changed.
  if(recvPacket->energy > 0){
 
    SmartPacket *newPacket = (SmartPacket *) new char [sizeof(SmartPacket)];
    new (newPacket) SmartPacket(recvPacket);
    newPacket->eventType = 1;
    newPacket->sendEnvType = newPacket->recvEnvType = 1;
    newPacket->sourceId = recvPacket->sourceId;
    newPacket->sinkId = recvPacket->sinkId;
    newPacket->forward = recvPacket->forward;
    newPacket->energy = recvPacket->energy - 1;
    newPacket->envToDo = recvPacket->envToDo;
   
    if(recvPacket->data < 500){
      newPacket->data = recvPacket->data + 50;
      MAXMEM = MAXMEM + 2;
      newPacket->memReq = recvPacket->memReq + 2;
    }
    else {
      newPacket->data = recvPacket->data - 50;
      MAXMEM = MAXMEM - 2;
      newPacket->memReq = recvPacket->memReq - 2;
    }
    newPacket->sourceVendor = recvPacket->sourceVendor;
    newPacket->execTime = newPacket->execTime;
    newPacket->dest = neighbourArray[getLVT()%numNeighbors];
    int latency = (int)((abs(id-newPacket->dest))/(totalNoOfNodes*.1)) + 1;
    if(((ActiveNodeState *)state->current)->elapsedTime <= recvPacket->recvTime){
      newPacket->recvTime = recvPacket->recvTime + recvPacket->execTime + latency;
    }
    else { 
      newPacket->recvTime = ((ActiveNodeState *)state->current)->elapsedTime + recvPacket->execTime + latency;
    }
    newPacket->size = sizeof(SmartPacket);
    ((ActiveNodeState *)state->current)->elapsedTime = recvPacket->recvTime + recvPacket->execTime;
    if(newPacket->recvTime <= LogicalProcess::SIMUNTIL){
      sendEvent(newPacket);
    }
  }
  else {
#ifdef PRINTSTUFF
    cout << " Packet ranout of energy/hops" << endl;
#endif
    /*SmartPacket *statsPacket = (SmartPacket *)new char[sizeof(SmartPacket)];
    new (statsPacket) SmartPacket(recvPacket);
    statsPacket->dest = statsDest;
    sendEvent(statsPacket); */
  }
}

void
ActiveNode1::callEnv3(SmartPacket *recvPacket){
  //This guy is processing env 3
  //This guy changes mem val based on sender and reciever 
  if (recvPacket->energy > 0){
  
     SmartPacket *newPacket = (SmartPacket *) new char [sizeof(SmartPacket)];
     new (newPacket) SmartPacket(recvPacket);
     newPacket->eventType = 1;
     newPacket->sendEnvType = newPacket->recvEnvType = 2;
     newPacket->sourceId = recvPacket->sourceId;
     newPacket->sinkId = recvPacket->sinkId;
     newPacket->forward = recvPacket->forward;
     newPacket->energy = recvPacket->energy - 1;
     newPacket->memReq = recvPacket->memReq;
     newPacket->envToDo = recvPacket->envToDo;
     newPacket->data = recvPacket->data;
     newPacket->sourceVendor = recvPacket->sourceVendor;
     newPacket->execTime = newPacket->execTime;
     newPacket->dest = neighbourArray[recvPacket->sinkId%numNeighbors];
     int latency = (int)((abs(id-newPacket->dest))/(totalNoOfNodes*.1)) + 1;
     if(((ActiveNodeState *)state->current)->elapsedTime <= recvPacket->recvTime){
      newPacket->recvTime = recvPacket->recvTime + recvPacket->execTime + latency;
    }
    else { 
      newPacket->recvTime = ((ActiveNodeState *)state->current)->elapsedTime + recvPacket->execTime + latency;
    }
    newPacket->size = sizeof(SmartPacket);
    if(newPacket->recvTime <= LogicalProcess::SIMUNTIL){
      sendEvent(newPacket);
    }
    ((ActiveNodeState *)state->current)->elapsedTime = recvPacket->recvTime + recvPacket->execTime;
   
    if (recvPacket->sourceId < recvPacket->sinkId) {
      MAXMEM--;
    }
    else {
      MAXMEM++;
    }
  }
  else {
#ifdef PRINTSTUFF
    cout << " Packet ranout of hops/energy " << endl;
#endif
    /*SmartPacket *statsPacket = (SmartPacket *)new char[sizeof(SmartPacket)];
    new (statsPacket) SmartPacket(recvPacket);
    statsPacket->dest = statsDest;
    sendEvent(statsPacket); */
  }
  
}
void
ActiveNode1::Tgen(SmartPacket *recvPacket){

  SmartPacket *genPacket = (SmartPacket *) new char [sizeof(SmartPacket)];
  new (genPacket) SmartPacket(recvPacket);
  genPacket->eventType = 0;
  genPacket->dest = id;
  genPacket->recvTime = recvPacket->recvTime + messageGenRate; 
  genPacket->size = sizeof(SmartPacket);
  if(genPacket->recvTime <= LogicalProcess::SIMUNTIL){
    sendEvent(genPacket);
  }
  SmartPacket *newPacket = (SmartPacket *) new char [sizeof(SmartPacket)];
  new (newPacket) SmartPacket(recvPacket);
  newPacket->eventType = 1;
  newPacket->sendEnvType = (recvPacket->recvTime)%(NumEnvs+1);
  newPacket->recvEnvType = (recvPacket->recvTime)%(NumEnvs+1);
  newPacket->sourceId = id;
  newPacket->sinkId = (getLVT()*id)%totalNoOfNodes + 1;
  newPacket->forward = getLVT()%4;
  newPacket->energy = MaxHops;
  newPacket->memReq = newPacket->sinkId%25;
  newPacket->envToDo = getLVT()%2;
  newPacket->data = 256;
  newPacket->sourceVendor = 2;
  newPacket->execTime = (newPacket->data - newPacket->memReq)%15 + 4;
  newPacket->recvTime = recvPacket->recvTime + 1;
  newPacket->dest = id;
  newPacket->size = sizeof(SmartPacket);
  if(newPacket->recvTime <= LogicalProcess::SIMUNTIL){
    sendEvent(newPacket);
  }
  ((ActiveNodeState *)state->current)->elapsedTime = newPacket->recvTime;
}

#endif







