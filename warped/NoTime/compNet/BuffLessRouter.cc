#include "BuffLessRouter.hh"

BuffLessRouter::BuffLessRouter(int myId, ConfigNet *confObj, int sd){
  netConf = confObj;
  id = myId;
  maxQLen = 0;
  statDest = sd;
  
  // Store the index into the router table (of this node)
  for (index = 0; index < netConf->noRouters; index++) {
    if (id == netConf->routerTable[index].id - 1) {
      break;
    }
  }
}

BuffLessRouter::~BuffLessRouter() {
  delete numGen;
}

void
BuffLessRouter::initialize() {
  delete ((BuffLessRouteState*)state->current)->gen;
  int *ptr = new int;
  ((BuffLessRouteState*)state->current)->gen = new MLCG((int)ptr, (int)ptr+5);
  delete ptr;
  setDistr();
}

void
BuffLessRouter::setDistr() {

  double mean, variance;
  DistType distr;
  
  distr = netConf->routerTable[index].dist;
  mean = netConf->routerTable[index].mean;
  variance = netConf->routerTable[index].var;
  MLCG *tmpGen = ((BuffLessRouteState*)state->current)->gen;

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
BuffLessRouter::servDelay() {
  int delay = (int)(*numGen)();
  while (delay <= 0) {
    delay = (int)(*numGen)();
  }
  return delay;
}

int
BuffLessRouter::propDelay(int delayType) {
  if (delayType == 0) {
    // this means it is a router to node delay or LAN delay
    return netConf->LANDelay;
  } else {
    // This means it is a router to router delay WAN delay
    return netConf->WANDelay;
  }
}

int
BuffLessRouter::getRouterId(int dest) {
  int j;
  for (j = 0; j < netConf->noRouters; j++) {
    if (j != index) {
      if ((dest == netConf->routerTable[j].startNode) ||
	  (dest > netConf->routerTable[j].startNode &&
	   dest <= netConf->routerTable[j].endNode)) {
	if (netConf->routerTable[index].connRouter && (1<<j)) {
	  return (netConf->routerTable[j].id - 1);
	} else {
	  for (j = 0; j < netConf->noRouters; j++) {
	    // This place if Funky - Algo is not correct
	    if (netConf->routerTable[index].connRouter && (1<<j)) {
	      return (netConf->routerTable[j].id - 1);
	    }
	  }
	}
      }
    }
  }
  return -1;
}

void
BuffLessRouter::executeProcess() {
  NetEvent *recvEvent = (NetEvent*)getEvent();
  MessEvent *packet;
  SchedEvent *sched;
  int dest, i, servTime;

  if (recvEvent != NULL) {
    if (recvEvent->type == MESSAGE) {
      if (((BuffLessRouteState*)state->current)->noMess < netConf->bufferSize) {
	// Despatch this packet if the router buffer is not full

	packet = new MessEvent();
	memcpy(packet, (MessEvent*)recvEvent, sizeof(MessEvent));
	packet->noHops++;

	if (packet->noHops < 4) {
	  ((BuffLessRouteState*)state->current)->noMess++;
	  if (maxQLen < ((BuffLessRouteState*)state->current)->noMess) {
	    maxQLen = ((BuffLessRouteState*)state->current)->noMess;
	  }
	  
	  // Send an event to indicate when this packet was scheduled for despatch
	  sched = (SchedEvent*) new char[sizeof(SchedEvent)];
	  new (sched) SchedEvent();
	  sched->dest = id;
	  servTime = servDelay();
	  if (((BuffLessRouteState*)state->current)->curSchedTime < getLVT()) {
	    ((BuffLessRouteState*)state->current)->curSchedTime = getLVT();
	  }
	  sched->recvTime = ((BuffLessRouteState*)state->current)->curSchedTime + servTime;
	  ((BuffLessRouteState*)state->current)->curSchedTime = sched->recvTime;
	  sched->size = sizeof(SchedEvent);
	  
	  ((BuffLessRouteState*)state->current)->utilTime += servTime;
	  sendEvent(sched);
	  
	  // Route this packet
	  dest = packet->finalDest + 1; // the net id and TimeWarp id differ by 1
	  if ((dest == netConf->routerTable[index].startNode) ||
	      (dest > netConf->routerTable[index].startNode &&
	       dest <= netConf->routerTable[index].endNode)) {
	    packet->dest = dest - 1;
	    packet->recvTime = ((BuffLessRouteState*)state->current)->curSchedTime + propDelay(0);
	  } else {
	    // This means that the message has to go across LANs (WANs)
	    // The ROUTING ALGO IS NOT COMPLETE!!!
	    packet->dest = getRouterId(dest);
	    packet->recvTime = ((BuffLessRouteState*)state->current)->curSchedTime + propDelay(1);
	  }
	  packet->size = sizeof(MessEvent);
	  if (packet->dest != -1) {
	    sendEvent(packet);
	  } else {
	    delete [] ((char*)packet);
	  }
	} else {
	  // if the number of hops exceeds 4 then discard this packet
	  delete [] ((char*)packet);
	}
      } else {
	// Just discard this packet as the router buffer is overflowing
	packet = (MessEvent*) new char[sizeof(MessEvent)];
	new (packet) MessEvent();
	memcpy(packet, (MessEvent*)recvEvent, sizeof(MessEvent));
	packet->noHops++;
	packet->dest = statDest;
	packet->recvTime = getLVT() + 1;
	sendEvent(packet);
      }
    } else {
      // A schedule event
      ((BuffLessRouteState*)state->current)->noMess--;
    }
  }
}

BasicState*
BuffLessRouter::allocateState() {
  return new BuffLessRouteState();
}

void
BuffLessRouter::finalize() {
  float utilization = (float)((BuffLessRouteState*)state->current)->utilTime/getLVT();
  cout << "*** Router id " << (id+1) << endl;
  cout << "Max length of the Router queue = " << maxQLen << endl;
  cout << "Utilization of the router = " << utilization << endl;
}
