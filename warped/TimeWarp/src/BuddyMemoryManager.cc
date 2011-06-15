#ifdef BUDDYMEMORYMANGER
#ifndef BUDDYMEMORYMANAGER_CC
#define BUDDYMEMORYMANAGER_CC
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


#include "BuddyMemoryManager.hh"

BuddyMemoryManager::BuddyMemoryManager( ){
  //allocate 2^Msize memory block
  
  memory = (unsigned *)malloc( 1 << Msize );
  if(memory == NULL) {
    warpedNewHandler();
  }
  
  
  //initialize the ptr's LinkF and LinkB
  for( int i = 0; (i < Msize); i++) {
    LinkF(avail + i) = LinkB(avail + i) = avail + i;
  }
  
  LinkF(avail + Msize) = LinkB(avail + Msize) = memory;
  LinkF(memory) = LinkB(memory) = avail+Msize;
  
  Kval(memory) = Msize;
  Tag(memory)  = 1;
  Mom(memory)  = (int *) memory;
}

BuddyMemoryManager::~BuddyMemoryManager(){
  free (memory) ;
}

BuddyMemoryManager* mem = 0 ;//  declaration of BuddyMemoryManager 

//globally overloaded new operator 
void * operator new( size_t size ){
  
  int k = ksize;
  int j;
  void *p;
  
  if ( mem  == (BuddyMemoryManager*)0) {
      mem = (BuddyMemoryManager *)malloc(sizeof(  BuddyMemoryManager )) ;
      if(mem == NULL)
	{
	  warpedNewHandler() ;
	}
      
      new (mem) BuddyMemoryManager();
  }
  
  size += 8;
 
  
  while( (int)size > (1<<k) )
    k++;
  
  j = k;
 while( j <= Msize && LinkF(&mem->avail[j]) == &mem->avail[j] )
    j++;

  if( j > Msize ) {

    j=Msize;
    int *memory = (int *)malloc( 1 << Msize );
    if(memory == NULL)
      {
	warpedNewHandler() ;
      }

    LinkF(mem->avail+Msize) = LinkB(mem->avail+Msize) = memory;
    LinkF(memory) = LinkB(memory) = mem->avail+Msize;    
    Kval(memory) = Msize;
    Tag(memory) = 1;
    Mom(memory) = memory;
  }
 

  
 
  void *l = LinkF(&mem->avail[j]);
  Kval(l) = k; 
  LinkF(&mem->avail[j]) = LinkF(l);
  LinkB(LinkF(l)) = &mem->avail[j];
  Tag(l) = 0;
  

  while( j > k ) {
    j--;
    p = (void *)( (char *)l + (1 << j));
    Tag(p) = 1;
    Mom(p) = Mom(l);    
    Kval(p) = j;
    LinkF(p) =  &mem->avail[j];
    LinkB(p) = &mem->avail[j];
    LinkF(&mem->avail[j]) = LinkB(&mem->avail[j]) = p;
  }


  return (char *)l + 8;
  
}
//globally overloaded delete operator
void operator delete( void *m) {
  int     k,j;
  void    *p,*l;
  bool    flag ;    
  
 
  l = (void *)((int)m - 8);
  k = Kval(l);
  j = k;
  flag = true;
  
  if( Kval(l) > Msize || Kval(l) <= 0 ) {
   
    return ;
  }
  
  do {

        
    // calculate the address of the buddy 
    if ( ((long)l - (long)Mom(l))%(1<<(k+1)) == 0 ) {
      p = (void *)((int)l + (1<<k));
    } else {
      p = (void *)((int)l - (1<<k));
    }
    
    if ( k == Msize )
      flag = false ;
    if ( Kval(p) != k) 
      flag = false ;
    if ( Tag(p) == 0 )
      flag = false ;     

    if ( flag != false){
      LinkF(LinkB(p)) = LinkF(p);
      LinkB(LinkF(p)) = LinkB(p);
      
      k++;     
      if ( p < l ) {
	l = p;
      } 
    }
    
  } while( flag );

  Tag(l) = 1;
  LinkF(l) = LinkF(&mem->avail[k]);
  LinkB(LinkF(&mem->avail[k])) = l;
  LinkB(l) = &mem->avail[k];
  LinkF(&mem->avail[k]) = l;
  Kval(l) = k;
         
}

#endif
#endif




