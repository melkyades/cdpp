#ifndef QUEUESTATE_HH
#define QUEUESTATE_HH

#include "Types.hh"
#include "BasicState.hh"

class QueueState : public BasicState {
public:
  QueueState(int );
  ~QueueState();
  void enQueue(int eventTime);
  Link *deQueue();

  QueueState& operator=(QueueState&);
  void copyState(BasicState *);

  bool *serverIdle;
  int noServ;    // Needed for state saving
  Link *head;    // Head and tail of the list of clients
  Link *tail;
};

#endif



