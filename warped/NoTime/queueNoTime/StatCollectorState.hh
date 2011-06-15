#ifndef STATCOLLECTORSTATE_HH
#define STATCOLLECTORSTATE_HH

#include "BasicState.hh"

class StatCollectorState : public BasicState {
public:
  StatCollectorState();
  ~StatCollectorState() {};
  
  StatCollectorState& operator=(StatCollectorState&);
  void copyState(BasicState *);

  int noClientsRecv;
  float avgArrTime;
  float throughPut;
  
  unsigned long long utilTime;
  //  float utilization;
  float avgServTime;
  
  float avgSysTime;
};

#endif
