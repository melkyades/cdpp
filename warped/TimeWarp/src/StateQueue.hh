//-*-c++-*-
#ifndef STATE_QUEUE_HH
#define STATE_QUEUE_HH
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
//
// $Id: StateQueue.hh,v 1.6 1999/10/24 23:26:21 ramanan Exp $
//
//
//---------------------------------------------------------------------------
#include "warped.hh"
#include "config.hh"

#include "BasicState.hh"
#include "KernelMsgs.hh"


#include "SortedListOfEvents.hh"


extern "C" {
  void exit(int);  
}

extern int BasicStateCompare(const BasicState*, const BasicState*);
class StateManager;


class StateQueue : public SortedListOfEvents< BasicState> {
#ifdef CHECKASSERTIONS
  friend StateManager;
#endif

public:
    
    // Documention for the following function in StateQueue.cc file.
    void insert(BasicState*) ;

  int id;    

  StateQueue(){
    setFunc( BasicStateCompare );
  }
  ~StateQueue(){};

  void reset() {
    listsize = 0;
    // initialize all listvalues
    head = NULL;
    tail = NULL;
    currentPos = NULL;
    insertPos = NULL;
    findPos = NULL;
  };

  BasicState* find(VTime, findMode_t );
  
  // CHANGE : needs to be checked - is it used at all ?
  VTime gcollect(VTime);  
  void clear(); // do not remove - vital error check function
  VTime gcollect(VTime, BasicEvent*&, Container<BasicEvent>*&);
  VTime timeOfStateFromCurrent(int );
  VTime timeDiff();

  ostream* stateFile;
  void setStateFile( ostream* outfile){
    stateFile = outfile;
  }
  
};

#endif



