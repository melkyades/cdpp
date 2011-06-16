//-*-c++-*-
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
// $Id: Container.hh,v 1.2 1999/10/28 04:34:06 mal Exp $
//
//---------------------------------------------------------------------------
#ifndef CONTAINER_HH
#define CONTAINER_HH

// container template class for misc use in std::lists, queues, and such.
template <class type>
class Container {
public:

  type* object;
  Container<type>* prev;
  Container<type>* next;

// Removed initializations here and used it only whenever necessary to reduce
// unnecessary initialization.
  Container() { 
    object = NULL;
    next = NULL; 
    prev = NULL;
  };

  Container(type* s) : object(s) {
    next = NULL; 
    prev = NULL;
  };

  ~Container() {
  };


#ifdef REUSE
  // memory reallocation functions/data
public:
  void* operator new(size_t);
  void operator delete(void*);
  void cleanup();

  static Container<type>* freepool;
  static unsigned freesize; 

#ifdef REUSESTATS
  void printReuseStats(std::ostream&);
#endif
#endif
};
#endif
