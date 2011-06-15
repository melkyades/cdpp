#include "RouteState.hh"

RouteState::RouteState() {
  head = tail = NULL;
  noMess = 0;
  utilTime = 0;
  curSchedTime = 0;
  gen = new MLCG;
}

RouteState::~RouteState() {
  Link *tmpLink = head;
  while (tmpLink != NULL) {
    head = tmpLink->next;
    //cout << "Delete pointer " << tmpLink << "\n";
    delete tmpLink;
    tmpLink = head;
  }
  delete gen;
}

void
RouteState::enQueue(MessEvent *addEvt) {
  if (head == NULL) {
    head = new Link;
    //cout << "Allocated pointer " << head << "\n"; 
    head->evnt = addEvt;
    head->next = NULL;
    tail = head;
  } else {
    // append this item to the queue
    Link *newItem = new Link;
    //cout << "Allocated pointer " << newItem << "\n"; 
    newItem->evnt = addEvt;
    newItem->next = NULL;
    tail->next = newItem;
    tail = newItem;
  }
  noMess++;
}

Link*
RouteState::deQueue() {
  Link *retVal;
 
  if (head != NULL) {
    noMess--;
    retVal = head;
    head = head->next;
    return retVal;
  } else {
    return NULL;
  }
}

RouteState&
RouteState::operator=(RouteState &rhs) {
  (BasicState &)*this = (BasicState &) rhs;
  Link *tmpLink = rhs.head;
  head = tail = NULL;
  noMess = rhs.noMess;
  utilTime = rhs.utilTime;
  curSchedTime = rhs.curSchedTime;
  *(this->gen) = *rhs.gen;

  while (tmpLink != NULL) {
    if (head == NULL) {
      head = new Link;
      head->evnt = new MessEvent();
      memcpy(head->evnt, tmpLink->evnt, sizeof(MessEvent));
      head->next = NULL;
      tail = head;
    } else {
      // append this item to the queue
      Link *newItem = new Link;
      newItem->evnt = new MessEvent();
      memcpy(newItem->evnt, tmpLink->evnt, sizeof(MessEvent));
      newItem->next = NULL;
      tail->next = newItem;
      tail = newItem;
    }
    tmpLink = tmpLink->next;
  }
}

void
RouteState::copyState(BasicState *cpyState) {
  *this = *((RouteState*)cpyState);
}


