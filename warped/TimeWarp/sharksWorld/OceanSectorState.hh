#ifndef OCEANSECTORSTATE_HH
#define OCEANSECTORSTATE_HH

#include <new.h>

#include "BasicState.hh"
#include "SortedListOfEvents.hh"
#include "OceanEvent.hh"
#include "BasicEvent.hh" 
extern int OceanEventCompare(OceanEvent *a , OceanEvent*b);
class OceanSectorState : public BasicState {
public:
  
  OceanSectorState();
  ~OceanSectorState() ;
  // copy constructor 
  OceanSectorState(const OceanSectorState&);
  OceanSectorState& operator=(OceanSectorState&);

  SortedListOfEvents<OceanEvent>  fishList ;
  SortedListOfEvents<OceanEvent> sharkList ;
 
  int getSize() const {
    return sizeof(OceanSectorState);
  }

  void copyState(BasicState *rhs) {
    operator=(*((OceanSectorState *) rhs));
  }

};
#endif 















