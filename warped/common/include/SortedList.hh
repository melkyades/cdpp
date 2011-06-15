//-*-c++-*-
#ifndef SORTED_LIST_HH
#define SORTED_LIST_HH
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
// $Id: SortedList.hh,v 1.4 1999/10/28 04:34:06 mal Exp $
//
//---------------------------------------------------------------------------

#include "Container.hh"
#include <fstream.h>
#include "warped.hh"
// This class implements a doubly link list, which follows the
// specification outlined in the warped kernel documentation

template < class Element > class SortedList {

  //#ifdef OLD_STYLE_FRIENDS
  //friend ostream& operator<< (ostream&, const SortedList<Element>& );
  //#else
  //friend ostream& operator<< <class Element> (ostream&, const SortedList<Element>& );
  //#endif

public:
  SortedList();
  SortedList( int (*)(const Element*, const Element*) );  // construct with compare func
  virtual ~SortedList();

  ofstream* lpFile;
  
  virtual void setFile(ofstream *outfile);

  void setFunc( int (*)(const Element*, const Element*) );
  int size() { return listsize;};
  Element* front();
  Element* back();
  Element* seek (int, listMode_t);
  virtual void insert(Element*);
  Element* get() const;
  Element* removeFind();
  Element* removeCurrent();
  Element* find(Element*, findMode_t mode = EQUAL);
  Element* findNext();
  void print(ostream& = cout) const;
  
  void saveCurrent() { tmpPos = currentPos; };
  void restoreCurrent() { currentPos = tmpPos; };

  Element* setCurrentToInsert();
  Element* setCurrentToFind();
  Element* peekPrev() const;
  Element* peekNext() const;

  Container< Element >* getCurrent() { return currentPos;};
  Container< Element >* getInsert() { return insertPos;};

  void setCurrent(Container< Element >* ptr) {
    currentPos = ptr;
  };

  Container< Element >* getHead() { return head; };
  Container< Element >* getTail(){ return tail;};

  virtual Element* remove(Container<Element> *);  

  void reset() {
    head = NULL;
    tail = NULL;
    findPos = NULL;
    insertPos = NULL;
    currentPos = NULL;
    tmpPos = NULL;
    listsize = 0;
  };

  int memoryUseage;

protected:
  Container< Element > *head;
  Container< Element > *tail;
  Container< Element > *findPos;
  Container< Element > *insertPos;
  Container< Element > *currentPos;
  Container< Element > *tmpPos;
  int (*compare)(const Element*, const Element*);
  int listsize;

private:

  Element* findBackwards(Element*, Container<Element>*, findMode_t);
};

#include "SortedList.cc"
#endif
