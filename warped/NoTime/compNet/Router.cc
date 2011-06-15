#include "Router.hh"

Router::Router(int myId, ConfigNet *confObj, int sd){
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

Router::~Router() {
  delete numGen;
}

void
Router::initialize() {
  delete ((RouteState*)state->current)->gen;
  int *ptr = new int;
  ((RouteState*)state->current)->gen = new MLCG((int)ptr, (int)ptr+5);
  delete ptr;
  setDistr();
}

void
Router::setDistr() {

  double mean, variance;
  DistType distr;
  
  distr = netConf->routerTable[index].dist;
  mean = netConf->routerTable[index].mean;
  variance = netConf->routerTable[index].var;
  MLCG *tmpGen = ((RouteState*)state->current)->gen;

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
Router::servDelay() {
  int delay = (int)(*numGen)();
  while (delay <= 0) {
    delay = (int)(*numGen)();
  }
  return delay;
}

int
Router::propDelay(int delayType) {
  if (delayType == 0) {
    // this means it is a router to node delay or LAN delay
    return netConf->LANDelay;
  } else {
    // This means it is a router to router delay WAN delay
    return netConf->WANDelay;
  }
}

int
Router::getRouterId(int dest) {
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
Router::executeProcess() {
  NetEvent *recvEvent = (NetEvent*)getEvent();
  MessEvent *packet;
  SchedEvent *sched;
  int dest, i, servTime;

  if (recvEvent != NULL) {
    if (recvEvent->type == MESSAGE) {

      // Enqueue this packet and schedule its delivery
      // after servTime if it is the only packet for delivery
      if (((RouteState*)state->current)->head == NULL) {
	sched = (SchedEvent*) new char[sizeof(SchedEvent)];
	new (sched) SchedEvent();
	sched->dest = id;
	servTime = servDelay();
	if (((RouteState*)state->current)->curSchedTime < getLVT()) {
	  ((RouteState*)state->current)->curSchedTime = getLVT();
	}
	sched->recvTime = ((RouteState*)state->current)->curSchedTime + servTime;
	((RouteState*)state->current)->curSchedTime = sched->recvTime;
	sched->size = sizeof(SchedEvent);
	
	((RouteState*)state->current)->utilTime += servTime;
	sendEvent(sched);
      } 
      // The packet is enqueued only if the router buffer is not full
      if (((RouteState*)state->current)->noMess < netConf->bufferSize) {
	packet = new MessEvent();
	memcpy(packet, (MessEvent*)recvEvent, sizeof(MessEvent));
	packet->noHops++;
	((RouteState*)state->current)->enQueue(packet);
	if (maxQLen < ((RouteState*)state->current)->noMess) {
	  maxQLen = ((RouteState*)state->current)->noMess;
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
      // A schedul event -- means despatch the first packet from the queue
      Link *tmpLink = ((RouteState*)state->current)->deQueue();
      if (tmpLink != NULL) {
	packet = tmpLink->evnt;

	if (packet->noHops < 4) {
	  // Route this packet
	  MessEvent *newPacket = (MessEvent*) new char[sizeof(MessEvent)];
	  new (newPacket) MessEvent();
	  memcpy(newPacket, packet, sizeof(MessEvent));
	  dest = newPacket->finalDest + 1; // the net id and TimeWarp id differ by 1
	  if ((dest == netConf->routerTable[index].startNode) ||
	      (dest > netConf->routerTable[index].startNode &&
	       dest <= netConf->routerTable[index].endNode)) {
	    newPacket->dest = dest - 1;
	    newPacket->recvTime = getLVT() + propDelay(0);
	  } else {
	    // This means that the message has to go across LANs (WANs)
	    // The ROUTING ALGO IS NOT COMPLETE!!!
	    newPacket->dest = getRouterId(dest);
	    newPacket->recvTime = getLVT() + propDelay(1);
	  }
	  newPacket->size = sizeof(MessEvent);
	  if (newPacket->dest != -1) {
	    sendEvent(newPacket);
	  } else {
	    delete [] ((char*)newPacket);
	  }
	  delete tmpLink;
	} else {
	  
	  cerr << "The message from " << (packet->srcId+1) << " to "
	       << (packet->finalDest+1) << " could not be sent \n"
	       << "as the number of hops exceeded 4 \n";
	}
	
	if (((RouteState*)state->current)->head != NULL) {
	  // Schedule the next packet's despatch
	  sched = (SchedEvent*) new char[sizeof(SchedEvent)];
	  new (sched) SchedEvent();
	  sched->dest = id;
	  servTime = servDelay();
	  sched->recvTime = ((RouteState*)state->current)->curSchedTime + servTime;
	  ((RouteState*)state->current)->curSchedTime = sched->recvTime;
	  sched->size = sizeof(SchedEvent);

	  ((RouteState*)state->current)->utilTime += servTime;
	  sendEvent(sched);
	}
      } else {
	//	cerr << "The router queue is empty when it shoudn't be \n";
      }
    }
  }
}

BasicState*
Router::allocateState() {
  return new RouteState();
}

void
Router::finalize() {
  float utilization = (float)((RouteState*)state->current)->utilTime/getLVT();
  cout << "*** Router id " << (id+1) << endl;
  cout << "Max length of the Router queue = " << maxQLen << endl;
  cout << "Utilization of the router = " << utilization << endl;
}
