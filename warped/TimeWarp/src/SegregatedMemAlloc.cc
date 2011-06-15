#ifndef SEGREGATEDMEMALLOC_CC
#define SEGREGATEDMEMALLOC_CC
#include "SegregatedMemAlloc.hh"

SegregatedMemAlloc::SegregatedMemAlloc(){



 
  NoOfEntries = 0;

  // initialize the list headers
  for(int i = 0; i <= NoOfLists; i ++){
    LinkF(availList + i) = LinkB(availList +i) = availList + i;
    TagH(availList + i ) = 0; 
    SizeH(availList + i ) = 0; 
  }
  
  int *memory = (int *)malloc( 1<< NoOfLists );
  if(memory == NULL)
    {
      warpedNewHandler() ;
    }
  
  HeadPtr =(BoundsTableStruct *) malloc (sizeof( BoundsTableStruct)); 
  if(HeadPtr == NULL)
    {
      warpedNewHandler() ;
    }

  HeadPtr->MinAddress = (PointerSize) memory;
  HeadPtr->MaxAddress = (PointerSize) memory + (1 << NoOfLists);
  HeadPtr->next = NULL;
  TailPtr = HeadPtr;

  // attach the this block to the last list 
  LinkF(memory) = LinkB(memory) = availList + NoOfLists;
  LinkF(availList +  NoOfLists) = LinkB(availList + NoOfLists) = memory;
  
  // the header tags  
  TagH(memory) = 1;
  SizeH(memory) = (1 << NoOfLists);

  // the footer tags  

  TagF((void *)((char *)memory + (1 << NoOfLists) - 8)) = 1;
  SizeF( (void *)((char *)memory + (1 << NoOfLists) - 8)) = (1 << NoOfLists);
  cout << " Done Initializing " << endl;

}


SegregatedMemAlloc::~SegregatedMemAlloc(){
  free((void *)memory);
}

void*
SegregatedMemAlloc::Allocate(size_t size){
  
  int k;
  int Offset , align;

  int found;
  int ListNo;
  int extraSize;
  void *ReturnVal;
  ReturnVal = NULL; 

  ListNo = 0;
  found = 0;
  k = MinSize;;
  Offset = 0;

 
  // align on a 8 byte boundary remember double !
  if (size < 8)
    size = size + ( 8 - size);
  else {
    align = size % 8;
    size = size + ( 8 - align);
  }
  
  size = size + 16; // allow space for tag and size in header and tag in the footer
 
  
  while ((1 << k) < (int)size )
    k++;

  ListNo = k;

  while(found == 0 && ListNo <= NoOfLists){
  // start searching from the k th list only 
    if( LinkF(availList + ListNo) != availList + ListNo ) { 
      // available so take the first guy and return it 
      ReturnVal = LinkF(availList + ListNo);
      found = 1;
      // remove from list
      if (TagH(ReturnVal) == 0 ){ 
	cout << " Used Block In Free List  !!!! " << endl;
      }
      LinkF(availList+ListNo) = LinkF(ReturnVal);
      LinkB(LinkF(ReturnVal)) =  availList + ListNo;
      LinkF(ReturnVal) = NULL;
      LinkB(ReturnVal) = NULL;
	
      TagH(ReturnVal) = 0;
      TagF((void *)((char *)ReturnVal + SizeH(ReturnVal) - 8 )) = 0; //used

      // now check if this guy is too big
      // note that this guy cannot be bigger that "size" 
      extraSize = SizeH(ReturnVal) - size; 
      if( extraSize  >=  MaxExtraSize) {
	// split the guy 
	SplitBlock( ReturnVal , size , ListNo  );
	SizeH(ReturnVal) = size;
	SizeF((void *)((char *)ReturnVal + size - 8 )) = size;
	TagF((void *)((char *)ReturnVal + size - 8 )) = 0;
	
      }
    }
      else {
	ListNo ++;
      }
  }
  if (found == 1){
    return ((void *)((char *)ReturnVal + 8 ));
    
    // block was found in the free list 
  }
  else {
   
    // block still not found so allocate more (4 K)
    int *memory = (int *)malloc(1 << NeedMoreMem);

    if(memory == NULL)
      {
	warpedNewHandler() ;
      }
   // cout << "Starting Address " << (int)memory << " Ending Address " 
//	 <<( (int)memory + ( 1 << NeedMoreMem)) << endl; 
    // update the bounds table
  //  BoundsTableStruct  NewNode;
  //  NewNode.MaxAddress = (int)memory + ( 1 << NeedMoreMem);
  //  NewNode.MinAddress = (int)memory;
    BoundsTableStruct *NewNode;
    NewNode = (BoundsTableStruct *)malloc(sizeof( BoundsTableStruct));
    if(NewNode == NULL)
      {
	warpedNewHandler() ;
      }

    
    NewNode->MinAddress = (PointerSize) memory;
    NewNode->MaxAddress =  (PointerSize) memory + (1 << NeedMoreMem);
    NewNode->next = NULL;
    TailPtr->next = NewNode;
    TailPtr = NewNode;
    
//    Boundstable[NoOfEntries] = NewNode;
    //NoOfEntries ++;
   
    // return to list
    TagH(memory) = 0;
    SizeH(memory) = ( 1 << NeedMoreMem );
    TagF((void *)((char *)memory + (1 << NeedMoreMem) - 8)) = 0;
    SizeF((void *)((char *)memory + (1 << NeedMoreMem) - 8)) = (1 << NeedMoreMem);
    //cout << " size " << SizeH(memory) << " SIZE " << size << endl;
    if ((SizeH(memory) - size) >= MaxExtraSize){
      SplitBlock(memory , size , NeedMoreMem );
      SizeH(memory) = size;
      TagF((void *)((char *)memory+ size - 8 )) = 0;
      SizeF((void *)((char *)memory + size - 8)) = size;
     
    }
   
  //  ReturnVal = (void *)memory;
    return ((void *)((char *)memory + 8));    
  }  
  
}

bool
SegregatedMemAlloc::SplitBlock(void *space , size_t size , int PowOfTwo) {
  // basically space has more than required so split the block into size and
  // rest and return the rest to availList and return the the other
  int extraSize;
  void *secondHalf;
  int k; 
  secondHalf = (void *)((char *)space + (int)size );
  extraSize = SizeH(space) - size;
  k = PowOfTwo;

  
  TagH(secondHalf) = 1;
  SizeH(secondHalf) = extraSize;
  TagF((void *)((char *)secondHalf + extraSize - 8)) = 1;
  SizeF((void *)((char *)secondHalf + extraSize - 8)) = extraSize;
  void *l = secondHalf;
  //  void *m = (void *)((char *)secondHalf + 8);
  // return the split guy to the correct list 
  //DeAllocate( m  ,(int) extraSize);
  //k = MinSize; 
   while ( (1 << k ) >= (int) extraSize)
     k--;
    
  
   // insert the block into availList 

  LinkF(l) = LinkF(availList + k);
  LinkB(l) = availList + k;
  if (LinkB(availList + k) == (availList + k))
    LinkB(availList + k) = l;
  else LinkB(LinkF(availList + k)) = l;
  LinkF(availList + k) = l;
   
  return true;
}

void
SegregatedMemAlloc::DeAllocate(void *space , size_t size){
  // takes a block and returns it to a appropriate free list 
  // also perform collapsing if the previous block and/or next block is free
  
  
  void *l = (char *)space - 8;
  
  int k;
  k = MinSize; 
 
  int lettag = TagH(l);
  
  if ( lettag == 1 ){
    while ( (1 << k ) < (int) size)
      k++;
  }
  else {
    size = SizeH(l);
    while ( (1 << k ) < (int) size)
      k++;
  }
  k--; 

  if( lettag == 0 ){
    NoDeletes ++; 
    if ( NoDeletes == StartCollapse ){  
      l = CollapseBlock(l , size);
      size = SizeH(l);
      NoDeletes = 0;
    }
     // set size and tags for current block 
    TagH(l)  = 1; // unused 
    //SizeH(l) = size;
    TagF((void *)((char *)l + size - 8))   = 1; 
    SizeF((void *)((char *)l + size - 8 )) = size;
  // insert the block into availList 
  }
  // insert the block into availList 
  // but first find its size
   
  if ((int)size > (1 << k)){
    while (( 1 << k) < (int)size ) 
      k++;
    k --;
  }

  LinkF(l) = LinkF(availList + k);
  LinkB(l) = availList + k;
  if (LinkB(availList + k) == (availList + k))
    LinkB(availList + k) = l;
  else LinkB(LinkF(availList + k)) = l;
  LinkF(availList + k) = l;
  
}
  
void*
SegregatedMemAlloc::CollapseBlock(void *space ,size_t size){
  
  int lowBound;
  int highBound;
  void * nextBlock;
  void *prevBlock;
  void *prevBlockFooter;
  lowBound = 0; 
  highBound = 0; 
  BoundsTableStruct *tmp;
  tmp = HeadPtr;
  
  while (tmp != NULL ){
    if (((PointerSize) space >= tmp->MinAddress) &&
	((PointerSize) space <= tmp->MaxAddress)) {
      lowBound  =  tmp->MinAddress;
      highBound =  tmp->MaxAddress;
    } 
    tmp = tmp->next; 
  }

  if ( highBound == 0  ) 
    cout << " Not Found the Address in bounds Table " << endl;
  nextBlock = ((char *)space + SizeH(space));
  prevBlockFooter  = ((char *)space - 8);
  prevBlock = (char *)space - SizeF(prevBlockFooter);
  
  if ( (PointerSize) nextBlock <  highBound ){
    // ok the next block is in range
    // cout << " TagH(nextBlcok) " << TagH(nextBlock) << endl;
    if (TagH(nextBlock) == 1){ // if unused 
      // collapse them 
      size = size  + SizeH(nextBlock);
      // remove the nextBlock from availlist 
      LinkF(LinkB(nextBlock)) = LinkF(nextBlock);
      LinkB(LinkF(nextBlock)) = LinkB(nextBlock); 
    }
  }
  
  if ((PointerSize) prevBlock > lowBound ){
    // ok the next block is in range
    // cout << " TagH(prevBlcok) " << TagH(prevBlock) << endl;
    if (TagH(prevBlock) == 1){ // if unused
      // collapse
      size = size + SizeH(prevBlock);
      void *x =(void *)( (char *)space - SizeH(prevBlock)); 
      // remove the prev block from availlist
      LinkF(LinkB(prevBlock)) = LinkF(prevBlock);
      LinkB(LinkF(prevBlock)) = LinkB(prevBlock); 
      space = x;
    }
  }
 
  SizeH(space)= size;

  return space; 

}


#ifdef SEGREGATEDMEMALLOC

SegregatedMemAlloc*  mem  = 0 ;

void *operator new( size_t size ){
  void *l;

  if ( mem  == (SegregatedMemAlloc*)0) {
    mem = (SegregatedMemAlloc *)malloc(sizeof(  SegregatedMemAlloc )) ;
    if(mem == NULL)
      {
	warpedNewHandler() ;
      }

    new (mem) SegregatedMemAlloc();
  }
  l = mem->Allocate(size);
  return l; 
  
}
void operator delete( void *m) {
  int size;
  int tag;
  

  void *l = (char *)m - 8;
   size = SizeH(l);
    tag = TagH(l );
  //cout << " Delete  size :" << size << " tag :" << tag << endl; 
  mem->DeAllocate( m , size);


}
#endif
#endif 


