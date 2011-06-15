#include "ServerState.hh"

ServerState::ServerState() {
  elapsedTime = 0;
}

ServerState&
ServerState::operator=(ServerState& cpyState) {
  (BasicState &)*this = (BasicState &) cpyState;
  this->elapsedTime = cpyState.elapsedTime;
}

void
ServerState::copyState(BasicState *rhs) {
  *this = *((ServerState *) rhs);
}

int
ServerState::getSize() const {
  return sizeof(ServerState);
}
