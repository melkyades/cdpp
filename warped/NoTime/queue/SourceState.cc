#include "SourceState.hh"

SourceState::SourceState() {
  eventsSent = 0; 
  gen = new MLCG;
}

void
SourceState::copyState(BasicState *rhs) {
  (BasicState&)*this = *rhs;
  this->eventsSent = ((SourceState*)rhs)->eventsSent;
  *(this->gen) = *(((SourceState*)rhs)->gen);
}
