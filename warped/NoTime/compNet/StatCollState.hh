#ifndef STATCOLLSTATE_HH
#define STATCOLLSTATE_HH

#include "BasicState.hh"

class StatCollState : public BasicState {
public:
  int noMessages;
  float avgMessDelay;
  int noLostMess;
  
  StatCollState& operator=(StatCollState&);
  void copyState(BasicState *);

  StatCollState();
  ~StatCollState(){};
};

#endif
