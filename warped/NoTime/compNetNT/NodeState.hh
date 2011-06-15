#ifndef NODESTATE_HH
#define NODESTATE_HH

#include "BasicState.hh"

class NodeState : public BasicState {
public:
  int noMess;
  VTime elapsedTime;

  NodeState (int totMess);
  ~NodeState () {}
};

#endif
