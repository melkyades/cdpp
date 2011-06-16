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
#include <fstream>
#include "BasicEvent.hh"
#include "InputQ.hh"

// This class implements a doubly link std::list, which follows the
// specification outlined in the warped kernel documentation

#define Element BasicEvent

extern "C" {
  void exit(int);  
}

class SequentialEventQueue : public InputQ {
  friend std::ostream& operator<< (std::ostream&, const SequentialEventQueue& );
  
public:
  SequentialEventQueue();
  ~SequentialEventQueue();
  
  std::ofstream* lpFile;
  void setFile(std::ofstream *outfile) {
    lpFile = outfile;
  }
  
  inline Element* removeAlreadyProcessedEvents() {
    for(register BasicEvent* deleteEvent = head; ((deleteEvent != NULL) && (deleteEvent->alreadyProcessed == true)); ((head = head->next), (delete [] (char *) deleteEvent), (deleteEvent = head)));

    insertPos = tail;
    
    return ((head != NULL) ? (head->prev = NULL, currentPos = head) : (insertPos = currentPos = head));
  }

  inline void garbageCollect(VTime* gtime) {
    for(register BasicEvent* deleteEvent = head; ((deleteEvent != NULL) && (deleteEvent->alreadyProcessed == true)); ((head = head->next), (delete [] (char *) deleteEvent), (deleteEvent = head)));

    insertPos = tail;
    
    ((head != NULL) ? (head->prev = NULL, currentPos = head) : (insertPos = currentPos = head));
  }
  
  inline Element* gotoHead() {
    return (currentPos = head);
  }
  
  inline Element* gotoTail() {
    return (currentPos = tail);
  }
  
  inline Element* gotoNext() {
    return ((currentPos != NULL) ? (currentPos = currentPos->next) :
	    currentPos);
  }
  
  int size() { return listsize;};
  Element* seek (int, listMode_t);
  void insert(Element*);
  
  inline Element* get() const  {
    return currentPos;
  }
  
  inline Element* peekEvent() {
    return currentPos;
  }

  Element* removeFind();
  Element* removeCurrent();
  Element* find(Element*, findMode_t mode = EQUAL);
  Element* findNext();
  void print(std::ostream & = std::cout) const;
  
  Element* setCurrentToInsert();
  Element* setCurrentToFind();
  Element* peekPrev() const;
  Element* peekNext() const;
  Element* getCurrent() { return currentPos;}
  
  void setCurrent(Element* ptr) {
    currentPos = ptr;
  }
  
  Element* getHead() { return head; };
  Element* getTail() { return tail; };
  Element* remove(Element *);  
  
  Element* head;
  Element* tail;
  Element* insertPos;
  Element* currentPos;
  Element* findPos;
  int listsize;
  
private:
  
  Element* findBackwards (Element*, Element*, findMode_t);
};

#endif

