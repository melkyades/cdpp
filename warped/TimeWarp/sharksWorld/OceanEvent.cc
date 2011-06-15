#ifndef OCEANEVENT_CC 
#define OCEANEVENT_CC 

#include "OceanEvent.hh" 

ostream&
operator<<(ostream& os ,OceanEvent& oe){
  
  os << "Creature: " ;
  if (oe.ct == 0 ){
    os <<"SHARK" ; 
  }
  else {
    os << "FISH" ; 
  }
  os << endl ;
  os << "eventType :" << oe.eventType ;
  os << " velocity " << oe.velocity << " theta " << oe.theta ;
  os << " entryPoint " << oe.entryPoint << " createdBy " << oe.sectorCreatedBy ;
  os << " creature id " << oe.creatureId << "at " << endl ;
  os << "(" << oe.xCord << "," << oe.yCord << ")" << endl ;
  os << (BasicEvent&) oe << endl ;
  return (os) ;
}

OceanEvent::OceanEvent( OceanEvent *oe){
  //  cout << *oe << endl;
  eventType = oe->eventType ; 
  ct = oe->ct ; 
  velocity = oe->velocity ; 
  theta = oe->theta ; 
  entryPoint = oe->entryPoint ; 
  sine = oe->sine ;
  cosine = oe->cosine ; 
  sectorCreatedBy = oe->sectorCreatedBy  ;
  creatureId = oe->creatureId ; 
  xCord = oe->xCord ;  
  yCord = oe->yCord  ; 
  (*(BasicEvent *)this) = *((BasicEvent *) oe);
}
#endif 


