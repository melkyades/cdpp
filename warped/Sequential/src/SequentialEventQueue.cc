//-*-c++-*-
#ifndef SORTED_LIST_OF_EVENTS_CC
#define SORTED_LIST_OF_EVENTS_CC
// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.

// BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY 
// FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT 
// PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, 
// EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE 
// PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME 
// THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION. 

// IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING 
// WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR 
// REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR 
// DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL 
// DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM 
// (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED 
// INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF 
// THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH HOLDER 
// OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 

//
// $Id:
//
//---------------------------------------------------------------------------

#include "SequentialEventQueue.hh"

SequentialEventQueue::SequentialEventQueue() {
  head       = NULL;
  tail       = NULL;
  insertPos  = NULL;
  currentPos = NULL;
  findPos    = NULL;
  listsize   = 0;
}

SequentialEventQueue::~SequentialEventQueue() {
  register Element* curptr;
  register Element* tempPtr;
  
  // step through the doubly-linked list and delete all elements
  curptr = head;
  
  if (head != NULL) {
    while (curptr != NULL && curptr->next != NULL) {
      tempPtr = curptr;
      curptr  = curptr->next;
      delete [] ((char *)tempPtr);
    }
    delete [] ((char *)curptr);
  }
  head = NULL;
}

Element* 
SequentialEventQueue::seek(int offset, listMode_t mode) {
  Element* retval;
  int i;
  
  if(abs(offset) < listsize) { // check if offset greater than no. of elmts.
    
    switch (mode) {
    case START:
      if ((head != NULL) && (offset >= 0)) {
	// start at head, move forward by offset elements
	currentPos = head;
	for (i = 0; i < offset; i++) {
	  currentPos = currentPos->next;
	}
	retval = currentPos;
      }
      else {
	retval = NULL;
	currentPos = NULL;
      }
      break;
    case CURRENT:
      if (currentPos != NULL) {
	i = offset;
	if (offset >= 0) {
	  while ((i > 0) && (currentPos != NULL)) {
	    // seek forward either the number of steps or until the end
	    currentPos = currentPos->next;
	    i--;
	  }
	}
	else { // offset < 0
	  while ((i < 0) && (currentPos != NULL)) {
	    // seek backward either the number of steps or until the end
	    currentPos = currentPos->prev;
	    i++;
	  }
	}
      }
      // Now currentPos is either the desired element, or NULL
      if (currentPos != NULL) {
	retval = currentPos;
      }
      else {
	retval = NULL;
	currentPos = NULL;
      }
      break;
    case END:
      if ((tail != NULL) && (offset <= 0)) {
	currentPos = tail;
	for (i = offset; i < 0; i++) {
	  currentPos = currentPos->prev;
	}
	retval = currentPos;
      }
      else {
	retval = NULL;
	currentPos = NULL;
      }
      break;
    default:
      cerr << "ERROR in SequentialEventQueue::Seek--Invalid seek mode" << endl;
      exit(-1);
    }
  }
  else { // seek is longer than list size
    retval = NULL;
    currentPos = NULL;
  }
  return retval;
} 


// insert element into the queue
void
SequentialEventQueue::insert(Element* newelem) {
  // Mainlist update
  if (head == NULL) {
    head       = newelem;
    head->prev = NULL;
    tail       = newelem;
    tail->next = NULL;
  }
  else {
    // insertPos can not be NULL because at least one element exists
    if(CompareEvent(newelem, insertPos) > 0) {
      // after insertPos
      
      // two cases to consider here
      //[a] newelem has to be inserted after insertPos as compare
      //    returned value > 0
      //[b] compare returned 0. In this case, we have to search
      //    for the first element with the same id and time
      
      while((insertPos != NULL) && (CompareEvent(newelem, insertPos) > 0)) {
	insertPos = insertPos->next;
      }
      
      if(insertPos == NULL)  {
	newelem->prev = tail;
	tail->next    = newelem;
	tail          = newelem;
	newelem->next = NULL;
      }
    }
    else {
      // before insertPos
      while((insertPos != NULL) && (CompareEvent(newelem, insertPos) <= 0)) {
	insertPos = insertPos->prev;
      }
      if(insertPos == NULL){
	newelem->next = head;
	head->prev    = newelem;
	head          = newelem;
	newelem->prev = NULL;
      }
      else {
	insertPos = insertPos->next;
      }
      if (CompareEvent(currentPos, newelem) >= 0) {
	currentPos = newelem;
      }
    }
  }
  // insertPos is pointing to NULL if tail or head
  // and otherwise it is pointing to the element behind insertion point
  if(insertPos != NULL)  {
    newelem->next = insertPos;
    newelem->prev = insertPos->prev;
    insertPos->prev->next = newelem;
    insertPos->prev = newelem;
  }
  
  insertPos = newelem;
  
  if (currentPos == NULL) {
    currentPos = insertPos;
  }
  
  listsize++;
}

Element*
SequentialEventQueue::peekPrev() const {
  return ((currentPos != NULL) ? (currentPos->prev) : currentPos);
}

Element*
SequentialEventQueue::peekNext() const {
  return ((currentPos != NULL) ? (currentPos->next) : NULL);
}


// delete element pointed to by currentPos from the queue
Element*
SequentialEventQueue::removeCurrent() {
  if(currentPos != NULL) {
    return remove(currentPos);
  } else {
    return NULL;
  }
}

Element*
SequentialEventQueue::removeFind() {
  return remove(findPos);
}

Element*
SequentialEventQueue::remove(Element* delptr) {
  register Element* prevptr;
  register Element* nextptr;
  Element *retval;
  
  if (delptr == NULL) {
    cerr << "ERROR: SequentialEventQueue::remove--can't remove NULL elements!"
	 << endl;
    retval = NULL;
  } 
  else {
    if (delptr == head) {
      nextptr = delptr->next;
      if (nextptr == NULL) { // std::list has only 1 element
	head       = NULL;
	tail       = NULL;
	insertPos  = NULL;
	currentPos = NULL;
	findPos    = NULL;
      }
      else { // std::list has at least 2 elements
	nextptr->prev = NULL;
	head = nextptr;
	if (insertPos == delptr) { 
	  insertPos = head;
	}
	if (findPos == delptr) { 
	  findPos = NULL;
	}
	if (currentPos == delptr) {
	  currentPos = head;
	}
      }
    }
    else if (delptr == tail) {
      prevptr = delptr->prev;
      if (prevptr == NULL) { // std::list has only 1 element
	head = NULL;
	tail = NULL;
	insertPos = NULL;
	currentPos = NULL;
	findPos = NULL;
      }
      else { // std::list has at least 2 elements
	prevptr->next = NULL;
	tail = prevptr;
	if (findPos == delptr) { 
	  findPos = tail;
	}
	if (insertPos == delptr) { 
	  insertPos = tail;
	}
	if (currentPos == delptr) {
	  currentPos = NULL;
	}
      }
    }
    else { // deleting from the middle
      prevptr = delptr->prev;
      nextptr = delptr->next;
      prevptr->next = nextptr;
      nextptr->prev = prevptr;
      if (insertPos == delptr) { 
	insertPos = prevptr;
      }
      if (findPos == delptr) { 
	findPos = prevptr;
      }
      if (currentPos == delptr) {
	currentPos = nextptr;
      }
    }
    retval = delptr;
    //delete delptr; Note : Assumption is that as there is no containers,
    // we return the delptr so that we can deleted it outside.
    listsize--;
  }
  findPos = NULL;
  return retval;
}

inline Element*
SequentialEventQueue::findBackwards(Element *element, 
				  Element* startFrom, 
				  findMode_t mode) {
  Element* current;
  
  current = startFrom;
  if (current == NULL) {
    findPos = NULL;
  }
  else {
    if( CompareEvent(current, element) >= 0 ) {
      // the current is greater than or equal to the element we're
      // looking for - go left..
      while((current != NULL) && (CompareEvent(current, element) >= 0)) {
	current=current->prev;
      }
      // kludge alert...  The case stuff (below) needs to be fixed instead
      // of the following 6 lines...  I'l do it later...
      if(current == NULL) {
	current = head;
      }
      else {
	current = current->next;
      }
    }
    else {
      // the current is less than the element we're looking for...
      while( (current !=NULL) && CompareEvent(current, element ) < 0 ) {
	current=current->next;
      }
    }

    // current is now either the first listelem equal to element (if it 
    // exists), or the first listelem greater than element (if there is 
    // no equal in the list).
    switch (mode) {
    case LESS:
      if (current == NULL) { // at end of list
	findPos = tail;
      }
      else {
	findPos = current->prev;
      }
      break;
    case LESSEQUAL:
      if (current == NULL) { // at end of list
	findPos = tail;
      }
      else if (CompareEvent(element, current) == 0) {
	findPos = current;
      }
      else {
	findPos = current->prev;
      }
      break;
    case EQUAL:
      if (current == NULL) {
	findPos = NULL;
      }
      else if (CompareEvent(element, current) == 0) {
	findPos = current;
      }
      else { // element is greater; so, nothing was found.
	findPos = NULL;
      }
      break;
    case GREATEREQUAL:
      if (current == NULL) {
	findPos = NULL;
      }
      else {
	findPos = current;
      }
      break;
    case GREATER:
      // Adjust the list to skip over all list elements equal to element
      while ((current != NULL) && (CompareEvent(current,element) == 0)) {
	current = current->next;
      }
      if (current == NULL) {
	findPos = NULL;
      }
      else if (CompareEvent(current, element) > 0) {
	findPos = current;
      } 
      else {
	findPos = NULL;
      }
      break;
    default:
      cerr << "ERROR: SequentialEventQueue::findElement--invalid find mode" 
	   << endl;
      exit(-2);
      break;
    }
  }
  if (findPos == NULL) {
    return NULL;
  }
  else {
    return findPos;
  }
}


// Accepts an Element with whatever key the compare function needs filled in.
// Returns the first occurrence of an element in the list with that key,
// or NULL, if that key isn't in the list.

Element*
SequentialEventQueue::find(Element *element, findMode_t mode) {
  if( findPos == NULL ) {
    return findBackwards(element, tail, mode);
  }
  else {
    return findBackwards(element, findPos, mode);
  }
}


// Accepts an Element with whatever key the compare function needs filled in.
// Returns the first occurrence after the findPos pointer of an element 
// in the list with that key, or NULL, if that key isn't in the list.
Element* 
SequentialEventQueue::findNext() {
  Element *retval;
  
  // start at list head, if no valid find ptr
  if (findPos == NULL) {
    retval = NULL;
  }
  else {
    // if findPos->next is the time we're looking for, return it
    if (findPos->next == NULL) {
      findPos = NULL;
      retval = NULL;
    }
    else if (CompareEvent(findPos, findPos->next) == 0) {
      findPos = findPos->next;
      retval = findPos;
    }
    else {
      findPos = NULL;
      retval = NULL;
    }
  }
  return retval;
}

ostream&
operator<< (ostream& os, const SequentialEventQueue& sl) {
  Element* ptr;
  unsigned i;
  
  i = 0;
  if (sl.listsize == 0) {
    os << "List = (NULL)\n";
  }
  else {
    for (ptr = sl.head; ptr != NULL; ptr = ptr->next) {
      os << "List[" << i << "] = " << ptr << " " 
	 << *(ptr) << "\n";
      i++;
    }
  }
  if(sl.currentPos) {
    os << "currentPos : " << sl.currentPos << " " 
       << *(sl.currentPos) <<endl;
  }
  if(sl.insertPos){
    os << "insertPos : " << sl.insertPos << " " 
       << *(sl.insertPos) <<endl;
  }
  if(sl.findPos) {
    os << "findPos : " << sl.findPos << " " 
       << *(sl.findPos) <<endl;
  }
  os << "-----\n" << endl;
  
  return(os);
}

void
SequentialEventQueue::print(ostream& outptr) const {
  Element* ptr;
  unsigned i;
  
  i = 0;
  if (listsize == 0) {
    outptr << "List = (NULL)\n";
  }
  else {
    for (ptr = head; ptr != NULL; ptr = ptr->next) {
      outptr << "List[" << i << "] = " << ptr << " " 
	     << *(ptr) << "\n";
      i++;
    }
  }
  if(currentPos) {
    outptr << "currentPos : " << currentPos << " " 
	   << *currentPos <<endl;
  }
  if(insertPos){
    outptr << "insertPos : " << insertPos << " " 
	   << *insertPos <<endl;
  }
  if(findPos) {
    outptr << "findPos : " << findPos << " " << *findPos <<endl;
  }
  outptr << "-----\n" << endl;
}


Element* 
SequentialEventQueue::setCurrentToInsert() {
  currentPos = insertPos;
  if (currentPos == NULL) {
    return NULL;
  }
  else {
    return currentPos;
  }
}


Element* 
SequentialEventQueue::setCurrentToFind() {
  currentPos = findPos;
  if (currentPos == NULL) {
    return NULL;
  }
  else {
    return currentPos;
  }
}

#endif
