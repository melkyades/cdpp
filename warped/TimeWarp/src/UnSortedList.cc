#ifndef UNSORTEDLIST_CC
#define UNSORTEDLIST_CC

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
// $Id: UnSortedList.cc,v 1.1 1998/03/07 21:11:03 dmadhava Exp $
//
// -----------------------------------------------------------------------

template < class Element > 
Container<type>* 
UnSortedList<Element>::search(const type* n) 
{
  Container<type>* p;
  
  for (p = head; p != NULL; p = p->next) {
    if (p->object == n) {
      break;
    };
  };
  return p;
};

template < class Element > 
void 
UnSortedList<Element>::append(type* n)
{
  Container<type>* p;
  
  p = new Container<type>(n);
  if (tail == NULL) {
    head = p;
    tail = p;
  } 
  else {
    tail->next = p;
    p->prev = tail;
    tail = p;
  };
  size = size + 1;
};

template < class Element > 
void 
UnSortedList<Element>::prepend(type* n)
{
  Container<type>* p;
  
  p = new Container<type>(n);
  if (head == NULL) {
    head = p;
    tail = p;
  } 
  else {
    head->prev = p;
    p->next = head;
    head = p;
  };
  size = size + 1;
};

template < class Element > 
void 
UnSortedList<Element>::insert_after(type* current, type* new_object)
{
  Container<type>* p;
  Container<type>* q;
  
  p = search(current);
  if (p == NULL) {
    cerr << "Attempt to insert after a non-existing list object." << endl;
  } 
  else {
    q = new Container<type>(new_object);
    
    if (p->next != NULL) {
      q->next = p->next;
      p->next->prev = q;
    };
    
    q->prev = p;
    p->next = q;
  };
  size = size + 1;
};

template < class Element > 
bool 
UnSortedList<Element>::remove(const type* n)
{
  Container<type>* p;
  Container<type>* previous;
  Container<type>* next;
  
  p = search(n);
  if (p == NULL) {
    //cerr << "Attempt to delete non-existing list object." << endl;
    return false;
  } 
  else {
    previous = p->prev;
    next = p->next;
    if (previous == NULL) {
      head = next;
    } 
    else {
      previous->next = next;
    };
    if (next == NULL) {
      tail = previous;
    } 
    else {
      next->prev = previous;
    };
    delete p;
    return true;
  };	  
  size = size - 1;
};

template < class Element > 
type* 
UnSortedList<Element>::successor(const type* n)
{
  Container<type>* p;
  
  p = search(n);
    if (p == NULL) {
      cerr << "Attempt to find successor of non-existing list object." << endl;
      return NULL;
    } 
    else {
      if (p->next==NULL) {
	return NULL;
      } 
      else {
	return p->next->object;
      };
    };
};

template < class Element > 
type* 
UnSortedList<Element>::predecessor(const type* n)
{
  Container<type>* p;
  
  p = search(n);
  if (p == NULL) {
    cerr << "Attempt to find predecessor of non-existing list object." 
	 << endl;
    return NULL;
  } 
  else {
    if (p->prev==NULL) {
      return NULL;
    } 
    else {
      return p->prev->object;
    };
  };
};



template < class Element >   
type* 
UnSortedList<Element>::first() {
  if (head == NULL) {
    return NULL;
  } 
  else {
    return head->object;
  };
};

template < class Element > 
type* 
UnSortedList<Element>::last() {
  if (tail == NULL) {
    return NULL;
  } 
  else {
    return tail->object;
  };
};


#endif
