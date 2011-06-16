//-*-c++-*-
#ifndef SORTED_LIST_CC
#define SORTED_LIST_CC
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
// $Id: SortedList.cc,v 1.2 1999/06/21 19:29:33 ramanan Exp $
//
//---------------------------------------------------------------------------

template < class Element >
inline SortedList < Element >::SortedList() {
  head = NULL;
  tail = NULL;
  insertPos = NULL;
  currentPos = NULL;
  findPos = NULL;
  listsize = 0;
  compare = NULL;
}


template < class Element >
inline SortedList< Element >::SortedList(int(*compareFunc)(const Element*, const Element*)) {
  head = NULL;
  tail = NULL;
  insertPos = NULL;
  currentPos = NULL;
  findPos = NULL;
  listsize = 0;
  compare = compareFunc;
}


template < class Element >
inline SortedList< Element>::~SortedList() {
  register Container<Element>* curptr;
  // step through the doubly-linked list and delete all elements
  curptr = head;
  if (head != NULL) {
    curptr = head;
    while (curptr->next != NULL) {
      curptr = curptr->next;
      delete curptr->prev;
    }
    delete curptr;
  }
}


template < class Element > 
inline void
SortedList< Element >::setFunc( int(*compareFunc)(const Element*, const Element*)) {
  compare = compareFunc;
}


template < class Element > 
inline Element* SortedList < Element >::front() {
  if (head == NULL) {
    return NULL;
  }
  else {
    return head->object;
  }
}


template < class Element > 
inline Element* SortedList< Element >::back() {
  if (tail == NULL) {
    return NULL;
  } 
  else {
    return tail->object;
  }
}


template < class Element > 
inline Element* 
SortedList< Element >::seek(int offset, listMode_t mode) {
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
	retval = currentPos->object;
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
	retval = currentPos->object;
      }
      else {
	retval = NULL;
	//	currentPos = NULL;	// Unnecessary statement.
      }
      break;
    case END:
      if ((tail != NULL) && (offset <= 0)) {
	currentPos = tail;
	for (i = offset; i < 0; i++) {
	  currentPos = currentPos->prev;
	}
	retval = currentPos->object;
      }
      else {
	retval = NULL;
	currentPos = NULL;
      }
      break;
    default:
      cerr << "ERROR in SortedList::Seek--Invalid seek mode" << endl;
      exit(-1);
    }
  }
  else { // seek is longer than list size
    retval = NULL;
    currentPos = NULL;
  }
  return retval;
} 


template < class Element >
inline void
SortedList < Element >::setFile(ofstream *outfile) {
  lpFile = outfile;
}


// insert element into the queue
template < class Element >
inline void
SortedList < Element >::insert(Element* toInsert) {
  register Container< Element >* newelem;

  newelem = new Container<Element>;
  newelem->object = toInsert;
  // Initialization necessary here since it is removed from Container()
  newelem->next = NULL;
  newelem->prev = NULL;

    // Mainlist update
  if (listsize == 0) {
    // empty list
    head = newelem;
    head->prev = NULL;
    tail = newelem;
    tail->next = NULL;
  }
  else {
    // insertPos can not be NULL because at least one element exists
    if(compare(newelem->object, insertPos->object) > 0) {
      // after insertPos
      
      // two cases to consider here
      //[a] newelem has to be inserted after insertPos as compare
      //    returned value > 0
      //[b] compare returned 0. In this case, we have to search
      //    for the first element with the same id and time
      
      while(insertPos != NULL &&
	    compare(newelem->object, insertPos->object) > 0) {
	insertPos = insertPos->next; 
      }
      if(insertPos==NULL){
	// tail reached
	newelem->prev = tail;
	tail->next = newelem;
	tail = newelem;
	newelem->next = NULL;
      }
    }
    else {
      // before insertPos
      while(insertPos != NULL &&
	    compare(newelem->object, insertPos->object) <= 0) {
	insertPos = insertPos->prev; 
      }
      if(insertPos == NULL){
	// head reached
	newelem->next = head;
	head->prev = newelem;
	head = newelem;
	newelem->prev = NULL;
      }
      else {
	insertPos = insertPos->next;
      }
    }
  }
  // insertPos is pointing to NULL if tail or head
  // and otherwise it is pointing to the element behind insertion point
  if( insertPos != NULL ){
    newelem->next = insertPos;
    newelem->prev = insertPos->prev;
    insertPos->prev->next = newelem;
    insertPos->prev = newelem;
  }
  
//   if (listsize == 0) {
//     head = newelem;
//     tail = newelem;
//     currentPos = NULL;
//     // just in case
//     findPos = NULL;
//   }
//   else {
//     bool done = false;
//     while (done == false) {
//       if (compare(newelem->object, insertPos->object) >= 0) {
// 	if (insertPos == tail) {
// 	  insertPos->next = newelem;
// 	  newelem->prev = insertPos;
// 	  tail = newelem;
// 	  done = true;
// 	}
// 	else {
// 	  insertPos = insertPos->next;
// 	}
//       }
//       // if it's not greater equal, it must be less... 
//       // else if (compare(newelem->object, insertPos->object) < 0) {
//       else {
// 	if (insertPos == head) {
// 	  insertPos->prev = newelem;
// 	  newelem->next = insertPos;
// 	  head = newelem;
// 	  done = true;
// 	}
// 	else if (compare(newelem->object, insertPos->prev->object) >= 0) {
// 	  newelem->next = insertPos;
// 	  newelem->prev = insertPos->prev;
// 	  insertPos->prev->next = newelem;
// 	  insertPos->prev = newelem;
// 	  done = true;
// 	}
// 	else {
// 	  insertPos = insertPos->prev;
// 	}
//       }
//     } // while
//   } // else size > 0
  insertPos = newelem;
  listsize++;
}
    

// return element at the currentPos pointer
template < class Element > 
inline Element*
SortedList< Element >::get() const {
  Element *retval;

  if(currentPos == NULL) {
    retval = NULL;
  }
  else {
    retval = currentPos->object;
  }
  return retval;
}


template <class Element>
inline Element*
SortedList<Element>::peekPrev() const {
  if (currentPos == NULL || currentPos->prev == NULL) {
    return NULL;
  }
  else {
    return currentPos->prev->object;
  }
}


template <class Element>
inline Element*
SortedList<Element>::peekNext() const {
  if (currentPos == NULL || currentPos->next == NULL) {
    return NULL;
  }
  else {
    return currentPos->next->object;
  }
}


// delete element pointed to by currentPos from the queue
template < class Element>
inline Element*
SortedList < Element >::removeCurrent() {
  return remove(currentPos);
}

template < class Element>
inline Element*
SortedList < Element >::removeFind() {
  return remove(findPos);
}

template < class Element>
inline Element*
SortedList < Element >::remove(Container<Element> * delptr) {
  register Container<Element>* prevptr;
  register Container<Element>* nextptr;
  Element *retval;

  if (delptr == NULL) {
    cerr << "ERROR: SortedList::remove--can't remove NULL elements!"
	 << endl;
    retval = NULL;
  } 
  else {
    if (delptr == head) {
      nextptr = delptr->next;
      if (nextptr == NULL) { // std::list has only 1 element
	head = NULL;
	tail = NULL;
	insertPos = NULL;
	currentPos = NULL;
	findPos = NULL;
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
    retval = delptr->object;
    delete delptr;
    listsize--;
  }
  findPos = NULL;
  return retval;
}


template < class Element >
inline Element*
SortedList< Element >::findBackwards(Element *element, 
				     Container<Element> *startFrom, 
				     findMode_t mode) {
  Container < Element > *current;

  current = startFrom;
  if (current == NULL) {
    //   cerr << "SortedList::findBackwards--list is empty! (size = " << listsize 
    //	 << ")" << endl;
    findPos = NULL;
  }
  else {
    if( compare(current->object, element ) >= 0 ) {
      // the current is greater than or equal to the element we're
      // looking for - go left..
      while((current != NULL) && (compare(current->object, element) >= 0)) {
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
      while( (current !=NULL) && compare(current->object, element ) < 0 ) {
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
      else if (compare(element, current->object) == 0) {
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
      else if (compare(element, current->object) == 0) {
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
      while ((current != NULL) && (compare(current->object,element) == 0)) {
	current = current->next;
      }
      if (current == NULL) {
	findPos = NULL;
      }
      else if (compare(current->object, element) > 0) {
	findPos = current;
      } 
      else {
	findPos = NULL;
      }
      break;
    default:
      cerr << "ERROR: SortedList::findElement--invalid find mode" << endl;
      exit(-2);
      break;
    }
  }
  if (findPos == NULL) {
    return NULL;
  }
  else {
    return findPos->object;
  }
}


// Accepts an Element with whatever key the compare function needs filled in.
// Returns the first occurrence of an element in the list with that key,
// or NULL, if that key isn't in the list.
template < class Element >
inline Element*
SortedList < Element >::find(Element *element, findMode_t mode) {
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
template < class Element>
inline Element* 
SortedList < Element >::findNext() {
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
    else if (compare(findPos->object, findPos->next->object) == 0) {
      findPos = findPos->next;
      retval = findPos->object;
    }
    else {
      findPos = NULL;
      retval = NULL;
    }
  }
  return retval;
}

// template < class Element >
// ostream&
// operator<< (ostream& os, const SortedList<Element>& sl) {
//   Container<Element> *ptr;
//   unsigned i;

//   i = 0;
//   if (sl.listsize == 0) {
//     os << "List = (NULL)\n";
//   }
//   else {
//     for (ptr = sl.head; ptr != NULL; ptr = ptr->next) {
//       os << "List[" << i << "] = " << ptr->object << " " 
// 	 << *(ptr->object) << "\n";
//       i++;
//     }
//   }
//   if(sl.currentPos) {
//     os << "currentPos : " << sl.currentPos->object << " " 
//        << *(sl.currentPos->object) <<endl;
//   }
//   if(sl.insertPos){
//     os << "insertPos : " << sl.insertPos->object << " " 
//        << *(sl.insertPos->object) <<endl;
//   }
//   if(sl.findPos) {
//     os << "findPos : " << sl.findPos->object << " " 
//        << *(sl.findPos->object) <<endl;
//   }
//   os << "-----\n" << endl;

//   return(os);
// }

template < class Element >
inline void
SortedList < Element >::print(ostream& outFile) const {
  
   Container<Element> *ptr;
   unsigned i;
   
   i = 0;
   if (listsize == 0) {
     outFile << "List = (NULL)\n";
   }
   else {
     for (ptr = head; ptr != NULL; ptr = ptr->next) {
       outFile << "List[" << i << "] = " << ptr->object << " " 
	       << *(ptr->object) << "\n";
       i++;
     }
   }
   
   if(currentPos) {
     outFile << "currentPos : " << currentPos->object << " " 
	     << *currentPos->object <<endl;
   }
   if(insertPos){
     outFile << "insertPos : " << insertPos->object << " " 
	     << *insertPos->object <<endl;
   }
   if(findPos) {
     outFile << "findPos: " << findPos->object << " " << *findPos->object 
	     << endl;
   }
   
   outFile << "-----\n" << endl;
}


template < class Element >
inline Element* 
SortedList < Element >::setCurrentToInsert() {
  currentPos = insertPos;
  if (currentPos == NULL) {
    return NULL;
  }
  else {
    return currentPos->object;
  }
}


template < class Element >
inline Element* 
SortedList < Element >::setCurrentToFind() {
  currentPos = findPos;
  if (currentPos == NULL) {
    return NULL;
  }
  else {
    return currentPos->object;
  }
}

#endif
