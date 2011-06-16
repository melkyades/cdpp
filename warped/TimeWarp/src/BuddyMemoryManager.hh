#ifndef BUDDYMEMORYMANAGER_HH
#define BUDDYMEMORYMANAGER_HH
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
//---------------------------------------------------------------------------


#include <iostream.h>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <new.h>

#include "warped.hh"
#include "config.hh"

#include "warpedNewHandler.hh"
#include "bool.hh"


extern "C" void* malloc(size_t);
// algo follows that given in knuth vol1 


#define Msize     15 // size of the memory block is 2^Msize
#define ksize      5 // not used right now



// global new and delete 
void * operator new( size_t );
void operator delete( void* );


#define LinkF(m) ((Head *)m)->linkf // forward link of doubly linked list
#define LinkB(m) ((Head *)m)->linkb // backward link of doubly linked list
#define Kval(m)  ((Head *)m)->kval // size of the memory block is 2 power kval
#define Tag(m)   ((Head *)m)->tag // 0 reserved 1 available
#define Mom(x)   ((Head *)x)->mom

struct Head {
  int   tag:1;
  int   kval:31;
  int  *mom;
  void *linkf;
  void *linkb;
};


class BuddyMemoryManager {
 
public:

  BuddyMemoryManager();
  ~BuddyMemoryManager();

  Head avail[Msize+1]; 
  unsigned int *memory;
};


#endif





