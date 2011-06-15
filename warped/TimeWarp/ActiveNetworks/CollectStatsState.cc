#include "CollectStatsState.hh"

CollectStatsState::CollectStatsState() {
  noMessages = 0;
  avgMessLatency = 0.0;
  noLostMessages = 0;
  noGotMessages = 0;
  avgHops = 0.0;
}

CollectStatsState&
CollectStatsState::operator=(CollectStatsState &css) {
  (BasicState &)*this = (BasicState &) css;
  this->noMessages = css.noMessages;
  this->avgMessLatency = css.avgMessLatency;
  this->noGotMessages = css.noGotMessages;
  this->noLostMessages = css.noLostMessages;
  this->avgHops = css.avgHops;
  return *this;
}

void
CollectStatsState::copyState(BasicState *cpState) {
  *this = *((CollectStatsState*)cpState);
}
