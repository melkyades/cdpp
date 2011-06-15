#ifndef ROUTESTATE_HH
#define ROUTESTATE_HH

#include "BasicState.hh"
#include "Types.hh"
#ifdef __GNUG__
#include <MLCG.h>
#else
#include "../rnd/MLCG.h"
#endif

class RouteState : public BasicState {
public:
  RouteState();
  ~RouteState();
  void enQueue(MessEvent *);
  Link *deQueue();

  RouteState& operator=(RouteState&);
  void copyState(BasicState *);

  long long curSchedTime;
  long long utilTime;
  int noMess;
  Link *head;
  Link *tail;
  MLCG *gen;  // Needed for Random number generation
};

#endif
