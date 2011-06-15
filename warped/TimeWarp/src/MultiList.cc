//-*-c++-*-
#ifndef MULTILIST_CC
#define MULTILIST_CC
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
// $Id: MultiList.cc,v 1.4 1999/10/24 23:26:10 ramanan Exp $
//
//
//---------------------------------------------------------------------------

template < class Element >
inline
MultiList< Element >::MultiList(){
}


template < class Element >
inline void
MultiList< Element >::initialize(int numObj) {
  numObjects = numObj;
  listSize  = new int[numObj];
  headObj   = new BasicEvent* [numObj];
  tailObj   = new BasicEvent* [numObj];
  findObj   = new BasicEvent* [numObj];
  insertObj = new BasicEvent* [numObj];
  currentObj = new BasicEvent* [numObj];
  lVTArray = new VTime[numObj];
  idleObjArray = new bool[numObj];

  for (int i = 0; i < numObj; i++) {
    listSize[i]  = 0;
    headObj[i]   = NULL;
    tailObj[i]   = NULL;
    findObj[i]   = NULL;
    insertObj[i] = NULL;
    currentObj[i] = NULL;
    lVTArray[i] = ZERO;
    idleObjArray[i] = false;
  }
}

template < class Element >
inline
MultiList< Element >::~MultiList(){
	delete [] listSize;
	delete [] headObj;
	delete [] tailObj;
	delete [] findObj;
	delete [] insertObj;
	delete [] currentObj;
	delete [] lVTArray;
	delete [] idleObjArray;
}

template < class Element >
inline Element*
MultiList< Element >::findFirstAtTime(Element* current){
  Element* firstGuy = current;

  while(firstGuy->prev != NULL &&
	current->recvTime == firstGuy->prev->recvTime) {
    firstGuy = firstGuy->prev;
  }
  findPos = firstGuy;
  return firstGuy;
}

template < class Element >
inline VTime
MultiList< Element >::findPrevTime(int i){
#ifdef MATTERNGVTMANAGER
  VTime retval = PINFINITY;  
  if (currentObj[i] != NULL){
    if(currentObj[i]->prevObj != NULL ) {
      retval = currentObj[i]->prevObj->recvTime;
    }
    else {
      retval = currentObj[i]->recvTime;
    }
  }
  return retval;
#else
  return lVTArray[i];
#endif
}

template <class Element >
inline Element*
MultiList< Element >::peekPrevObj(){
  Element* retval;
  if (currentPos != NULL) {
    if(currentPos->prevObj == NULL) {
      retval = NULL;
    }
    else {
      retval = currentPos->prevObj;
    }
  }
  else {
    retval = NULL;
  }
  return retval;
}

template <class Element >
inline Element*
MultiList< Element >::getObj(int id){
  Element *retval;
  if (currentObj[id] == NULL) {
    retval = NULL;
  }
  else {
    retval = currentObj[id];
  }
  return retval;
}

template <class Element >
inline Element*
MultiList< Element >::setCurrentObjToFindObj(int id) {
  Element *retval;
  currentObj[id] = findObj[id];
  if (currentObj[id] == NULL) {
    retval = NULL;
  }
  else {
    retval = currentObj[id];
  }

  return retval;
}

template <class Element >
inline bool
MultiList< Element >::testCurrentObj(int id){
  if(currentObj[id] != NULL ){
    return true;
  }
  else {
    return false;
  }
}


// resetMiniHead moves the head of minilist 'i' to the new position for 
// garbage collection
template < class Element >
inline void 
MultiList< Element >::resetMiniHead(int id, VTime& gtime) {

  while (headObj[id] != NULL && 
	 headObj[id]->recvTime < gtime) {
    headObj[id] = headObj[id]->nextObj;
  }
  if (headObj[id] == NULL) {
    tailObj[id] = NULL;
    findObj[id] = NULL;
    insertObj[id] = NULL;
    listSize[id] = 0;
  }
  else {
		//headObj[id]->prevObj = NULL;
    insertObj[id] = tailObj[id];
    findObj[id] = tailObj[id];
  }
}

// seekid moves currentPos to one of the following
//   START + offset    : starting from head of minilist spec. by id
//   CURRENT + offset  : starting from currentpos -- no relation to id
//   END + offset      : starting from tail of minilist spec. by id
template < class Element >
inline Element*
MultiList< Element >::seekId(int offset, listMode_t mode, int id){
  Element* retval;
  int i;
  
  if(abs(offset) < listSize[id]) { 
    // check if offset greater than no. of elmts.
    
    switch (mode) {
    case START:
      if ((headObj[id] != NULL) && (offset >= 0)) {
	// start at head, move forward by offset elements
	currentPos = headObj[id];
	for (i = 0; i < offset; i++) {
	  currentPos = currentPos->nextObj;
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
	    currentPos = currentPos->nextObj;
	    i--;
	  }
	}
	else { // offset < 0
	  while ((i < 0) && (currentPos != NULL)) {
	    // seek backward either the number of steps or until the end
	    currentPos = currentPos->prevObj;
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
      if ((tailObj[id] != NULL) && (offset <= 0)) {
	currentPos = tailObj[id];
	for (i = offset; i < 0; i++) {
	  currentPos = currentPos->prevObj;
	}
	retval = currentPos;
      }
      else {
	retval = NULL;
	currentPos = NULL;
      }
      break;
    default:
      cerr << "ERROR in MultiList::Seek--Invalid seek mode" << endl;
      exit(-1);
    }
  }
  else { // seek is longer than list size
    retval = NULL;
    currentPos = NULL;
  }
  return retval;
} 



// insert toInsert into the main list and adjust minilist pointers
//    insert element means insertion as defined by the compare function
//    both main and minilist are sorted.
// insertPos will point to inserted element.
template < class Element >
inline void
MultiList< Element >::insert(Element* newelem, int listId) {

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
    if(compare(newelem, insertPos) > 0) {
      // after insertPos

      // two cases to consider here
      //[a] newelem has to be inserted after insertPos as compare
      //    returned value > 0
      //[b] compare returned 0. In this case, we have to search
      //    for the first element with the same id and time

      while(insertPos != NULL &&
	    compare(newelem, insertPos) > 0) {
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
	    compare(newelem, insertPos) <= 0) {
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
  
  
  // Minilist update
  if (listSize[listId] == 0){
    // empty list
    headObj[listId] = newelem;
    tailObj[listId] = newelem;
    headObj[listId]->prevObj = NULL;
    tailObj[listId]->nextObj = NULL;
  }
  else {
    // insertObj can not be NULL because at least one element exists
    if(compare(newelem, insertObj[listId]) > 0) {
      // after insertPos
      while(insertObj[listId] != NULL &&
	    compare(newelem, insertObj[listId]) > 0) {
	insertObj[listId] = insertObj[listId]->nextObj; 
      }
      if(insertObj[listId]==NULL){
	// tail reached
	newelem->prevObj = tailObj[listId];
	tailObj[listId]->nextObj = newelem;
	tailObj[listId] = newelem;
	newelem->nextObj = NULL;
      }
    }
    else {
      // before insertPos
      while(insertObj[listId] != NULL &&
	    compare(newelem, insertObj[listId]) <= 0) {
	if(insertObj[listId]->prevObj != NULL){
	  insertObj[listId] = insertObj[listId]->prevObj; 
	}
	else {
	  insertObj[listId] = NULL ;
	}
      }
      if(insertObj[listId]==NULL){
	// head reached
	newelem->nextObj = headObj[listId];
	headObj[listId]->prevObj = newelem;
	headObj[listId] = newelem;
	newelem->prevObj = NULL;
      }
      else {
	insertObj[listId] = insertObj[listId]->nextObj;
      }
    }
  }
  // insertPos is pointing to NULL if tail or head
  // and otherwise it is pointing to the element behind insertion point
  if( insertObj[listId] != NULL ){
    newelem->nextObj = insertObj[listId];
    newelem->prevObj = insertObj[listId]->prevObj;
    insertObj[listId]->prevObj->nextObj = newelem;
    insertObj[listId]->prevObj = newelem;
  }
	
  insertPos = newelem;
  listsize++;
	
  insertObj[listId] = newelem;
  listSize[listId]++;
}

// remove container pointed to by currentPos, returns object pointer 
// ATTENTION: The minilist pointers need to be updated correctly.
// actually resetminihead updates the minilist for us, so this
// allows us to blindly call remove

template < class Element >
inline Element*
MultiList< Element >::removeCurrentObj() {
  return MultiList<Element>::remove(currentPos);
  
}


// remove container pointed to by findObj spec. by id, 
// returns object pointer and 
// resets findObj of id to NULL
template < class Element >
inline Element*
MultiList< Element >::removeFind(int id) {
  Element *removeObj = MultiList<Element>::remove(findObj[id],id);
  findObj[id] = NULL;
  return removeObj;
}

// the argument delptr points to an existing element in the list.
// also, it is assumed that id matches to the minilist id to which 
// delptr is pointing. If no id is specified the default argument for
// id is -1. Note: This implies no minilist update is done.
template < class Element >
inline Element*
MultiList< Element >::remove(Element* delptr, int id) {
  Element *retval;
#ifdef LPDEBUG
  if(id >= 0) {
    //    printSmallQ(id);
    //    cout << "Id = " << id << " delptr = " << delptr << endl;
  }
#endif
  if ((delptr == NULL) || (listsize == 0)) {
    cerr << "ERROR: MultiList::remove--can't remove NULL elements!" << endl; 
    retval = NULL;
  } 
  else {
    // list has at least one element and the pointer which is given 
    // as argument is a pointer in the list

    // Main list
    if (head == tail) { // only one element
      // sanity check - can be removed later....
      if (delptr != head) {
	cout << "MultiList: ERROR!  Removing element not in list!" << endl;
	cout << "Element being removed: " 
	     << delptr << " " << *(delptr) << endl;
	print();
	abort();
      }
      else {
	head = NULL;
	tail = NULL;
	insertPos = NULL;
	currentPos = NULL;
	findPos = NULL;
      }
    }
    else if (head == delptr) { // first element
      head = delptr->next;
      head->prev = NULL;
      if(insertPos == delptr) {
	insertPos = head;
      }
      if(currentPos == delptr){
	currentPos = head;
      }
    }
    else if (tail == delptr) { // last element
      tail = delptr->prev;
      tail->next = NULL;
      if(insertPos == delptr) {
	insertPos = tail;
      }
      if(currentPos == delptr){
	currentPos = tail;
      }
    }
    else { // middle
      delptr->prev->next = delptr->next;
      delptr->next->prev = delptr->prev;
      if (insertPos == delptr) {
	insertPos = delptr->next;
      }
      if (currentPos == delptr) {
	currentPos = delptr->next;
      }
      if (findPos == delptr) {
	findPos = delptr->next;
      }
    }
    
    
    // remove container from the appropriate mini list
    if (id >= 0) { // is this if test necessary ?
      if (headObj[id] == tailObj[id]) { // only one element
	// sanity check - can be removed later....
	if (delptr != headObj[id]) {
	  cout << "MultiList: ERROR!  Element not in minilist!" << endl;
	  cout << "Element being removed: " << delptr << " " 
	       << *(delptr) << endl;
	  print();
	  abort();
	}
	else {
	  headObj[id] = NULL;
	  tailObj[id] = NULL;
	  insertObj[id] = NULL;
	  currentObj[id] = NULL;
	  findObj[id] = NULL;
	  listSize[id] = 0;
	}
      }
      else {
	if (headObj[id] == delptr) { // first element
	  headObj[id] = delptr->nextObj;
	  headObj[id]->prevObj = NULL;
	  if (insertObj[id] == delptr) {
	    insertObj[id] = headObj[id];
	  }
	  if(currentObj[id] == delptr){
	    currentObj[id] = headObj[id];
	  }
	  if(findObj[id] == delptr){
	    findObj[id] = headObj[id];
	  }
	}
	else if (tailObj[id] == delptr) { // last element
	  tailObj[id] = delptr->prevObj;
	  tailObj[id]->nextObj = NULL;
	  if (insertObj[id] == delptr) {
	    insertObj[id] = tailObj[id];
	  }
	  if(currentObj[id] == delptr){
	    currentObj[id] = tailObj[id];
	  }
	  if(findObj[id] == delptr){
	    findObj[id] = tailObj[id];
	  }
	}
	else { // middle
	  if (delptr->prevObj != NULL)
	    delptr->prevObj->nextObj = delptr->nextObj;
	  delptr->nextObj->prevObj = delptr->prevObj;
	  if (insertObj[id] == delptr) {
	    insertObj[id] = delptr->nextObj;
	  }
	  if (currentObj[id] == delptr) {
	    currentObj[id] = delptr->nextObj;
	  }
	  if (findObj[id] == delptr) {
	    findObj[id] = delptr->nextObj;
	  }
	}
	listSize[id]--;
      }
    }
    retval = delptr;
    listsize--;
  }
  return retval;
}


// This function calls the backwards search function to find an event
// which satisfies the find mode with respect to time and id as given 
// in the compare function. It is using the findObj pointer if given, 
// otherwise starts at the tail of the minilist. Note: The function is 
// using the minilist not the main list.
template < class Element >
inline Element*
MultiList< Element >::find(Element *element, findMode_t mode, int id){ 
  if( findObj[id] == NULL ) {
    return findBackwards(element, tailObj[id], mode,id);
  }
  else {
    return findBackwards(element, findObj[id], mode,id);
  }
}


// This function steps further through the minilist specified by the id,
// starting with the element specified in findObj[id]. It steps through 
// the list only as long as there are more events at the same time for this 
// id. End is signalled by returning NULL.
template < class Element >
inline Element* 
MultiList< Element >::findNext(int id) {
  Element *retval;

  // start at list head, if no valid find ptr
  if (findObj[id] == NULL) {
    retval = NULL;
  }
  else {
    // if findPos->next is the time we're looking for, return it
    if (findObj[id]->nextObj == NULL) {
      findObj[id] = NULL;
      retval = NULL;
    }
    else {
      if (compare(findObj[id], findObj[id]->nextObj) == 0) {
				findObj[id] = findObj[id]->nextObj;
				retval = findObj[id];
      }
      else {
				findObj[id] = NULL;
				retval = NULL;
      }
    }
  }
  return retval;
}


// Starting from the startFrom pointer in the list this function finds
// the first occurance of an event with this id and time. From that it 
// moves depending on the mode.
//  [10] - [15] - [15] - [16] = all in the same id list
//                  ^
//                  startFrom and element searched for is 15 EQUAL
//           ^ = result
template < class Element >
inline Element*
MultiList< Element >::findBackwards(Element *element, 
				    Element *startFrom,
				    findMode_t mode, int id){
  Element *current;

  current = startFrom;
  if (current == NULL) {
    //   cerr << "SortedList::findBackwards--list is empty! (size = " << listsize 
    //	 << ")" << endl;
    findObj[id] = NULL;
    findPos = NULL ;
  }
  else {
    if( compare(current, element ) >= 0 ) {
      // the current is greater than or equal to the element we're
      // looking for - go left..
      while((current != NULL) && (compare(current, element) >= 0)) {
	current=current->prevObj;
      }
      // kludge alert...  The case stuff (below) needs to be fixed instead
      // of the following 6 lines...  I'l do it later...
      if(current == NULL) {
	current = headObj[id];
      }
      else {
	current = current->nextObj;
      }
    }
    else {
      // the current is less than the element we're looking for...
      while( (current !=NULL) && compare(current, element ) < 0 ) {
	current=current->nextObj;
      }
    }
    
    // current is now either the first listelem equal to element (if it 
    // exists), or the first listelem greater than element (if there is 
    // no equal in the list).
    switch (mode) {
    case LESS:
      if (current == NULL) { // at end of list
	findObj[id] = tailObj[id];
	findPos = findObj[id];
      }
      else {
	findObj[id] = current->prevObj;
	findPos = findObj[id];
      }
      break;
    case LESSEQUAL:
      if (current == NULL) { // at end of list
	findObj[id] = tailObj[id];
	findPos = findObj[id];
      }
      else if (compare(element, current) == 0) {
	findObj[id] = current;
	findPos = findObj[id];
      }
      else {
	findObj[id] = current->prevObj;
	findPos = findObj[id];
      }
      break;
    case EQUAL:
      if (current == NULL) {
	findObj[id] = NULL;
	findPos = findObj[id];
      }
      else if (compare(element, current) == 0) {
	findObj[id] = current;
	findPos = findObj[id];
      }
      else { // element is greater; so, nothing was found.
	findObj[id] = NULL;
	findPos = findObj[id];
      }
      break;
    case GREATEREQUAL:
      if (current == NULL) {
	findObj[id] = NULL;
	findPos = findObj[id];
      }
      else {
	findObj[id] = current;
	findPos = findObj[id];
      }
      break;
    case GREATER:
      // Adjust the list to skip over all list elements equal to element
      while ((current != NULL) && (compare(current,element) == 0)) {
	current = current->nextObj;
      }
      if (current == NULL) {
	findObj[id] = NULL;
	findPos = findObj[id];
      }
      else if (compare(current, element) > 0) {
	findObj[id] = current;
	findPos = findObj[id];
      } 
      else {
	findObj[id] = NULL;
	findPos = findObj[id];
      }
      break;
    default:
      cerr << "ERROR: SortedList::findElement--invalid find mode" << endl;
      exit(-2);
      break;
    }
  }
  if (findObj[id] == NULL) {
    return NULL;
  }
  else {
    return findObj[id];
  }
}

template < class Element >
ostream&
operator<< (ostream& os, const MultiList<Element>& ml) {
  register Element *ptr;
  register unsigned i = 0;
  int id, j, col;

  os << (SortedListOfEvents<BasicEvent>&) ml << "\n\n";

  for (i = 0, id = 0; id < ml.numObjects; id++) {
    os << "MiniList " << id << ": length = " << ml.listSize[id] << "\n";
    for (ptr = ml.headObj[id]; ptr != NULL; ptr = ptr->nextObj) {
      os << "MiniList[" << i << "] = " << ptr << " " 
				 << *(ptr) << "\n";
      i++;
    }
    if(ml.currentObj[id]) {
      os << "currentPos : " << ml.currentObj[id] << " " 
				 << *(ml.currentObj[id]) <<endl;
    }
    if(ml.insertObj[id]){
      os << "insertPos : " << ml.insertObj[id] << " " 
				 << *(ml.insertObj[id]) <<endl;
    }
    if(ml.findObj[id]) {
      os << "findPos : " << ml.findObj[id] << " " 
				 << *(ml.findObj[id]) <<endl;
    }
    os << "-----\n" << endl;
  }
	
  col = 0;
  os << "\n\ncurrentObjArray:\n";
  for(j=0; j< ml.numObjects; j++){
    if (col >= 64) {
      os << "\n";
      col = 0;
    }
    if(ml.currentObj[j] != NULL){
      // Chris: This was lVTArray, but I changed it because that was not
      // being set consistently and I think printCurrentObj should print
      // either the pointer or some value referenced through currentObj.
      os << "(" << j << ")" << ml.currentObj[j] << "["
				 << ml.currentObj[j]->recvTime << "] ";
      col += 16;
    }
    else {
      col += 5;
      os << "NULL ";
    }
  }
  os << "\n\nlVTarray:\n";
	
  for(j=0; j< ml.numObjects; j++){
    if (j%8 == 0) {
      os << "\n";
    }
    os << "(" << j << ")" << ml.lVTArray[j] << " ";
  }
  os << endl;
	
  return( os );
	
}

template < class Element >
inline void
MultiList < Element >::printSmallQ(ostream& out,int id) {
  register Element *ptr;
  register unsigned i = 0;
	
  out << "MiniList " << id << endl;
  out << "length = " << listSize[id] << "\n";
  for (ptr = headObj[id]; ptr != NULL; ptr = ptr->nextObj) {
    out << "MiniList[" << i << "] = " << ptr << " " 
				<< *(ptr) << "\n";
    i++;
  }
  if(currentObj[id]) {
    out << "currentPos : " << currentObj[id] << " " 
				<< *(currentObj[id]) <<endl;
  }
  if(insertObj[id]){
    out << "insertPos : " << insertObj[id] << " " 
				<< *(insertObj[id]) <<endl;
  }
  if(findObj[id]) {
    out << "findPos : " << findObj[id] << " " 
				<< *(findObj[id]) <<endl;
  }
  if(tailObj[id]) {
    out << "tailPos : " << tailObj[id] << " " 
				<< *(tailObj[id]) <<endl;
  }
	out << "-----\n" << endl;
}

template < class Element >
inline void
MultiList < Element >::printCurrentObj() {
	//  for(int i =0; i< numObjects; i++){
	//    if(currentObj[i] != NULL && currentObj[i]->prevObj != NULL)
	//    cout << currentObj[i]->prevObj->object->recvTime << " ";
	//  }
	//  cout << endl;
  int j, col;
  col = 0;
  for(j=0; j< numObjects; j++){
    if (col >= 64) {
      cout << "\n";
      col = 0;
    }
    if(currentObj[j] != NULL){
      // Chris: This was lVTArray, but I changed it because that was not
      // being set consistently and I think printCurrentObj should print
      // either the pointer or some value referenced through currentObj.
      cout << "(" << j << ")" << currentObj[j] << "["
					 << currentObj[j]->recvTime << "] ";
      col += 16;
    }
    else {
      col += 5;
      cout << "NULL ";
    }
  }
  cout << endl;
}

template < class Element >
inline void
MultiList < Element >::printlVTArray() {
  int j;
  for(j=0; j< numObjects; j++){
    if (j%8 == 0) {
      cout << "\n";
    }
    cout << "(" << j << ")" << lVTArray[j] << " ";
  }
  cout << endl;

}

template <class Element>
inline Element*
MultiList <Element>::getHead(int id) {
  return headObj[id];
}

#endif

