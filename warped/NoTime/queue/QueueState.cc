#include "QueueState.hh"

QueueState::QueueState(int noServers) {
  serverIdle = new bool[noServers];
  // Set all the servers to idle initially
  for (int i = 0; i < noServers; i++) {
    serverIdle[i] = true;
  }
  noServ = noServers;
  head = tail = NULL;
}

QueueState::~QueueState() {
  delete [] serverIdle;
  Link *tmpLink = head;
  while (tmpLink != NULL) {
    head = tmpLink->next;
    delete tmpLink;
    tmpLink = head;
  }
}

void
QueueState::enQueue(int eventTime) {
  if (head == NULL) {
    head = new Link;
    head->entryTime = eventTime;
    head->next = NULL;
    tail = head;
  } else {
    // append this item to the queue
    Link *newItem = new Link;
    newItem->entryTime = eventTime;
    newItem->next = NULL;
    tail->next = newItem;
    tail = newItem;
  }
}

Link*
QueueState::deQueue() {
  Link *retVal;
  
  return ((head != NULL) ? ((retVal = head), (head = head->next), retVal) :
	  head);
}

QueueState&
QueueState::operator=(QueueState& cpyState) {
  (BasicState &)*this = (BasicState &) cpyState;
  this->noServ = cpyState.noServ;
  
  memcpy(this->serverIdle, cpyState.serverIdle, sizeof(bool)*noServ);
  Link *tmpLink = cpyState.head;
  head = tail = NULL;
  while (tmpLink != NULL) {
    if (head == NULL) {
      head = new Link;
      head->entryTime = tmpLink->entryTime;
      head->next = NULL;
      tail = head;
    } else {
      // append this item to the queue
      Link *newItem = new Link;
      newItem->entryTime = tmpLink->entryTime;
      newItem->next = NULL;
      tail->next = newItem;
      tail = newItem;
    }
    tmpLink = tmpLink->next;
  }
}

void
QueueState::copyState(BasicState *rhs) {
  *this = *((QueueState*)rhs);
}

