#include "Queue.hh"
#include "QueueState.hh"

Queue::Queue(int myId, int firstServId, int lastServId){
  id = myId;
  int noServers = lastServId - firstServId;
  servIds = new int[noServers];
  for (int i = 0; i < noServers; i++) {
    servIds[i] = firstServId + i;
  }
  noServ = noServers;
}

Queue::~Queue() {
  delete [] servIds;
}

void
Queue::executeProcess() {
  BasicEvent *recvEvent;
  recvEvent = getEvent();

  if (recvEvent != NULL) {
    // The id of the source is always less than the queue's
    Client *customer = (Client*)recvEvent;
    if (customer->sender < id) {
      // This means that the event is from source
      // Find if any of the servers are idle
      int i;
      for (i = 0; i < noServ; i++) {
	if (((QueueState*)state->current)->serverIdle[i] == true) {
	  break;
	}
      }

      if (i < noServ) {
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

	newEvent->dest = servIds[i];
	newEvent->size = sizeof(Client);
	newEvent->recvTime = getLVT() + 1;
	// Set this server as busy
	((QueueState*)state->current)->serverIdle[i] = false;

	sendEvent(newEvent);
      } else {
	//Enqueue the event if it can't be sent to any server
	((QueueState*)state->current)->enQueue(customer->entryTime);
      }
    } else {
      // The received event is from a server which is an idle message
      // If there is a customer waiting in the queue send him to this server
      if (((QueueState*)state->current)->head != NULL) {
	Client *newEvent = (Client*) new char[sizeof(Client)];
	new (newEvent) Client();

	newEvent->entryTime = ((QueueState*)state->current)->head->entryTime;
	Link *tmpLink = ((QueueState*)state->current)->head;
	((QueueState*)state->current)->head = tmpLink->next;
	delete tmpLink;
	
	newEvent->dest = recvEvent->sender;
	newEvent->size = sizeof(Client);
	newEvent->recvTime = getLVT() + 1;

	sendEvent(newEvent);
      } else {
	// No customer 
	// Set the idle flag of the corresponding server to true
	int i;
	for (i = 0; i < noServ; i++) {
	  if (servIds[i] == recvEvent->sender) {
	    break;
	  }
	}
	((QueueState*)state->current)->serverIdle[i] = true;
      }
    }
  }
}

BasicState*
Queue::allocateState() {
  return new QueueState(noServ);
}

