#ifndef SERVERSTATE_HH
#define SERVERSTATE_HH

#include "BasicState.hh"

class ServerState : public BasicState {
public:
  ServerState();
  ~ServerState(){};
  
  VTime elapsedTime;   // Time elapsed from beginning of the server

  ServerState& operator=(ServerState&);
  void copyState(BasicState *);
  int getSize() const;
};
#endif
