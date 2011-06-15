#include "Server.hh"
#include "ServerState.hh"

Server::Server(int myId, int destId1, int destId2) {
  id = myId;
  qDest = destId1;
  statDest = destId2;
}

Server::~Server() {
  delete numGen;
}

void
Server::initialize() {
  delete ((ServerState*)state->current)->gen;
  int *ptr = new int;
  ((ServerState*)state->current)->gen = new MLCG((int)ptr, ((int)ptr+6));
  delete ptr;
}

void
Server::setDistribution(DistType distr, double mean, double variance) {
  MLCG *tmpGen = ((ServerState*)state->current)->gen;

  switch(distr){
  case UNIFORM :
    numGen = new Uniform(mean, variance, tmpGen);
    break;
  case NORMAL :
    numGen = new Normal(mean, variance, tmpGen);
    break;
  case BINOMIAL :
    numGen = new Binomial((int)mean, variance, tmpGen);
    break;
  case POISSON :
    numGen = new Poisson(mean, tmpGen);
    break;
  case EXPONENTIAL :
    numGen = new NegativeExpntl(mean, tmpGen);
    break;
  default :
    cerr << "Improper Distribution for a Server Object!!!" << "\n";
    break;
  }
}

int
Server::serviceTime() {
  double delay;

  delay = (*numGen)();
  while (delay <= 0.0) {
    delay = (*numGen)();
  }
  return ( (int) delay );
}

void
Server::executeProcess() {
  Client *recvEvent;
  recvEvent = (Client*) getEvent();
  
  if (recvEvent != NULL) {
    // Send event to the Statistics Collector
    StatEvent *newEvent = (StatEvent*) new char[sizeof(StatEvent)];
    new (newEvent) StatEvent();

    newEvent->dest = statDest;
    newEvent->size = sizeof(StatEvent);
    newEvent->entryTime = recvEvent->entryTime;
    newEvent->waitTime = getLVT();
    // For the system exit time find out the service time for this client
    newEvent->exitTime = getLVT() + serviceTime();
    newEvent->recvTime = newEvent->exitTime;

    // Send event to the queue object saying this server is idle
    BasicEvent *idleEvent = (BasicEvent*) new char[sizeof(BasicEvent)];
    new (idleEvent) BasicEvent();

    idleEvent->dest = qDest;
    idleEvent->recvTime = newEvent->exitTime;
    idleEvent->size = sizeof(BasicEvent);

    sendEvent(newEvent);
    sendEvent(idleEvent);
  }  
}

BasicState*
Server::allocateState() {
  return new ServerState();
}

