#ifndef TABLEEVENT_HH
#define TABLEEVENT_HH

#include "BasicEvent.hh"
#include "tableTypes.hh"

class tableEvent : public BasicEvent {


public:
  friend ostream& operator<<(ostream& os , tableEvent& te);

  tableEvent(){
    eventType = 0 ;
    velocity = 0;
    theta = 0 ;
    entryPoint = C ;
    sine = 0 ;
    cosine = 0 ;
    sectorCreatedBy = 0 ;
    puckId = 0 ;
    xCord = yCord = 0 ;
  };

tableEvent(tableEvent *te);
  ~tableEvent(){} ;

   tableEvent *prev ;
   tableEvent *next ;

   int eventType ;
   float velocity ;
   double theta ;
   int entryPoint ;
   double sine , cosine ;
   int sectorCreatedBy ;
   int puckId;
   int xCord , yCord ;
};
 
inline
int tableEventCompare(tableEvent *a , tableEvent *b){

  if (a->recvTime > b->recvTime || (a->recvTime == b->recvTime && a->eventId > b->eventId)){
         return 1 ;
  }
  else if (a->recvTime < b->recvTime || (a->recvTime == b->recvTime && a->eventId < b->eventId)) {
       return -1 ;
  }
  return 0 ;
}

#endif




