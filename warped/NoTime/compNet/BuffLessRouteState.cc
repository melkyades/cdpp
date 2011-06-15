#include "BuffLessRouteState.hh"

BuffLessRouteState::BuffLessRouteState() {
  noMess = 0;
  utilTime = 0;
  curSchedTime = 0;
  gen = new MLCG;
}

BuffLessRouteState::~BuffLessRouteState() {
  delete gen;
}

BuffLessRouteState&
BuffLessRouteState::operator=(BuffLessRouteState &rhs) {
  (BasicState &)*this = (BasicState &) rhs;
  noMess = rhs.noMess;
  utilTime = rhs.utilTime;
  curSchedTime = rhs.curSchedTime;
  *(this->gen) = *rhs.gen;
}

void
BuffLessRouteState::copyState(BasicState *cpyState) {
  *this = *((BuffLessRouteState*)cpyState);
}


