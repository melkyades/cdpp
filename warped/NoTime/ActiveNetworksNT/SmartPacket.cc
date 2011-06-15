#ifndef SMARTPACKET_CC
#define SMARTPACKET_CC

#include "SmartPacket.hh"

ostream&
operator<<(ostream& os ,SmartPacket& sp) {

  os << (BasicEvent&) sp << endl ;
  return (os) ;
}

SmartPacket::SmartPacket(SmartPacket *sp) {
  (*(BasicEvent *)this) = *((BasicEvent *) sp);
  eventType = sp->eventType;
  sendEnvType = sp->sendEnvType;
  recvEnvType = sp->recvEnvType;
  sourceId = sp->sourceId;
  sinkId = sp->sinkId;
  forward = sp->forward;
  energy = sp->energy;
  memReq = sp->memReq;
  envToDo = sp->envToDo;
  data = sp->data;
  sourceVendor = sp->sourceVendor; 
  execTime = sp->execTime;
}
#endif
