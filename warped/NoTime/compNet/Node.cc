#include "Node.hh"
#include "NodeState.hh"

Node::Node(int myId, ConfigNet *confObj, int rId, int statId) {
  id = myId;
  netConf = confObj;
  routerId = rId; 
  statDest = statId;
}

Node::~Node() {
  delete numGen;
  delete destGen;
}

void
Node::genMessage(MessEvent *newMess) {
  newMess->startTime = getLVT();
  newMess->srcId = id;
  newMess->finalDest = getDest();
  newMess->recvTime = getLVT() + netConf->LANDelay;
  newMess->dest = routerId;
  newMess->size = sizeof(MessEvent);
}

int
Node::getDest() {
  int dest = (int)(*destGen)();
  if (dest >= id) {
    return (dest+1);
  } else {
    return dest;
  }
}

void
Node::setDistr() {
  double mean, variance;
  DistType distr;
  
  distr = netConf->nodeDist[id];
  mean = netConf->nodeMean[id];
  variance = netConf->nodeVar[id];
  MLCG *tmpGen = ((NodeState*)state->current)->gen;

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

int
Node::interMessDelay() {
  double delay = -1.0;

  while (delay <= 0.0) {
    delay = (*numGen)();
  }
  return ((int)delay);
}

void
Node::initialize() {
  delete ((NodeState*)state->current)->gen;
  int *ptr = new int;
  ((NodeState*)state->current)->gen = new MLCG((int)ptr, (int)ptr+6);
  delete ptr;

  setDistr();
  destGen = new Uniform(0, netConf->noNodes-1, ((NodeState*)state->current)->gen);

  SchedEvent *newEvent = (SchedEvent*) new char[sizeof(SchedEvent)];
  new (newEvent) SchedEvent();

  newEvent->dest = id;
  newEvent->size = sizeof(SchedEvent);
  if (routerId != -1) {
    newEvent->recvTime = getLVT() + 1;
  } else {
    // This means this node is disconnected so let it sleep
    newEvent->recvTime = PINFINITY;
  }
  sendEvent(newEvent);
}

void
Node::executeProcess() {
  NetEvent *recvEvent = (NetEvent*)getEvent();
  
  if (recvEvent != NULL && getLVT() != PINFINITY) {
    if (recvEvent->type == SCHED) {
      // This means a message has to be originated from this node
      if (((NodeState*)state->current)->noMess > 0) {
	MessEvent *newEvent = (MessEvent*) new char[sizeof(MessEvent)];
	new (newEvent) MessEvent();
      
	genMessage(newEvent);
	sendEvent(newEvent);
      
	SchedEvent *nextMess = (SchedEvent*) new char[sizeof(SchedEvent)];
	new (nextMess) SchedEvent();
      
	nextMess->type = SCHED;
	nextMess->dest = id;
	nextMess->recvTime = getLVT() + interMessDelay();
	nextMess->size = sizeof(SchedEvent);
	((NodeState*)state->current)->noMess--;
	sendEvent(nextMess);
      }
    } else { // recvEvent->type != SCHED
      MessEvent *statEvent = (MessEvent*) new char[sizeof(MessEvent)];
      new (statEvent) MessEvent();
      memcpy(statEvent, (MessEvent*)recvEvent, sizeof(MessEvent));
      
      if (statEvent->finalDest == id) {
	statEvent->recvTime = getLVT() + 1;
	statEvent->dest = statDest;
	statEvent->size = sizeof(MessEvent);
	sendEvent(statEvent);
      } else {
	cerr << "The message has not been delivered to the correct destination \n";
      }
    }
  }
}

BasicState*
Node::allocateState() {
  return new NodeState(netConf->totMess);
}
