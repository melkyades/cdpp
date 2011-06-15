#ifndef SOURCESTATE_HH
#define SOURCESTATE_HH

#include "BasicState.hh"

class SourceState : public BasicState {
public: 
  SourceState() { eventsSent = 0;};
  ~SourceState() {};
  
  void copyState(BasicState *rhs) {
    (BasicState&)*this = *rhs;
    this->eventsSent = ((SourceState*)rhs)->eventsSent;
  }
  int eventsSent;
};

#endif
