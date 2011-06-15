#ifndef QUEUESTATE_HH
#define QUEUESTATE_HH

#include "Types.hh"
#include "BasicState.hh"

typedef struct info {
  bool servIdle;
  VTime idleTime;
}Info;

class QueueState : public BasicState {
public:
  QueueState(int );
  ~QueueState();
  void enQueue(int eventTime);
  Link *deQueue();

  QueueState& operator=(QueueState&);
  void copyState(BasicState *);

  bool allServerBusy;
  Info *servInfo;
  int noServ;    // Needed for state saving
  Link *head;    // Head and tail of the list of clients
  Link *tail;
};

#endif



