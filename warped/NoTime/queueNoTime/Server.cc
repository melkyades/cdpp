#include "Server.hh"
#include "ServerState.hh"

Server::Server(int myId, int destId1, int destId2) {
  id = myId;
  qDest = destId1;
  statDest = destId2;

  int *ptr = new int;
  gen = new MLCG((int)ptr, ((int)ptr+6));
  delete ptr;
}

Server::~Server() {
  delete gen;
  delete numGen;
}

void
Server::setDistribution(DistType distr, double mean, double variance) {

  switch(distr){
  case UNIFORM :
    numGen = new Uniform(mean, variance, gen);
    break;
  case NORMAL :
    numGen = new Normal(mean, variance, gen);
    break;
  case BINOMIAL :
    numGen = new Binomial((int)mean, variance, gen);
    break;
  case POISSON :
    numGen = new Poisson(mean, gen);
    break;
  case EXPONENTIAL :
    numGen = new NegativeExpntl(mean, gen);
    break;
  default :
    cerr << "Improper Distribution for a Server Object!!!" << "\n";
    break;
  }
}

int
Server::serviceTime() {
  double delay = -1.0;

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
    // update the elapsed time of the server if needed
    if (recvEvent->entryTime > ((ServerState*)state->current)->elapsedTime) {
      ((ServerState*)state->current)->elapsedTime = recvEvent->entryTime;
    }
    // Send event to the Statistics Collector
    StatEvent *newEvent = (StatEvent*) new char[sizeof(StatEvent)];
    new (newEvent) StatEvent();

    newEvent->dest = statDest;
    newEvent->size = sizeof(StatEvent);
    newEvent->entryTime = recvEvent->entryTime;
    newEvent->waitTime = ++((ServerState*)state->current)->elapsedTime;
    // For the system exit time find out the service time for this client
    newEvent->exitTime = newEvent->waitTime + serviceTime();
    // Set the new elapsed time of the server
    ((ServerState*)state->current)->elapsedTime = newEvent->exitTime;
    newEvent->recvTime = getLVT() + 1;

    // Send event to the queue object saying this server is idle at this time
    IdleEvent *idEvent = (IdleEvent*) new char[sizeof(IdleEvent)];
    new (idEvent) IdleEvent();

    idEvent->dest = qDest;
    idEvent->recvTime = getLVT() + 1;
    idEvent->idleTime = newEvent->exitTime;
    idEvent->size = sizeof(IdleEvent);

    sendEvent(newEvent);
    sendEvent(idEvent);
  }  
}

BasicState*
Server::allocateState() {
  return new ServerState();
}

