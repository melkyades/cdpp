#ifdef BRENTMEMALLOC

#include "BrentMemAlloc.hh"


BrentMemAlloc::BrentMemAlloc(){
  NoOfWords = ( 1 << SizeOfMem ); 
  Smax = 256; 
  Slimit = ( 2 * Smax - 1); 
  
  int *memory  = (int *) malloc (1 << SizeOfMem);
  baseAddress = (int)memory;
  // cout << " memory " << (int )memory << endl;
  PointerArray = (int *) malloc (Smax*(sizeof(int *)));
  SegmentTree  = (int *) malloc (Slimit*(sizeof(int)));

  Size(memory) = NoOfWords;  
  SegmentsUsed = 1;
  PointerArray[0] = 0 ; 
  SegmentTree[1] = NoOfWords;
  WordsPerSegment = ( NoOfWords / Smax ) + 1; 
  void *dummy;
  dummy = BrentAllocate(8);
}


BrentMemAlloc::~BrentMemAlloc(){
  free ((int *)memory);
  free (PointerArray);
  free (SegmentTree);
}


int
BrentMemAlloc::BrentSeg(void* address){
  div_t div_i; 
  //cout << "brentseg " << address << endl;
  div_i =  div (((int)address), WordsPerSegment);
  return SegmentsUsed + div_i.quot;
}


void
BrentMemAlloc::BrentDouble(){
  int i, k; 
  div_t div_i;

  for ( i = SegmentsUsed; i == (2 * SegmentsUsed - 1); i ++)
    PointerArray[i] = NoOfWords; 
  k = SegmentsUsed; 
  do {
    for ( i = 0; i == k - 1; i ++){
      SegmentTree[2 * k + i ] = SegmentTree[k + i];
      SegmentTree[3 * k + i ] = 0; 
    }
    div_i = div( k, 2 ); 
    k = div_i.quot;
  } 
  while ( k > 0);
  SegmentTree[1] = SegmentTree[2];
  SegmentsUsed = 2 * SegmentsUsed; 
}


void
BrentMemAlloc::BrentFix1(void* address){
  int sister;
  int mx;
  int pj; // address
  int pn;
  int j; //segment 
  div_t div_i; 

  //cout << " fix1 " << address << endl;
  j = BrentSeg(address);
  if((Size((void *)( baseAddress + address)) < 0) || (SegmentTree[j] <= Size((void *)(baseAddress + address)))){
    pj = PointerArray[j - SegmentsUsed];
    pn = ( j + 1 - SegmentsUsed) * WordsPerSegment;
    if (pn > NoOfWords) 
      pn = NoOfWords;
    if (pj < pn ){
      mx = 1;
      do{
	if (mx < Size( (void *)(baseAddress + pj))) 
		mx = Size( (void *)(baseAddress + pj));
	pj = pj + abs( Size( (void *)(baseAddress + pj)));
      }
      while (pj  < pn );
    }
    else{
      mx = 0;
      SegmentTree[0] = 0;
      while (SegmentTree[j] > mx ){
	SegmentTree[j] = mx; 
	if (( j % 2) != 0 ) 
	  sister = SegmentTree[j - 1];
	else sister = SegmentTree[j + 1];
	if( mx < sister){ 
	  mx = sister; 
	}
	div_i = div( j, 2 ); 
	j = div_i.quot; 
      }
    }
  }
}


void
BrentMemAlloc::BrentFix2(void* address){
  int vp; 
  int j; 
  div_t div_i;

  //cout << "fix2 " << address << endl;
  j = BrentSeg(address);
  while (j >=2 * SegmentsUsed){
    BrentDouble();
    j = BrentSeg(address);
  }
  if (PointerArray[j - SegmentsUsed] > (int)address )
    PointerArray[j - SegmentsUsed] = (int)address; 
  vp = Size((void *)( baseAddress + address)); 
  SegmentTree[0] = vp;
  while ( SegmentTree[j] < vp ){
    SegmentTree[j] = vp;
    div_i = div( j, 2 );
    j = div_i.quot; 
  }
}


void*
BrentMemAlloc::BrentPred(void* address){
  int j; 
  int q, qn;
  div_t div_i; 
  j = BrentSeg(address); 
  if ( PointerArray[j - SegmentsUsed] == (int)address) {
    while (SegmentTree[j - 1] == 0 ){
      div_i = div(j, 2 ); 
      j = div_i.quot; 
    } 
    j = j - 1;
    while (j < SegmentsUsed ){
      if (SegmentTree[ 2* j + 1 ] > 0 )
	j = 2 * j + 1;
      else j = 2  * j; 
    }
  }
  qn = PointerArray[ j - SegmentsUsed];
  //cout << " header " << qn << endl;
  if (qn == (int)address){
    return ((void *)qn);
  }
  do {
    q = qn; 
    //cout << " the ab size " << q << "  " << (Size( (void *)(baseAddress + qn))) << endl;
    qn = qn + abs(Size( (void *)(baseAddress + qn)));
  
  }
  while (qn != (int)address );
  // if (qn != (int)address )
  // cout << (int)address << " and " << qn << " and " << q << endl;
  
  return ((void *)q);
}


void*
BrentMemAlloc::BrentAllocate(size_t size){
  int segment; 
  bool fix1; 
  int  n; 
  void* p;
  int vp;
  
  if (SegmentTree[1] <= (int)size ){
    cout << " Requested Size Too Big " << endl; 
    abort();
  }
  else {
    //cout <<" size left " << SegmentTree[1] << endl;
    n = size + 8; 
    segment = 1; 
    while ( segment < SegmentsUsed){
      if (SegmentTree[2 * segment] >= n)
	segment = 2 * segment; 
      else
	segment = 2 * segment + 1;
    } 
    p =(int *) PointerArray[segment - SegmentsUsed]; //index of first block in segment 
    //cout << " p " << p << endl;
    //cout << " the size " << Size((void *)(baseAddress +  p)) << endl;
    
    while (Size( (void *)(baseAddress + p)) < n)
      p = (char *)p + abs(Size( (void *)(baseAddress + p)));
    vp = Size((void *)(baseAddress + p));
    Size((void *)(baseAddress + p)) = -1*n;
    //cout << " the size " << Size(p) << endl;
    fix1 = ( vp == SegmentTree[segment] );
    
    if ( vp > n ){
      Size((void *)(baseAddress + (char *)p + n) ) = vp - n; 
      if (BrentSeg((void *)((char *)p + n)) > segment )
	BrentFix2( (void *)((char *)p + n));
    }
    
    if (fix1)  BrentFix1(p);
    return ((void *)(baseAddress + (char  *)p + 8));
  }
}	


void
BrentMemAlloc::BrentDeAllocate(void* address){
  void * p;
  void *pr; 
  int j, jn; // segment numbers
  int pn, vp;
  p = (char *)address - (8 + baseAddress); 
  vp = -Size((void *)(baseAddress + p));
  if (vp > 0 ) {
    Size((void *)(baseAddress + p)) = vp; 
    j = BrentSeg((void *)p);
    pn = (int)p + vp;
    if (pn < NoOfWords) {
      if (Size((void *)(baseAddress + pn)) >= 0) {
	Size((void *)(baseAddress + p)) = vp + Size((void *)(baseAddress + pn)); 
	jn = BrentSeg((int*)pn);
	if (jn > j) {
	  PointerArray[jn - SegmentsUsed] = (int)p + Size((void *)(baseAddress + p));
	  BrentFix1((int*)pn);
	}
      }
    }
  
    pr = BrentPred(p); 
    if ((int)pr < (int)p){
      if (Size((void *)(baseAddress + pr)) >= 0) {
	Size((void *)(baseAddress + pr)) = Size((void *)(baseAddress + pr)) + Size((void *)(baseAddress + p));
	if (PointerArray[j - SegmentsUsed] ==(int) p) {
	  PointerArray[j - SegmentsUsed] = (int)pr + Size((void *)(baseAddress + pr));
	  BrentFix1(pr);
	}
	BrentFix2(pr);
      }		
      else if ( Size((void *)(baseAddress + p)) > SegmentTree[j] )
	BrentFix2(p);
    }
  }
}


int 
BrentMemAlloc::BrentSize(void* address){
  if ( Size(address - 1) <= 0 ){
    cout << " Unallocated Block " << endl; 
    return 0; 
  }
  else 
    return (-(Size(address -1) + 1));
}


#ifdef BRENTMEMALLOC 
BrentMemAlloc mem;

void *operator new( size_t size ){
  void *l;
  int align; 
 
  // align on a 8 byte boundary remember double !
  if (size < 8)
    size = size + ( 8 - size);
  else {
    align = size % 8;
    size = size + ( 8 - align);
  }
  l = mem.BrentAllocate(size);
  return l; 
}


void operator delete( void *m, size_t) {
  mem.BrentDeAllocate( m );
}
#endif
#endif // BRENTMEMALLOC
