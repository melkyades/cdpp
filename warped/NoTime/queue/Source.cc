#include "Source.hh"
#include "SourceState.hh"

Source::Source(int myId, int myDest, int totalEvents) {
  id = myId;
  dest = myDest;
  maxEvents = totalEvents;
}

Source::~Source() {
  delete numGen;
}


void
Source::setDistribution(DistType distr, double mean, double variance) {
  MLCG *tmpGen = ((SourceState*)state->current)->gen;

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
    cerr << "Improper Distribution for a Source Object!!!" << "\n";
    break;
  }
}

void
Source::initialize() {
  delete ((SourceState*)state->current)->gen;

  int *ptr = new int;
  ((SourceState*)state->current)->gen = new MLCG((int)ptr, ((int)ptr+15));
  delete ptr;

  // This event is required for scheduling the source itself
  // This event is scheduled for a time when an item in the queue
  // has to be entered
  BasicEvent *myEvent = (BasicEvent*) new char[sizeof(BasicEvent)];
  new (myEvent) BasicEvent();

  myEvent->dest = id;
  myEvent->recvTime = getLVT() + 1;
  myEvent->size = sizeof(BasicEvent);

  sendEvent(myEvent);
}

int
Source::interArrivalTime() {
  double delay;
  delay = (*numGen)();
  while ((int) delay <= 0) {
    delay = (*numGen)();
  }
  return ( (int) delay );
}

void
Source::executeProcess() {
  BasicEvent *recvEvent;
  
  recvEvent = getEvent();

  if (recvEvent != NULL && ((SourceState*)state->current)->eventsSent < maxEvents) {

    // When the source receives an event it means that an item in 
    // the queue has to sent out now
    Client *newItem;
    newItem = (Client*) new char[sizeof(Client)];
    new (newItem) Client();
    
    newItem->dest = dest;
    newItem->entryTime = getLVT() + 1;
    newItem->recvTime = newItem->entryTime;
    newItem->size = sizeof(Client);
    
    sendEvent(newItem);
    ((SourceState*)state->current)->eventsSent++; // Increment as a queue item has been generated

    BasicEvent *myEvent = (BasicEvent*) new char[sizeof(BasicEvent)];
    new (myEvent) BasicEvent();
    
    myEvent->dest = id;
    myEvent->recvTime = getLVT() + interArrivalTime();
    myEvent->size = sizeof(BasicEvent);
    
    sendEvent(myEvent);
  }
}

BasicState*
Source::allocateState() {
  return new SourceState();
}
