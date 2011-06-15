#ifndef ROUTESTATE_HH
#define ROUTESTATE_HH

#include "BasicState.hh"
#include "Types.hh"

class RouteState : public BasicState {
public:
  RouteState(int );
  ~RouteState();
  void enQueue(MessEvent *);
  Link *deQueue();
  MessEvent *getOverflowPackets();

  RouteState& operator=(RouteState&);
  void copyState(BasicState *);

  long long utilTime;
  int noMess, buffSize;
  VTime elapsedTime;
  Link *oflMark,* prevOflMark; // Needed for marking overflow packets
  Link *head;
  Link *tail;
};

#endif
