#ifndef OCEANEVENT_HH
#define OCEANEVENT_HH

#include "BasicEvent.hh" 
#include "SeaTypes.hh" 

class OceanEvent: public BasicEvent {

  

public:  
  friend ostream& operator<<(ostream& os ,  OceanEvent& oe);

  OceanEvent(){
    eventType = 0 ; 
    ct = WATER ; 
    velocity = 0 ; 
    theta = 0 ; 
    entryPoint = C ; 
    sine = 0 ;
    cosine = 0 ; 
    sectorCreatedBy = 0 ;
    creatureId = 0 ; 
    xCord = yCord = 0 ; 
  } ;
  
  OceanEvent(OceanEvent *oe);
  ~OceanEvent(){} ; 

  // this stuff because I use SortedListOfEvents
  OceanEvent *prev ;
  OceanEvent *next ; 

  int eventType ;  //  0=>arrival 1=> exit 2=>kill
  creatureType ct ; 
  float velocity ; // in grid units/sec
  int theta ; 
  int entryPoint ;  
  double  sine , cosine ; 
  int sectorCreatedBy ; 
  int creatureId ; // only for sector 
  int xCord , yCord ; // location 
};
  


inline
int OceanEventCompare( OceanEvent *a , OceanEvent *b){
  
  if ( a->recvTime > b->recvTime ||
       (a->recvTime == b->recvTime && a->eventId  > b->eventId)){
    return 1 ;
  }
  else if ( a->recvTime < b->recvTime ||
      (a->recvTime == b->recvTime && a->eventId  < b->eventId)) {
    return -1 ;
  }
  return 0 ;
}






#endif
