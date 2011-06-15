//-*-c++-*-
#ifndef SORTED_LIST_OF_EVENTS_HH
#define SORTED_LIST_OF_EVENTS_HH
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
// $Id:
//
//---------------------------------------------------------------------------

#include "warped.hh"
#include "config.hh"

#include <fstream.h>

// This class implements a doubly link list, which follows the
// specification outlined in the warped kernel documentation

extern "C" {
  void exit(int);  
}

template < class Element > class SortedListOfEvents {

#ifdef OLD_STYLE_FRIENDS
  friend ostream& operator << (ostream&, const SortedListOfEvents& );
#else
  friend ostream& operator << <> (ostream&, const SortedListOfEvents& );
#endif

public:
  SortedListOfEvents();
  // construct with compare func
  SortedListOfEvents( int (*)(const Element*, const Element*) );
  ~SortedListOfEvents();

  ofstream* lpFile;
  void setFile(ofstream *outfile) {
    lpFile = outfile;
  };

  void setFunc( int (*)(const Element*, const Element*) );
  int size() { return listsize;};
  Element* front() { return head; };
  Element* back()  { return tail; };
  Element* seek (int, listMode_t);
  void insert(Element*);
  Element* get() const;

  Element* peekEvent() const {
    return get();
  };

  Element* removeFind();
  Element* removeCurrent();
  Element* find(Element*, findMode_t mode = EQUAL);
  Element* findNext();
  void print(ostream & = cout) const;
  
  Element* setCurrentToInsert();
  Element* setCurrentToFind();
  Element* peekPrev() const;
  Element* peekNext() const;

  void setCurrent(Element* ptr) { currentPos = ptr; }
  Element* getCurrent() { return currentPos;}
  void saveCurrent(){ savedCurrentPos = currentPos; }
  void restoreCurrent() { currentPos = savedCurrentPos; }

  Element* getHead() { return head; };
  Element* getTail() { return tail; };
  Element* remove(Element *);  

protected:
  int (*compare)(const Element*, const Element*);

  Element* head;
  Element* tail;
  Element* insertPos;
  Element* currentPos;
  Element* findPos;
  Element* savedCurrentPos;
  int listsize;

private:

  Element* findBackwards (Element*, Element*, findMode_t);
};

#include "SortedListOfEvents.cc"
#endif

