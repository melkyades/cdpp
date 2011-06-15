#ifndef SMARTPACKET_HH
#define SMARTPACKET_HH

#include "BasicEvent.hh"

class SmartPacket : public BasicEvent {


public:
  friend ostream& operator<<(ostream& os , SmartPacket& sp);

  SmartPacket(){
    eventType = 0;
    sendEnvType = 0;
    recvEnvType = 0;
    sourceId = 0;
    sinkId = 0;
    startTime = 0;
    forward = 0;
    energy = 0;
    memReq = 0;
    envToDo = 0;
    data = 0;
    sourceVendor = 0;
    execTime = 0;
  };

SmartPacket(SmartPacket *sp);
  ~SmartPacket(){} ;

   SmartPacket *prev ;
   SmartPacket *next ;

   int eventType;
   int sendEnvType;
   int recvEnvType;
   int sourceId;
   int sinkId;
   int startTime;
   int forward;
   int energy;
   int memReq;
   int envToDo;
   int data;
   int sourceVendor;
   int execTime;
};
 
inline
int SmartPacketCompare(SmartPacket *a , SmartPacket *b){

  if (a->recvTime > b->recvTime || (a->recvTime == b->recvTime && a->eventId > b->eventId)){
         return 1 ;
  }
  else if (a->recvTime < b->recvTime || (a->recvTime == b->recvTime && a->eventId < b->eventId)) {
       return -1 ;
  }
  return 0 ;
}

#endif




