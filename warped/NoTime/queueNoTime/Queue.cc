#include "Queue.hh"
#include "QueueState.hh"
#include <limits.h>

Queue::Queue(int myId, int firstServId, int lastServId){
  id = myId;
  int noServers = lastServId - firstServId;
  servIds = new int[noServers];
  for (int i = 0; i < noServers; i++) {
    servIds[i] = firstServId + i;
  }
  noServ = noServers;
  causalViol = 0;
}

Queue::~Queue() {
  cout << "\nNo of violations Queue object is making = " << causalViol << endl;
  delete [] servIds;
}

void
Queue::executeProcess() {
  BasicEvent *recvEvent;
  recvEvent = getEvent();
  int serv;

  if (recvEvent != NULL) {
    // The id of the source is always less than the queue's
    Client *customer = (Client*)recvEvent;
    if (customer->sender < id) {
      // This means that the event is from source
      // Find out the server to whom this event has to be sent
      serv = serverToSend();

      if (serv < noServ) {
	// This means a server is idle
	Client *newEvent = (Client*) new char[sizeof(Client)];
	new (newEvent) Client();

	// Check if there is already a client waiting in the state queue
	if (((QueueState*)state->current)->head != NULL) {
	  newEvent->entryTime = ((QueueState*)state->current)->head->entryTime;
	  // move the head
	  Link *tmpLink;
	  tmpLink = ((QueueState*)state->current)->head;
	  ((QueueState*)state->current)->head = tmpLink->next;
	  delete tmpLink;
	  // Enter this item into the queue
	  ((QueueState*)state->current)->enQueue(customer->entryTime);
	} else {
	  newEvent->entryTime = customer->entryTime;
	}

	newEvent->dest = servIds[serv];
	newEvent->size = sizeof(Client);
	newEvent->recvTime = getLVT() + 1;
	// Set this server as busy
	((QueueState*)state->current)->servInfo[serv].servIdle = false;

	sendEvent(newEvent);
      } else { // No server Idle
	//Enqueue the event if it can't be sent to any server
	((QueueState*)state->current)->allServerBusy = true;
	((QueueState*)state->current)->enQueue(customer->entryTime);
      }
    } else {
      // The received event is from a server which is an idle message
      // If there is a customer waiting in the queue send him to the 
      // server with the least idletime stamp
      IdleEvent *idEvnt = (IdleEvent*)recvEvent;
      if (((QueueState*)state->current)->head != NULL) {
	Client *newEvent = (Client*) new char[sizeof(Client)];
	new (newEvent) Client();

	newEvent->entryTime = ((QueueState*)state->current)->head->entryTime;
	Link *tmpLink = ((QueueState*)state->current)->head;
	((QueueState*)state->current)->head = tmpLink->next;
	delete tmpLink;

	// Find the destination server
	if (((QueueState*)state->current)->allServerBusy == true) {
	  newEvent->dest = idEvnt->sender;
	} else {
	  serv = serverToSend();
	  if (idEvnt->idleTime < ((QueueState*)state->current)->servInfo[serv].idleTime) {
	    newEvent->dest = idEvnt->sender;
	  } else {
	    newEvent->dest = servIds[serv];
	    ((QueueState*)state->current)->servInfo[serv].servIdle = false;
	    // Set the server from whom idle message was recieved as idle
	    ((QueueState*)state->current)->servInfo[idEvnt->sender-2].servIdle = true;
	    ((QueueState*)state->current)->servInfo[idEvnt->sender-2].idleTime = idEvnt->idleTime;
	  }
	}

	newEvent->size = sizeof(Client);
	newEvent->recvTime = getLVT() + 1;

	sendEvent(newEvent);
      } else { // No customer 
	// Set the idle flag of the corresponding server to true
	((QueueState*)state->current)->allServerBusy = false;
	int i;
	for (i = 0; i < noServ; i++) {
	  if (servIds[i] == idEvnt->sender) {
	    break;
	  }
	}
	((QueueState*)state->current)->servInfo[i].servIdle = true;
	((QueueState*)state->current)->servInfo[i].idleTime = idEvnt->idleTime;
      }
    }
  }
}

int
Queue::serverToSend() {
  int retServ = noServ;
  int leastIdleTime = INT_MAX;
  for (int i = 0; i < noServ; i++) {
    if (((QueueState*)state->current)->servInfo[i].servIdle == true) {
      if (((QueueState*)state->current)->servInfo[i].idleTime < leastIdleTime) {
	leastIdleTime = ((QueueState*)state->current)->servInfo[i].idleTime;
	retServ = i;
      }
    }
  }
  return retServ;
}

void 
Queue::causalityViolationHandler(BasicEvent *evt) {
  causalViol++;
}

BasicState*
Queue::allocateState() {
  return new QueueState(noServ);
}

