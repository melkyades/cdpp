#ifndef STATCOLLECTOR_HH
#define STATCOLLECTOR_HH

#include "SimulationObj.hh"
#include "BasicState.hh"

class StatCollector : public SimulationObj {
public:
  StatCollector(int );
  ~StatCollector(){};

  void executeProcess();
  void finalize();
  BasicState *allocateState();
};

#endif
