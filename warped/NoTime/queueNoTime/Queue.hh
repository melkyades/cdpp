#ifndef QUEUE_HH
#define QUEUE_HH

#include "SimulationObj.hh"

class Queue : public SimulationObj {
private:
  int *servIds; // Ids of the servers to send the events to
  int noServ;
  int serverToSend();
  int causalViol;
public:
  Queue(int, int, int);
  ~Queue(); 

  void causalityViolationHandler(BasicEvent*);
  void executeProcess();
  BasicState* allocateState();
};

#endif
