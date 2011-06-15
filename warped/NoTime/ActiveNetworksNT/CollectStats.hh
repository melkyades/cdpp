#ifndef COLLECTSTATS_HH
#define COLLECTSTATS_HH

#include "SimulationObj.hh"
#include "BasicState.hh"
#include "CollectStatsState.hh"

class CollectStats : public SimulationObj {
public:
  CollectStats(int );
  ~CollectStats(){};

  void executeProcess();
  void finalize();
  BasicState *allocateState() {
    return new CollectStatsState();
  };
};

#endif
