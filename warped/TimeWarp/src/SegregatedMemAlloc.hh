#ifndef SEGREGATEDMEMALLOC_HH
#define SEGREGATEDMEMALLOC_HH
#include<iostream.h>
#include<stdlib.h>
#include "LogicalProcess.hh"
#include "config.hh"
#include "warpedNewHandler.hh"

typedef long PointerSize; // Uncomment this like if pointer's are long
// typedef int PointerSize; // Uncomment this if pointer's are int

// structure of each memory block header
struct Header {
  int tagH;
  int sizeH:31;
  void *linkf;
  void *linkb;
};

// structure of each memory block footer 
struct Footer {
  int tagF;
  int sizeF:31;
};

// structure for bounds table 

struct BoundsTableStruct {
  PointerSize MinAddress;
  PointerSize MaxAddress;
  BoundsTableStruct *next; 
};

// define the following Macros 

#define LinkF(m) ((Header *)m)->linkf // forward link of doubly linked list
#define LinkB(m) ((Header *)m)->linkb // backward link of doubly linked list
#define SizeH(m) ((Header *)m)->sizeH // size of the memory block 
#define TagH(m)  ((Header *)m)->tagH // 0 reserved 1 available
#define TagF(m)  ((Footer *)m)->tagF // tag in footer 
#define SizeF(m) ((Footer *)m)->sizeF // size of block


// Constants Also automatically defines the largest block 2^NoOfLists
#define NoOfLists     20
// The sizes in list i are less than or equal to 2^i and greater that 2^(i-1)
#define MinSize  10  // therefor offset min i is 5 should be power of 2 
#define MaxExtraSize  32  //if  available - requested > MaxExtraSize then split the block and return the extra to appropriate list  
#define NeedMoreMem 20  /// allocate more if we run out of memory
#define StartCollapse 10000// no of deletes after which the collapseing starts

// global new and delete 
void * operator new( size_t );
void operator delete( void *);


class SegregatedMemAlloc {
  friend class LogicalProcess;
 public:
  Header  availList[NoOfLists + 1]; // set up the head pointer for each list
  //BoundsTableStruct BoundsTable[200];
  BoundsTableStruct *HeadPtr;
  BoundsTableStruct *TailPtr;
  int NoOfEntries;
  // neee to maintain bounds of all memory allocated ?
  int NoDeletes;
  unsigned int memory;
  SegregatedMemAlloc();
  ~SegregatedMemAlloc();

  // search the lists for appropriate (first fit) and return it if
  // available - requested < 32 other wise split and return remainder to
  // appropriate list
  void *Allocate( size_t size);

  // return the deleted block to the lists and set pointers 
  void DeAllocate(void*, size_t size);
  
  // split a block into requred size and attach the remaining to
  // appropriate list
  bool SplitBlock(void *space, size_t size, int PowOfTwo);
  void * CollapseBlock(void *space, size_t size);
  
  void registerLP(LogicalProcess *){}
};
#endif














