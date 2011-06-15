#ifdef BRENTMEMALLOC

#ifndef BRENTMEMALLOC_HH
#define BRENTMEMALLOC_HH

#include<iostream.h>
#include<stdlib.h>
//#include "LogicalProcess.hh"
#include "config.hh"

struct Bheader {
  int size ; 
};

#define Size(m) ((Bheader *)m)->size 
#define SizeOfMem 20 // The size of the block initially allocated 2 ^ 15

// global new and delete 
void * operator new( size_t  );
void operator delete( void *, size_t );

class BrentMemAlloc{
 
private: 
  int NoOfWords ;
  int Smax ;
  int Slimit ; 
  int* memory ; 
  int baseAddress ;
  int *SegmentTree;
  int *PointerArray; 
  int *Vsize ; 
  int SegmentsUsed ; 
  int WordsPerSegment ; 
  
public:
  // the constructor
  BrentMemAlloc();
  
  // the destructor 
  ~BrentMemAlloc();
  
  // returns the index of segment containing address 
  int 
  BrentSeg(void* address);

  // doubles the number of segments actually in use assuming
  // SegmentsUsed < Smax / 2
  void 
  BrentDouble();

  // housekeeping necessary if  CW p is allocated or merged with block on 
  // its left in a different segment 
  void
  BrentFix1(void* address);

  // housekeeping necessary after a block is freed or merged on 
  //its right or created by splitting 
  void
  BrentFix2(void* address) ;
  
  // returns the predecessor of block p 
  void* 
  BrentPred(void* adddress);

  // return block of atleast size 
  void*
  BrentAllocate(size_t size);
  
  // deallocate the block 
  void
  BrentDeAllocate(void* address);

  // return size of block 
  int
  BrentSize(void *address);

};
#endif // BRENTMEMALLOC_HH

#endif // BRENTMEMALLOC
