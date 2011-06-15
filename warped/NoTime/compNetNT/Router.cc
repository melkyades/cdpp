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
  int *ptr = new int;
  gen = new MLCG((int)ptr, ((int)ptr+6));
  delete ptr;

  // Set the distributions for this router
  setDistr();
}

Router::~Router() {
  delete gen;
  delete numGen;
}

void
Router::setDistr() {

  double mean, variance;
  DistType distr;
  
  distr = netConf->routerTable[index].dist;
  mean = netConf->routerTable[index].mean;
  variance = netConf->routerTable[index].var;
  
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
	if (netConf->routerTable[index].connRouter & (1<<j)) {
	  return (netConf->routerTable[j].id - 1);
	} else {
	  for (j = 0; j < netConf->noRouters; j++) {
	    // This place if Funky - Algo is not correct
	    if (netConf->routerTable[index].connRouter & (1<<j)) {
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
      packet = (MessEvent*) new char[sizeof(MessEvent)];
      new (packet) MessEvent();
      memcpy(packet, (MessEvent*)recvEvent, sizeof(MessEvent));
      packet->noHops++;

      // Enqueue this packet and schedule its delivery
      // after servTime if it is the only packet for delivery
      if (((RouteState*)state->current)->head == NULL) {
	if (((RouteState*)state->current)->elapsedTime < packet->entryTime) {
	  ((RouteState*)state->current)->elapsedTime = packet->entryTime;
	}
	sched = (SchedEvent*) new char[sizeof(SchedEvent)];
	new (sched) SchedEvent();
	sched->dest = id;
	servTime = servDelay();
	((RouteState*)state->current)->elapsedTime += servTime;
	sched->recvTime = getLVT() + 1;
	sched->size = sizeof(SchedEvent);
	
	((RouteState*)state->current)->utilTime += servTime;
	sendEvent(sched);
      } 

      ((RouteState*)state->current)->enQueue(packet);
      if (maxQLen < ((RouteState*)state->current)->noMess) {
	maxQLen = ((RouteState*)state->current)->noMess;
      }
    } else {
      // A schedul event -- means despatch the first packet from the queue
      Link *tmpLink = ((RouteState*)state->current)->deQueue();
      if (tmpLink != NULL) {
	packet = tmpLink->evnt;

	if (packet->noHops < 4) {
	  // Route this packet
	  dest = packet->finalDest + 1; // the net id and TimeWarp id differ by 1
	  if ((dest == netConf->routerTable[index].startNode) ||
	      (dest > netConf->routerTable[index].startNode &&
	       dest <= netConf->routerTable[index].endNode)) {
	    packet->dest = dest - 1;
	    packet->entryTime = ((RouteState*)state->current)->elapsedTime + propDelay(0);
	    packet->recvTime = getLVT() + 1;
	  } else {
	    // This means that the message has to go across LANs (WANs)
	    // The ROUTING ALGO IS NOT COMPLETE!!!
	    packet->dest = getRouterId(dest);
	    packet->entryTime = ((RouteState*)state->current)->elapsedTime + propDelay(1);
	    packet->recvTime = getLVT() + 1;
	  }
	  packet->size = sizeof(MessEvent);
	  if (packet->dest != -1) {
	    sendEvent(packet);
	  } else {
	    delete [] ((char*)packet);
	  }
	  delete tmpLink;
	} else {
	  
	  cerr << "The message from " << (packet->srcId+1) << " to "
	       << (packet->finalDest+1) << " could not be sent \n"
	       << "as the number of hops exceeded 4 \n";
	}
	
	if (((RouteState*)state->current)->head != NULL) {
	  // Schedule the next packet's despatch
	  packet = ((RouteState*)state->current)->head->evnt;
	  if (((RouteState*)state->current)->elapsedTime < packet->entryTime) {
	    ((RouteState*)state->current)->elapsedTime = packet->entryTime;
	  }
	  sched = (SchedEvent*) new char[sizeof(SchedEvent)];
	  new (sched) SchedEvent();
	  sched->dest = id;
	  servTime = servDelay();
	  ((RouteState*)state->current)->elapsedTime += servTime;
	  sched->recvTime = getLVT() + 1;
	  sched->size = sizeof(SchedEvent);
	  
	  ((RouteState*)state->current)->utilTime += servTime;
	  sendEvent(sched);
	  // Discard the packets that overflow the buffer
	  while (packet = ((RouteState*)state->current)->getOverflowPackets()) {
	    packet->dest = statDest;
	    packet->recvTime = getLVT() + 1;
	    sendEvent(packet);
	  }
	}
      } else {
	cerr << "The router queue is empty when it shoudn't be \n";
      }
    }
  }
}

BasicState*
Router::allocateState() {
  return new RouteState(netConf->bufferSize);
}

void
Router::finalize() {
  float utilization = (float)((RouteState*)state->current)->utilTime/((RouteState*)state->current)->elapsedTime;
  cout << "*** Router id " << (id+1) << endl;
  cout << "Max length of the Router queue = " << maxQLen << endl;
  cout << "Utilization of the router = " << utilization << endl;
}
