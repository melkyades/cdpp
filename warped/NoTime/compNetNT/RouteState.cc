#include "RouteState.hh"

RouteState::RouteState(int bs) {
  head = tail = NULL;
  noMess = 0;
  utilTime = 0;
  elapsedTime = 0;
  buffSize = bs;
  prevOflMark = NULL;
  oflMark = NULL;
}

RouteState::~RouteState() {
  Link *tmpLink = head;
  while (tmpLink != NULL) {
    head = tmpLink->next;
    delete tmpLink;
    tmpLink = head;
  }
}

void
RouteState::enQueue(MessEvent *addEvt) {
  Link *tmpLink, *prevLink;
  if (head == NULL) {
    head = new Link;
    //cout << "Allocated pointer " << head << "\n"; 
    head->evnt = addEvt;
    head->next = NULL;
    tail = head;
  } else {
    // Maintain an ordered queue (ordered on entry time of packets)
    Link *newItem = new Link;
    //cout << "Allocated pointer " << newItem << "\n"; 
    newItem->evnt = addEvt;
    for (tmpLink = head->next, prevLink = head; tmpLink != NULL;) {
      if (tmpLink->evnt->entryTime > addEvt->entryTime) {
	prevLink->next = newItem;
	newItem->next = tmpLink;
	break;
      }
      prevLink = tmpLink;
      tmpLink = tmpLink->next;
    }
    if (tmpLink == NULL) {
      prevLink->next = newItem;
      newItem->next = NULL;
      tail = newItem;
    }
  }
  noMess++;
  if (oflMark == NULL && noMess > buffSize) {
    oflMark = tail;
    for (prevOflMark = head, tmpLink = head->next; tmpLink != oflMark; ) {
      prevOflMark = tmpLink;
      tmpLink = tmpLink->next;
    }
  }
}

Link*
RouteState::deQueue() {
  Link *retVal;
 
  if (head != NULL) {
    noMess--;
    retVal = head;
    head = head->next;
    // Move the buffer overflow marker after dequeuing an item
    if (oflMark != NULL) {
      prevOflMark = oflMark;
      oflMark = oflMark->next;
    }
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
  while (tmpLink != NULL) {
    if (head == NULL) {
      head = new Link;
      head->evnt = tmpLink->evnt;
      head->next = NULL;
      tail = head;
    } else {
      // append this item to the queue
      Link *newItem = new Link;
      newItem->evnt = tmpLink->evnt;
      newItem->next = NULL;
      tail->next = newItem;
      tail = newItem;
    }
    tmpLink = tmpLink->next;
  }
}

MessEvent*
RouteState::getOverflowPackets() {
  MessEvent *retVal = NULL;
  Link *tmpLink;
  if (oflMark != NULL && oflMark->evnt->entryTime <= elapsedTime) {
    retVal = oflMark->evnt;
    tmpLink = oflMark;
    oflMark = oflMark->next;
    prevOflMark->next = oflMark;
    delete tmpLink;
    noMess--;
    return retVal;
  }
  return retVal;
}

void
RouteState::copyState(BasicState *cpyState) {
  *this = *((RouteState*)cpyState);
}












