#include "NodeState.hh"

NodeState::NodeState(int totMess) {
  noMess = totMess;
  gen = new MLCG;
}

NodeState&
NodeState::operator=(NodeState &rhs) {
  (BasicState&)*this = (BasicState&)rhs;
  this->noMess = rhs.noMess;
  *(this->gen) = *rhs.gen;
  return *this;
}

void
NodeState::copyState(BasicState *cpyState) {
  *this = *((NodeState*)cpyState);
}
