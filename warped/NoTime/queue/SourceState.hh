#ifndef SOURCESTATE_HH
#define SOURCESTATE_HH

#include "BasicState.hh"
#ifdef __GNUG__
#include <MLCG.h>
#else
#include "../rnd/MLCG.h"
#endif

class SourceState : public BasicState {
public: 
  SourceState() ;
  ~SourceState() {delete gen;};
  
  MLCG *gen;
  void copyState(BasicState *rhs);
  int eventsSent;
};

#endif
