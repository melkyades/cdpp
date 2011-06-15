#include "ServerState.hh"

ServerState::ServerState() {
  numClientsRecv = 0;
  elapsedTime = 0;
  gen = new MLCG;
}

ServerState&
ServerState::operator=(ServerState& cpyState) {
  (BasicState &)*this = (BasicState &) cpyState;
  this->numClientsRecv = cpyState.numClientsRecv;
  this->elapsedTime = cpyState.elapsedTime;
  *(this->gen) = *cpyState.gen;
  return (*this);
}

void
ServerState::copyState(BasicState *rhs) {
  *this = *((ServerState *) rhs);
}

int
ServerState::getSize() const {
  return sizeof(ServerState);
}
