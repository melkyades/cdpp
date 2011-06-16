#ifdef NEW_MEM_ALLOC

#include "NewMemAlloc.hh"
#include <iostream>

NewMemAlloc GlobalMemoryManager;

// The constructor for global memory manager is called automatically.
// It initializes the system to have atleast one block of memory.
// This is done by a call to the initialise(void) function

NewMemAlloc::NewMemAlloc()
{
  cerr << "Using new memory manager" << endl;
  listOfBlocks = NULL;
  initialise();
}

// The destructor walks thro the list of the blocks of memory allocated
// and frees them. A simple linked list traversal

NewMemAlloc::~NewMemAlloc()
{
  GlobalBlockList *deleteBlock = listOfBlocks;

  while (listOfBlocks != NULL) {
    listOfBlocks = listOfBlocks->link;
    
    free(deleteBlock->memory);
    free(deleteBlock);
  }

#ifdef NEW_MEM_STATS
  cout << "Total number of new's                : " << totalNews    << endl;
  cout << "Total number of deletes's            : " << totalDeletes << endl;
  cout << "Total number of blocks used          : " << newBlock + 1 << endl;
  cout << "  [ each of size ";
  cout << (INITIALLY_ALLOCATE * MULTIPLES_FACTOR / 1024) << " k ]"  << endl;
  cout << "Number of hits ( .ie. no splitting ) : " << totalHits    << endl;
  cout << "Number of last entry searches done   : ";
  cout << totalLastEntrySearches << endl;
  cout << "Number of times coalesing was done   : " << coaleseCount << endl;
#endif  
}

// This function inserts a given block of memory into the correct list
// Note : The list into which this block fits is directly determined by
//        the size of the block being inserted. Ofcourse large blocks are
//        simply added to the last entry.

void
NewMemAlloc::insertBlock(BlockHeader *insertBlock)
{
  register listNumber = getSize(insertBlock->sizeAndFlag) - 1;

  if (listNumber >= MAX_ENTRIES) {
    listNumber = MAX_ENTRIES - 1;
  }
  
  if (freeBlocksList[listNumber] != NULL) {
    freeBlocksList[listNumber]->prev = insertBlock;
  }
  
  insertBlock->prev = NULL;
  insertBlock->next = freeBlocksList[listNumber];

  freeBlocksList[listNumber] = insertBlock;
}

// This function allocates a chunk of memory of the size passed by
// the user to this function. The list of blocks are maintained as a
// stack.

BlockHeader *
NewMemAlloc::allocateChunkOfMemory(int sizeOfBlock)
{
  GlobalBlockList *newBlock;
  BlockHeader     *tempHeader;

#ifdef NEW_MEM_STATS
  newBlock++;
#endif
  
  newBlock = (GlobalBlockList *) malloc(sizeof(GlobalBlockList));

  newBlock->link   = listOfBlocks;
  newBlock->memory = (char *) malloc(sizeOfBlock * MULTIPLES_FACTOR);
  tempHeader       = (BlockHeader *) newBlock->memory;

  tempHeader->prev = NULL;
  tempHeader->next = NULL;
  setSize(tempHeader->sizeAndFlag, sizeOfBlock);
  markFree(tempHeader->sizeAndFlag);
  
  listOfBlocks     = newBlock;

  return tempHeader;
}

// This function initializes the memory management system. It itializes all
// the pointers, allocates a big block of memory ( user defined ) and inserts
// it into the appropriate list and gets out. This is called only once in the
// system, when the first block of memory is allocated by a call to new.

void
NewMemAlloc::initialise(void)
{
  register    counter;
  
  // Initialise all the list pointers to NULL to indicate that they do not
  // have any deleted blocks that can be reused.
  
  for(counter = 0; (counter < MAX_ENTRIES); counter++)
    freeBlocksList[counter] = NULL;

  // Allocate a big block of memory of the size specified by the user
  // defined constants and insert that block into the appropriate list in the
  // system.

#ifdef NEW_MEM_STATS
  totalNews = totalDeletes = totalHits = newBlock = coaleseCount = 0;
  totalLastEntrySearches   = 0;
#endif
  
  freeBlocksList[MAX_ENTRIES - 1] = allocateChunkOfMemory(INITIALLY_ALLOCATE);
}

// This function removes a given free block from the corresponding list.
// Note : Since the free lists are maintained as two dimensional arrays,
// ----   just the node to be deleted is enough. With it we can find the
//        previous and next node in the list and adjust their pointers to
//        indicate that the node has been deleted.
//
//        The function checks if a previous or next nodes exists for the node
//        being deleted. If they exists then their corresponding next and prev
//        pointers are set. Simple doubly-linked list deletion algorithm.
//
//        Since function is small and used heavily, inlining it will do good.

inline
void
NewMemAlloc::removeFromList(BlockHeader *toRemove)
{
  BlockHeader *previous;
  BlockHeader *next;
  int         tempSize;
  
  if ((previous = toRemove->prev) != NULL) {
    previous->next = toRemove->next;
  }
  else {
    if ((tempSize = getSize(toRemove->sizeAndFlag)) > (MAX_ENTRIES - 1)) {
      tempSize = MAX_ENTRIES;
    }
    freeBlocksList[(--tempSize)] = toRemove->next;
  }
  
  if ((next = toRemove->next) != NULL) {
    next->prev = toRemove->prev;
  }
}

void
NewMemAlloc::coaleseMemory(char *bigBlock, int MaxOffset)
{
  BlockHeader *tempBlock    = (BlockHeader *) bigBlock;
  register     Offset       = 0;
  BlockHeader *coaleseBlock = NULL;
  char         coaleseFlag  = 0;
  register     coaleseCount = 0;
  int          coaleseSize  = 0;

  // cout << "------- Before coalesing --------\n";
  // printList();

#ifdef NEW_MEM_STATS
  coaleseCount++;
#endif
  
  MaxOffset *= MULTIPLES_FACTOR;
  
  while (Offset < MaxOffset) {
    if (!inUse(tempBlock->sizeAndFlag)) {
      if (coaleseFlag) {
	// We are already coalesing a block. Simply add this block to the
	// one we are coalesing already and delete this block from its
	// corresponding list.
	coaleseSize += getSize(tempBlock->sizeAndFlag);
	removeFromList(tempBlock);
	coaleseCount++;
      }
      else {
	// Start coalesing from this block and proceed.
	coaleseFlag  = 1;
	coaleseCount = 0;
	coaleseSize  = getSize(tempBlock->sizeAndFlag);
	coaleseBlock = tempBlock;
      }
    }
    else {
      if (coaleseFlag) {
	// Well we have an block that is currently used.
	// We were coalesing a block. if we have coalesed .ie. joined two
	// or more blocks then we need to remove the coalesed block from
	// its list and promote it to its right position in the new list
	// We can call removeFromList() as that function does not use the
	// size of the block for deletion. It simply manipulates the pointers
	// that are unchanged.
	
	if (coaleseCount > 0) {
	  removeFromList(coaleseBlock);
	  setSize(coaleseBlock->sizeAndFlag, coaleseSize);
	  markFree(coaleseBlock->sizeAndFlag);
	  insertBlock(coaleseBlock);
	}

	coaleseFlag  = 0;
	coaleseCount = 0;
	coaleseSize  = 0;
      }
    }

    Offset   += (getSize(tempBlock->sizeAndFlag) * MULTIPLES_FACTOR);
    tempBlock = (BlockHeader *) (bigBlock + Offset);
  }

  if (coaleseFlag) {
    if (coaleseCount > 0) {
      removeFromList(coaleseBlock);
      setSize(coaleseBlock->sizeAndFlag, coaleseSize);
      markFree(coaleseBlock->sizeAndFlag);
      insertBlock(coaleseBlock);
    }
  }
  
  // cout << "========= After coalesing ==========\n";
  // printList();
}

// This function traverses the blocks in the last entry to check if
// some free block is available. If so carve a chunk from it for the
// user and adjust the list to reflect the operation.
//
// If no free chunk of memory large enough to hold the block was found
// then coalese memory.
//
// Still no blocks of the right size are found then allocate a new block
// of memory and return a part of it to the user.

inline
void *
NewMemAlloc::checkLastEntry(int size, char coaleseFlag)
{
   BlockHeader *searchBlock, *tempBlock;
   int         tempSize;
   
   searchBlock = freeBlocksList[MAX_ENTRIES - 1];

#ifdef NEW_MEM_STATS
   totalLastEntrySearches++;
#endif
   
   while (searchBlock != NULL) {
     if ((int) getSize(searchBlock->sizeAndFlag) >= size) {
       // Okay. We got a guy who can be used to allocate memory for our man.
       // Here we need to add one to the size of the block that we need to
       // to alloacate since the size s now converted in terms of the list
       // entries .ie. size now starts from 0 rather than 1
       
       tempBlock       = (BlockHeader *) ( ((char *) searchBlock) +
					   (size * MULTIPLES_FACTOR));
       
       tempBlock->prev = searchBlock->prev;
       tempBlock->next = searchBlock->next;
       tempSize        = (getSize(searchBlock->sizeAndFlag) - size);
     

       // Do we need to move this block up in the list ?
       
       if (tempSize <= (MAX_ENTRIES - 1)) {
	 // Yes. So delete this guy from this list and insert it in the
	 // std::list which points to the right size
	 removeFromList(searchBlock);
	 setSize(tempBlock->sizeAndFlag, tempSize);
	 markFree(tempBlock->sizeAndFlag);
	 insertBlock(tempBlock);
       }
       else {
	 setSize(tempBlock->sizeAndFlag, tempSize);
	 markFree(tempBlock->sizeAndFlag);
	 if (tempBlock->prev == NULL) {
	   freeBlocksList[MAX_ENTRIES - 1] = tempBlock;
	 }
	 else {
	   tempBlock->prev->next = tempBlock;
	 }
       }
       
       setSize(searchBlock->sizeAndFlag, size);
       markInUse(searchBlock->sizeAndFlag);
       
       return (void *) ( ((char *) searchBlock) + 8);
     }
     else {
       // The current block is too small. So check the next block
       searchBlock = searchBlock->next;
     }
   }

   // Okay. Now comes the worst case in the system. Now we need to
   // start coalising blocks to get free memory from the system.
   // If event coalising fails then allocate a new block and return
   // the necesseary size back to the user.

   if (coaleseFlag) {
     GlobalBlockList *head = listOfBlocks;

     while (head != NULL) {
       coaleseMemory(listOfBlocks->memory, INITIALLY_ALLOCATE);
       head = head->link;
     }
   }
		 
   return NULL;
}

// Allocate a block fo user defined size. The algorithm proceeds like this
//
// Step 1 : Add block header overhead to the required size
// Step 2 : Add "x" to above size, such that (size % MULTIPLES_FACTOR) = 0
// Step 3 : Find in which block that entry fits.
// Step 4 : Check if that block has an free block.
// Step 5 : If it has a free block adjust the list and return block
// Step 6 : If not search in the next entries for a new block.
// Step 7 : If no free block was found, then Coalese the memory
// Step 8 : If still not enough memory was found allocate a new block and
//          split block and return the memory to the user.

void *
NewMemAlloc::allocateBlock(size_t size)
{
  int         extraBytes;
  register    listNumber;
  int         *returnBlock;
  BlockHeader *searchBlock, *tempBlock;
  char        iterate = 2;
  
  // The block inuse overhead is only size of integer.
  // So need to add only sizeof(int) to the size passed to the function.

#ifdef NEW_MEM_STATS
  totalNews++;
#endif
  
  size += 8;

  if ((extraBytes = (size % MULTIPLES_FACTOR)) != 0) {
    size +=  MULTIPLES_FACTOR - extraBytes;
  }
  
  // Note : This is as good as saying listNumber = size / MULTIPLES_FACTOR
  // ----   assuming MULTIPLES_FACTOR is a power of two whcich is specified
  //        by the user as a constant MULTIPLES_POW_2
  
  listNumber = (size >> MULTIPLES_POW_2) - 1;
  size       = listNumber + 1;
  
  // The iteration loop does the following stuff in each iteration.
  // (a). Iteration #1
  //        Check if the list has blocks that can directly satisfy 
  //        the current memory request.
  // (b). Iteration #2
  //        If we come to the next iteration, in the first iteration,
  //        coalesing of blocks would have been done. So need to search
  //        the list once again to find out if we can satisfy the request
  // (c). On termination
  //         Even after coalesing memory, our request cannot be satified so
  //         allocate a new block and allocate a chunk of memory to it
  
  while (iterate) {
    listNumber = (size - 1);
    do {
      // Check if the freeListOfBlocks[listNumber] has some free blocks whcih
      // can be reused.
      
      if (freeBlocksList[listNumber] != NULL) {
	// Okay... We have an unused block of memory. So remove it from the
	// std::list and then set the up the used bit etc. before returning to user
	
	returnBlock = (int *) (searchBlock = freeBlocksList[listNumber]);
	
	removeFromList(searchBlock);
	
	if ((size_t) getSize(searchBlock->sizeAndFlag) > size) {
	  tempBlock         = (BlockHeader *) ( ((char *) searchBlock) + (size * MULTIPLES_FACTOR));
	  
	  tempBlock->prev   = searchBlock->prev;
	  tempBlock->next   = searchBlock->next;
	  
	  setSize(tempBlock->sizeAndFlag, (getSize(searchBlock->sizeAndFlag) -
					   size));
	  markFree(tempBlock->sizeAndFlag);
	  
	  insertBlock(tempBlock);
	}
	
	setSize((*returnBlock), size);
	markInUse((*returnBlock));
	
#ifdef NEW_MEM_STATS
	totalHits++;
#endif	
	return (void *) (returnBlock + 2);
      }
      else
	listNumber++;
    } while (listNumber < (MAX_ENTRIES - 1));
    
    // Now, we need to check the last entry in the list. Here we need to
    // traverse the list to find the block of the right size needed.
    // incase we fail then coalese memory. If we still fail then allocate a
    // new chunk of memory and return the right value to the user. This stuff
    // is done by the following function.
    
    // The function was put in just make the code more readable.
    
    if ((returnBlock = (int *) checkLastEntry(size, (iterate == 2))) == NULL) {
      // Well, the last entry too did not have blcoks of the size we need
      // It would have coalesed the blocks for us. So we need to walk thro
      // the list of coalesed blocks again to find out if we have the block
      // of the size we would need. Assuming that coalesing has worked. For
      // now we assume that. Later a state variable would be set to indicate
      // how coalesing performed for us.
      
      if (iterate == 1) {
	// Well, everything has now failed. So all we can do now is to
	// allocate a new block of memory and use it. That is it
	// This is done outside the loop.
	
	break;
      }
    }
    else {
      return (void *) returnBlock;
    }
    
    iterate--;
  }

  // The worst case. Allocate more memory and return a chunk from that
  
  insertBlock(allocateChunkOfMemory(INITIALLY_ALLOCATE));

  return checkLastEntry(size, 0);
}
    
// This function gets a pointer from the user and uses the header to get
// the right information about the block and inserts the block into the
// correct list. This is a relatively a simple process.
//
// Note : When we returned a block of memory back to the user when he asked
// ----   for new memory after adding sizeof(int) to it. This was done to
//        keep the information in the header of each block transpartent from
//        the user. Now we need to subtract the equivalent number from the
//        block being deleted to get to the header correctly.

void
NewMemAlloc::deleteBlock(void *deletePtr)
{
  BlockHeader *deleteBlock = (BlockHeader *) ( ((char *) deletePtr) - 8);

#ifdef NEW_MEM_STATS
  totalDeletes++;
#endif
  
  markFree(deleteBlock->sizeAndFlag);
  GlobalMemoryManager.insertBlock(deleteBlock);
}

// The globally overloaded new function. The new function simply calls the
// GlobalMemoryManager functions to get things done.
// Note : Since GlobalMemoryManager is defined globally its constructor would
// ----   have been called earlier and the system would have already
//        initialized itself. Hence here we need not make a special check.

void * operator new(size_t size)
{
  return (GlobalMemoryManager.allocateBlock(size));
}

// The globally overloaded delete function. This function simply calls the
// GlobalMemoryManager functions to get the actual things done.
// Note : If the user passes illegal pointers to this function then the
// ----   function will simply crash or may not work properly. So it the
//        user's reponsibility to delete the right pointers.

void operator delete(void *deletePtr, size_t)
{
  GlobalMemoryManager.deleteBlock(deletePtr);
}

// Display the freeBlocksList

void
NewMemAlloc::printList(void)
{
  register     counter;
  BlockHeader *temp;

  for(counter = 0; (counter < MAX_ENTRIES); counter++) {
    if (freeBlocksList[counter] != NULL) {
      cout << "   freeBlocksList[" << counter << "] = ";
      
      temp = freeBlocksList[counter];
      while (temp != NULL) {
	cout << (int) getSize(temp->sizeAndFlag) << ", ";
	temp = temp->next;
      }

      cout << endl;
    }
  }
}

#endif
