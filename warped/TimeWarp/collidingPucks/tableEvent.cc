#ifndef TABLEEVENT_CC
#define TABLEEVENT_CC

#include "tableEvent.hh"

ostream&
operator<<(ostream& os ,tableEvent& te) {

  os << "eventType :" << te.eventType ;
  os << "velocity " << te.velocity << "theta " << te.theta ;
  os << " entryPoint " <<te.entryPoint << " createdBy " << te.sectorCreatedBy ;
  os << " puck id " << te.puckId << "at " << endl ;
  os << "(" << te.xCord << "," << te.yCord << ")" << endl ;
  os << (BasicEvent&) te << endl ;
  return (os) ;
}

tableEvent::tableEvent(tableEvent *te) {
  //  cout << *te << endl;
  (*(BasicEvent *)this) = *((BasicEvent *) te);
  eventType = te->eventType ;
  velocity = te->velocity ;
  theta = te->theta ;
  entryPoint = te->entryPoint ;
  sine = te->sine ;
  cosine = te->cosine ;
  sectorCreatedBy = te->sectorCreatedBy  ;
  puckId = te->puckId ;
  xCord = te->xCord ;
  yCord = te->yCord  ;

}
#endif
