#ifndef INITOBJECTSTATE_HH
#define INITOBJECTSTATE_HH

#include "BasicState.hh"

class InitObjectState : public BasicState {
  
public:
  InitObjectState(){};
  ~InitObjectState(){};
   
  int getSize() const {
    return sizeof(InitObjectState);
  }

  void copyState(BasicState *rhs) {
    BasicState::copyState(rhs);
  }
  
};

#endif
