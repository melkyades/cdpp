
#ifndef ACTIVENODESTATE_HH
#define ACTIVENODESTATE_HH

#include <new.h>

#include "BasicState.hh"
#include "SmartPacket.hh"
#include "BasicEvent.hh"
extern int SmartPacketCompare(SmartPacket *a , SmartPacket *b);
class ActiveNodeState : public BasicState {
public:
  VTime elapsedTime;

ActiveNodeState(){ elapsedTime = 0;};
~ActiveNodeState(){};

  int getSize() const {
    return sizeof(ActiveNodeState);
  }

  void copyState(BasicState *rhs) {
    operator=(*((ActiveNodeState *) rhs));
  }
};
#endif

