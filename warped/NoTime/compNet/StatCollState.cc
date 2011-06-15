#include "StatCollState.hh"

StatCollState::StatCollState() {
  noMessages = 0;
  avgMessDelay = 0.0;
  noLostMess = 0;
}

StatCollState&
StatCollState::operator=(StatCollState &rhs) {
  (BasicState &)*this = (BasicState &) rhs;
  this->noMessages = rhs.noMessages;
  this->avgMessDelay = rhs.avgMessDelay;
  this->noLostMess = rhs.noLostMess;
  return *this;
}

void
StatCollState::copyState(BasicState *cpyState) {
  *this = *((StatCollState*)cpyState);
}



