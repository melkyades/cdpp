#ifndef COLLECTSTATSSTATE_HH
#define COLLECTSTATSSTATE_HH

#include "BasicState.hh"

class CollectStatsState : public BasicState {
public:
  int noMessages;
  float avgMessLatency;
  int noGotMessages;
  int noLostMessages;
  float avgHops;

  CollectStatsState& operator=(CollectStatsState&);
  void copyState(BasicState *);

  CollectStatsState();
  ~CollectStatsState(){};
};

#endif
