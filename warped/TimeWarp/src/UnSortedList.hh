#ifndef UNSORTEDLIST_HH
#define UNSORTEDLIST_HH

// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
// Cincinnati.  All rights reserved.
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
// $Id: 
//
// -----------------------------------------------------------------------

#ifndef NULL
#define NULL 0
#endif

#include <iostream.h>
#include "warped.hh"
#include "config.hh"

template <class Element>
class Container;

// template for constructing doubly linked lists

template <class Element>
class UnSortedList {

private:

  Container<Element>* head;
  Container<Element>* tail;
  int size;
  Container<Element>* search(const Element* n);

public:

  UnSortedList() {head = NULL; tail=NULL; size=0;};
  ~UnSortedList() {};
    
  Container<Element>* getHead() { return head; };
  Container<Element>* getTail() { return tail; };
  int getSize() { return size; }
  void append(Element* n);
  void prepend(Element* n);
  void insert_after(Element* current, Element* new_object);
  bool remove(const Element* n);
  Element* successor(const Element* n);
  Element* predecessor(const Element* n);
  Element* first();
  Element* last();
};  

#include "UnSortedList.cc"
#endif




