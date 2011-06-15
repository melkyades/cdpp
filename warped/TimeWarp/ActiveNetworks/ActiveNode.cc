#ifndef ACTIVENODE_CC
#define ACTIVENODE_CC

#define MAXMEM 40
#define NumEnvs 3

#include "ActiveNode.hh"

int lastSent;
int MaxHops = 12;

ActiveNode::ActiveNode(int identifier , char *myName , int *dest , int noOfNeighbors){

  name = new char[strlen(myName)+1];
  strcpy(name,myName);

  state->nameOfObject = new char[strlen(myName)+1];
  strcpy(state->nameOfObject,myName);

  id = identifier ;
  numNeighbors = noOfNeighbors;

  timeToService = 0;

  ifstream inputFile("ActiveNetworks.config");
  inputFile >> totalNoOfNodes >> messageGenRate;

  statsDest = totalNoOfNodes;

  for (int i = 0; i < noOfNeighbors; i++){
     neighbourArray[i] = dest[i];
  }

  delete [] myName;
}

void 
ActiveNode::executeProcess(){
  
SmartPacket *recvPacket = (SmartPacket *)getEvent();
if(recvPacket->eventType == 1){ //1 means event is a SmartPacket
  if(recvPacket->sinkId == id){ // Packet has reached destination
#ifdef PRINTSTUFF
    cout << "Packet from source " << recvPacket->sourceId 
         << " reached destination " << id
         << "at time = " << recvPacket->recvTime << endl;
#endif
    SmartPacket *statsPacket = (SmartPacket *)new char[sizeof(SmartPacket)];
    new (statsPacket) SmartPacket(recvPacket);
    statsPacket->dest = statsDest;
    statsPacket->energy = MaxHops - (recvPacket->energy - 1);
    sendEvent(statsPacket);
  }
  else {
    NodeOS(recvPacket); // Not destination pass control to the NodeOS
  }
}
else {
  Tgen(recvPacket); // 0 is a traffic generation event
  SmartPacket *statsPacket = (SmartPacket *)new char[sizeof(SmartPacket)];
  new (statsPacket) SmartPacket(recvPacket);
  statsPacket->dest = statsDest;
  statsPacket->eventType = 0;
  sendEvent(statsPacket);
}
}

void
ActiveNode::NodeOS(SmartPacket *recvPacket){

  int test = validatePacket(recvPacket); // packet validation

if (test == 0){
#ifdef PRINTSTUFF
  cout << "Packet failed security check at node " << id
       << " at time = " << recvPacket->recvTime << endl;
#endif
  SmartPacket *statsPacket = (SmartPacket *)new char[sizeof(SmartPacket)];
  new (statsPacket) SmartPacket(recvPacket);
  statsPacket->dest = statsDest;
  sendEvent(statsPacket);

}

else {
#ifdef PRINTSTUFF
  cout << "Packet passed security check at node " << id
       << " at time = " << recvPacket->recvTime << endl;
#endif 
  //Pass packet to correct environment
  //This node has only envs 1 and 2
  if(recvPacket->recvEnvType == 1){
    callEnv1(recvPacket);
  }

  else if(recvPacket->recvEnvType == 2){
     callEnv2(recvPacket);
  }
  
  else{ // Desired env is not there at node - forward or discard packet
    if (recvPacket->forward == 0){
#ifdef PRINTSTUFF
      cout << "Packet deleted at node " << id
           << " at time = " << recvPacket->recvTime 
           << " as requested env was not avialbale and forwrding not requested"
           << endl;
#endif
      SmartPacket *statsPacket = (SmartPacket *)new char[sizeof(SmartPacket)];
      new (statsPacket) SmartPacket(recvPacket);
      statsPacket->dest = statsDest;
      sendEvent(statsPacket);
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
        int latency = (int)(abs(id-newPacket->dest))/(totalNoOfNodes*.1) + 1;
         if(timeToService <= recvPacket->recvTime){
          newPacket->recvTime = recvPacket->recvTime+recvPacket->execTime+latency;
        }
        else {
          newPacket->recvTime = timeToService + recvPacket->execTime + latency;
        }
        timeToService = recvPacket->recvTime + recvPacket->execTime;
        newPacket->size = sizeof(SmartPacket);
        sendEvent(newPacket);
      }
      else {
#ifdef PRINTSTUFF
        cout << " Packet ranout of hops/energy" << endl;
#endif
        SmartPacket *statsPacket= (SmartPacket *)new char[sizeof(SmartPacket)];
        new (statsPacket) SmartPacket(recvPacket);
        statsPacket->dest = statsDest;
        sendEvent(statsPacket);

      }
      
    }
  }
}
}

int
ActiveNode::validatePacket(SmartPacket *recvPacket){
  //This is the packet validation guy 0 - failed 1 - passed
  if (recvPacket->sourceId != recvPacket->sinkId) {
    if (recvPacket->sourceVendor == 1){
      if (recvPacket->memReq < MAXMEM){
        return 1;
      }
      else {
        return 0;
      }
    }
    else if (recvPacket->sourceVendor == 2){
      if (recvPacket->memReq < MAXMEM/2){
        return 1;
      }
      else {
        return 0;
      }
    }
    else {
      if (recvPacket->memReq < MAXMEM/3){
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
ActiveNode::callEnv1(SmartPacket *recvPacket){
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
    if(timeToService <= recvPacket->recvTime){
      newPacket->recvTime = recvPacket->recvTime + recvPacket->execTime + latenc
y;
    }
    else {
      newPacket->recvTime = timeToService + recvPacket->execTime + latency;
    }
    sendEvent(newPacket);
  }
  else {
#ifdef PRINTSTUFF
    cout << " Packet ranout of energy/hops" << endl;
#endif
    SmartPacket *statsPacket = (SmartPacket *)new char[sizeof(SmartPacket)];
    new (statsPacket) SmartPacket(recvPacket);
    statsPacket->dest = statsDest;
    sendEvent(statsPacket);
  }
}

void
ActiveNode::callEnv2(SmartPacket *recvPacket){
  //This guy is processing env2
  // This guy does a congestion reduction routing.
  if (recvPacket->energy > 0){
  timeToService += recvPacket->execTime;
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
  newPacket->dest = neighbourArray[(lastSent+1)%numNeighbors];
  int latency = (int)((abs(id-newPacket->dest))/(totalNoOfNodes*.1)) + 1;
  if(timeToService <= recvPacket->recvTime){
      newPacket->recvTime = recvPacket->recvTime + recvPacket->execTime + latency;
    }
    else {
      newPacket->recvTime = timeToService + recvPacket->execTime + latency;
    }

  sendEvent(newPacket);
  timeToService = recvPacket->recvTime + recvPacket->execTime; 
  lastSent = (lastSent+1)%numNeighbors;
  }
  else {
#ifdef PRINTSTUFF
    cout << " Packet ranout of hops/energy " << endl;
#endif
    SmartPacket *statsPacket = (SmartPacket *)new char[sizeof(SmartPacket)];
    new (statsPacket) SmartPacket(recvPacket);
    statsPacket->dest = statsDest;
    sendEvent(statsPacket);
  }
}

void
ActiveNode::Tgen(SmartPacket *recvPacket){
  //This guy is the traffic generator
  SmartPacket *genPacket = (SmartPacket *) new char [sizeof(SmartPacket)];
  new (genPacket)SmartPacket(recvPacket);
  genPacket->eventType = 0;
  genPacket->dest = id;
  genPacket->recvTime = recvPacket->recvTime + messageGenRate; 
  genPacket->size = sizeof(SmartPacket);
  sendEvent(genPacket);

  SmartPacket *newPacket = (SmartPacket *) new char [sizeof(SmartPacket)];
  new (newPacket) SmartPacket(recvPacket);
  newPacket->eventType = 1;
  newPacket->sendEnvType = (id + recvPacket->recvTime)%(NumEnvs+1);
  newPacket->recvEnvType = (id + recvPacket->recvTime)%(NumEnvs+1);
  newPacket->sourceId = id;
  newPacket->sinkId = (getLVT()*id)%totalNoOfNodes + 1;
  newPacket->forward = getLVT()%2;
  newPacket->energy = 8;
  newPacket->memReq = newPacket->sinkId%15;
  newPacket->envToDo = getLVT()%2;
  newPacket->data = 256;
  newPacket->sourceVendor = 1;
  newPacket->execTime = (newPacket->data - newPacket->memReq)%15 + 4;
  newPacket->recvTime = recvPacket->recvTime + 1;
  newPacket->dest = id;
  sendEvent(newPacket);
}

#endif







