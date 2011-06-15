#ifndef NODESTATE_HH
#define NODESTATE_HH

#include "BasicState.hh"
#ifdef __GNUG__
#include <MLCG.h>
#else
#include "../rnd/MLCG.h"
#endif

class NodeState : public BasicState {
public:
  int noMess;
  MLCG *gen;

  NodeState& operator=(NodeState&);
  void copyState(BasicState *);

  NodeState (int totMess);
  ~NodeState () {
    delete gen;
  }
};

#endif

