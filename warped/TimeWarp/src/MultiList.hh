//-*-c++-*-
#ifndef MULTILIST_HH
#define MULTILIST_HH
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
// $Id: MultiList.hh,v 1.5 1999/09/13 17:26:24 dmartin Exp $
//
//
//---------------------------------------------------------------------------


#include "SortedListOfEvents.hh"

extern "C" {
  void exit(int);  
}

template < class Element > 
class MultiList : public SortedListOfEvents < Element > {

#ifdef OLD_STYLE_FRIENDS
  friend ostream& operator<< (ostream&, const MultiList& );
#else
  friend ostream& operator<< <> (ostream&, const MultiList& );
#endif

public:  
  MultiList();
  ~MultiList();


  Element* seekId(int, listMode_t, int);
  void insert(Element*, int);
  Element* removeFind(int);
  Element* removeCurrentObj();
  Element* find(Element*, findMode_t mode = EQUAL, int id = 0);
  Element* findNext(int);
  Element* getObj(int);
  Element* setCurrentObjToFindObj(int);
  void setCurrentToCurrentObj(int i){currentPos = currentObj[i];}
  bool testCurrentObj(int);
  Element* peekPrevObj();
  VTime findPrevTime(int i);
  Element* findFirstAtTime(Element* current);
  void resetMiniHead(int, VTime&);
  void initialize(int);
  void printSmallQ(ostream& = cout,int = 0);
  void printCurrentObj();
  void printlVTArray();
  VTime *lVTArray;
  Element* getHead(int);
  
protected:
  int numObjects;
  int *listSize;
  Element** headObj;
  Element** tailObj;
  Element** findObj;
  Element** insertObj;
  Element** currentObj;
  bool *idleObjArray;
  Element* remove(Element*, int =-1);

private:  
  Element* findBackwards (Element*, Element*, findMode_t, int);

};
#include "MultiList.cc"
#endif








