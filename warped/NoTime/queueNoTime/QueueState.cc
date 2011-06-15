#include "QueueState.hh"

QueueState::QueueState(int noServers) {
  servInfo = new Info[noServers];
  // Set all the servers to idle initially
  for (int i = 0; i < noServers; i++) {
    servInfo[i].servIdle = true;
    servInfo[i].idleTime = 0;
  }
  allServerBusy = false;
  noServ = noServers;
  head = tail = NULL;
}

QueueState::~QueueState() {
  delete [] servInfo;
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
  servInfo = new Info[noServ];
  for (int i = 0; i < noServ; i++) {
    servInfo[i].servIdle = cpyState.servInfo[i].servIdle;
    servInfo[i].idleTime = cpyState.servInfo[i].idleTime;
  }
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

