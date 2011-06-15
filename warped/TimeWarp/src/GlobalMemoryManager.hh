#ifndef GLOBALMEMORYMANAGER_HH
#define GLOBALMEMORYMANAGER_HH


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



#include <stdlib.h>
#include <new.h>
#include "warped.hh"
#include "config.hh"

#include "warpedNewHandler.hh"

#define HASHTABLESIZE 1024  // size of hash table
#define EXTRABLOCKS 2      // number of block to new when new is called
#define LISTSIZE 20         // size of the free list
#define SIZE 32             // size of memory block
// memory reallocation functions/data

class MemoryManager{
  // global new & delete 
  friend void* operator new(size_t bytes);
 friend void operator delete(void* space);
public:
  
  MemoryManager();
  
  ~MemoryManager();

  // linked list Node
  struct freeNode{
    freeNode* next;    // store a ptr to the next node in the list 
    void* blockPtr;    // pointer to the allocated block
  };

  // hash table Node
  struct hashTableNode{
    // contains a pointer to a linked list
    // of free blocks
    freeNode* headptr; // store head and tail for easy insertion 
    freeNode* tailptr; // into the linked list
    freeNode* currentFreeNode; // currently free block
    int freesize; // indicates number of free blocks 
                  // in the linked list at this hashed spot
  };

  hashTableNode* hashTable;// hash table, will be created later
  int hash(int valueToHash);
  void createTable();
  void printTable(int location, size_t bytes);
  void deAllocate(void* space, size_t bytes);
  void* findAndAssign(int value, size_t size);
};





#endif



