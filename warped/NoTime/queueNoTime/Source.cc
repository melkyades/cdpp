#include "Source.hh"
#include "SourceState.hh"

Source::Source(int myId, int myDest, int totalEvents) {
  id = myId;
  dest = myDest;
  maxEvents = totalEvents;

  int *ptr = new int;
  gen = new MLCG((int)ptr, ((int)ptr+15));
  delete ptr;
}

Source::~Source() {
  delete gen;
  delete numGen;
}

void
Source::setDistribution(DistType distr, double mean, double variance) {

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
    cerr << "Improper Distribution for a Source Object!!!" << "\n";
    break;
  }
}

void
Source::initialize() {
  // This event is required for scheduling the source itself
  // This event is scheduled for a time when an item in the queue
  // has to be entered
  BasicEvent *myEvent = (BasicEvent*) new char[sizeof(BasicEvent)];
  new (myEvent) BasicEvent();

  myEvent->dest = id;
  myEvent->recvTime = getLVT() + interArrivalTime();
  myEvent->size = sizeof(BasicEvent);

  sendEvent(myEvent);
}

int
Source::interArrivalTime() {
  double delay = -1.0;
  while (delay <= 0) {
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
