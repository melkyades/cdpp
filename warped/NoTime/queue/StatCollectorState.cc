#include "StatCollectorState.hh"

StatCollectorState::StatCollectorState() {
  noClientsRecv = 0;
  avgArrTime = 0.0;
  utilTime = 0;
  avgServTime = 0.0;
  throughPut = 0.0;
  avgSysTime = 0.0;
  //  utilization = 0.0;
}

StatCollectorState&
StatCollectorState::operator=(StatCollectorState& cpyState) {
  (BasicState &)*this = (BasicState &) cpyState;
  this->noClientsRecv = cpyState.noClientsRecv;
  this->avgArrTime = cpyState.avgArrTime;
  this->avgServTime = cpyState.avgServTime;
  this->avgSysTime = cpyState.avgSysTime;
  this->throughPut = cpyState.throughPut;
  this->utilTime = cpyState.utilTime;
  //  this->utilization = cpyState.utilization;
  return *this;
}

void
StatCollectorState::copyState(BasicState *rhs) {
  *this = *((StatCollectorState *) rhs);
}
