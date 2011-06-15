#ifndef BUFFLESSROUTESTATE_HH
#define BUFFLESSROUTESTATE_HH

#include "BasicState.hh"
#include "Types.hh"
#ifdef __GNUG__
#include <MLCG.h>
#else
#include "../rnd/MLCG.h"
#endif

class BuffLessRouteState : public BasicState {
public:
  BuffLessRouteState();
  ~BuffLessRouteState();

  BuffLessRouteState& operator=(BuffLessRouteState&);
  void copyState(BasicState *);

  long long curSchedTime;
  long long utilTime;
  int noMess;
  MLCG *gen;  // Needed for Random number generation
};

#endif
