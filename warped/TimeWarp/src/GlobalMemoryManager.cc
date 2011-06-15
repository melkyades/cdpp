#ifndef GLOBALMEMORYMANAGER_CC
#define GLOBALMEMORYMANAGER_CC

// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.
//
// BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY 
// FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT 
// PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, 
// EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE 
// PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME 
// THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION. 
//
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
//
// 
//
//
//--------------------------------------------------------------------------





#include "GlobalMemoryManager.hh"

// constructor
MemoryManager::MemoryManager(){

  createTable(); // creates hash table
}

//destructor
MemoryManager::~MemoryManager(){
  register int i;
  void* delptr;
  freeNode *ptr, *nextptr;
  for(i = 0; i < HASHTABLESIZE; i++) {
    if(hashTable[i].freesize != 0) {
//      cout << "cleaning up (" << hashTable[i].freesize << ")"<< endl;
//      cout << "size is " << *((int*)(hashTable[i].headptr->blockPtr) - SIZE)
//           << endl;
      ptr = hashTable[i].headptr;
      do {
	nextptr = ptr->next;
        delptr = (char*)ptr->blockPtr - SIZE;
	free(delptr);
	free(ptr);
	ptr = nextptr;
      } while (nextptr != NULL);
    }
  }
  free(hashTable);
}


// createTable function - creates hash table of specified size
void
MemoryManager::createTable(){
  hashTable = (hashTableNode*) malloc(sizeof(hashTableNode) * HASHTABLESIZE);
    if(hashTable == NULL)
      {
	warpedNewHandler() ;
      }

  // initializing hashtable nodes
  for(int i = 0; i < HASHTABLESIZE; i++){
    hashTable[i].freesize = 0;
    hashTable[i].headptr = NULL;
    hashTable[i].tailptr = NULL;
  }
}
  

// hash function - returns hashed value 
int 
MemoryManager::hash(int valueToHash){
  register int hashedValue;
  // considering the last 10 bits only
  hashedValue = valueToHash & 1023 ; 
  return hashedValue;
}

// deallocate function - instead of deleting this block add this block to the 
// free list
void
MemoryManager::deAllocate(void* space, size_t bytes){
  register int value;
  freeNode* newNode;
  value = hash(bytes);// get location
  // check the list to see if it has been created
  if(hashTable[value].headptr == NULL){
    newNode = (freeNode*) malloc(sizeof(freeNode));
    if(newNode == NULL)
      {
	warpedNewHandler() ;
      }
 
    hashTable[value].headptr = newNode;
    hashTable[value].tailptr = newNode;
    hashTable[value].freesize++;
    newNode->blockPtr = space;
    newNode->next = NULL;
  }
  else if(hashTable[value].freesize < LISTSIZE){// allocate a list node
    newNode =(freeNode*) malloc(sizeof(freeNode)); 
    if(newNode == NULL)
      {
	warpedNewHandler() ;
      }

    // allocate a list node
    // adding element to the head of the list
    newNode->next = hashTable[value].headptr;
    hashTable[value].headptr = newNode; 
    // fill in the details
    newNode->blockPtr = space;
    hashTable[value].freesize++;
  }
  else { // list is large, we don't want to make it larger
    space = (char*)space - SIZE;
    free(space);
  }
  hashTable[value].currentFreeNode = hashTable[value].headptr;
  // uncomment the next line if you need to see the lists
  // printTable(value, bytes);
}

// findAndAssign function - gets the best fit and aligns the list
void*
MemoryManager::findAndAssign(int value, size_t size){
  size_t numBytes;
  freeNode* prevPtr, *ptr;
  void* tempPtr, *newptr;
  int found = 0 ;
  // first find the best fit node
  prevPtr = hashTable[value].headptr;
  for(ptr = hashTable[value].headptr; ptr != NULL; ptr = ptr->next){
    numBytes = *((int*)((char*)(ptr->blockPtr) - SIZE)) ; // go back and read size
    if(numBytes == size){
    found = 1;
    break;
    }
    prevPtr = ptr ;
  }
  if(found == 1){
    // set prevPtr to point to node->next cause you are taking
    // this node out of the list
    if(prevPtr == ptr){
      hashTable[value].headptr = ptr->next;
    }
    else {
    prevPtr->next = ptr->next;
    }
    tempPtr = ptr->blockPtr;
    ptr->next = NULL;
    //  ptr->blockPtr = NULL;
    free((void*)ptr);
    hashTable[value].freesize--;
    memset((void *)tempPtr, 0, size); // filling in padding with 0
    return tempPtr;
  }
  else { // no node found, bummer !! call system new
    newptr = malloc(size+SIZE); // newly allocated with space for size
    if(newptr == NULL)
      {
	warpedNewHandler() ;
      }

    memset((void *)newptr, 0, size+SIZE); // filling in the padding with 0
    *(int*)newptr = size ;
    return (((char*)newptr + SIZE)); // skip size of an integer 
  }
} // okay we found a fit or we didn't - indicated by found flag
 
// print function - prints out the list at a particular hash value
void
MemoryManager::printTable(int location, size_t bytes){
  int i = 0 ;
  cout << "***** Print Linked List @location = " << location << endl;
  freeNode* ptr = hashTable[location].headptr;
  while(ptr != NULL){
        i++;
	cout << "Element " << i << " @location = " << location 
         << " => size = " << bytes << endl ;
	ptr = ptr->next;
  }
}

#ifdef GLOBALMEMORYMANAGER
MemoryManager* mem = 0 ; // static declaration needed for new & delete

void* operator new(size_t bytes){
  register int value;
  void* ptr, *extraPtr;

  // workaorund for Linux machines 
  if ( mem  == (MemoryManager*)0) {
    mem = (MemoryManager *)malloc(sizeof(MemoryManager )) ;
    if(mem == NULL)
      {
	warpedNewHandler() ;
      }

    new (mem) MemoryManager();
  }

  value = mem->hash(bytes);// hash to location in hashtable
  if(mem->hashTable[value].freesize == 0){ // if no free blocks available
    // call system malloc to allocate memory
    // hey hey hey, pointer arithmetic alert
    ptr = malloc(bytes+SIZE); // newly allocated block with space for size
    if(ptr == NULL)
      {
	warpedNewHandler() ;
      }

    *(int*)ptr = bytes ;
    for(int i = 0; i < EXTRABLOCKS-1;i++){
      extraPtr = malloc(bytes+SIZE);
      if(extraPtr == NULL)
	{
	  warpedNewHandler() ;
	}

      *(int*)extraPtr = bytes;
      mem->deAllocate(((char*)extraPtr+SIZE), bytes);
    }
    return ((char*)ptr+SIZE); // skip two integer lengths
  }
  //  we can't directly assign the node here. Have to find the
  //  best fit and assign that node. so call findAndAssign procedure
  void* node = mem->findAndAssign(value, bytes);
  return node;
}

void operator delete(void* space){
//call deallocate

  space = (char*)space - SIZE ; // go back and read the size
  size_t bytes = *((int*)space);
  space = (char*)space + SIZE ; // bring ptr back to original location
  mem->deAllocate(space, bytes);
};

#endif
#endif
