#ifndef SERVERSTATE_HH
#define SERVERSTATE_HH

#include "BasicState.hh"
#ifdef __GNUG__
#include <MLCG.h>
#else
#include "../rnd/MLCG.h"
#endif

class ServerState : public BasicState {
public:
  ServerState();
  ~ServerState(){ delete gen;};
  
  MLCG *gen;
  VTime elapsedTime;   // Time elapsed from beginning of the server
  int numClientsRecv;  // Number of clients serviced from the queue

  ServerState& operator=(ServerState&);
  void copyState(BasicState *);
  int getSize() const;
};
#endif
