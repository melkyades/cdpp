//-*-c++-*-
#ifndef INPUTQUEUE_HH
#define INPUTQUEUE_HH
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
// $Id: SequentialInputQueue.hh,v 1.4 1999/10/28 04:55:45 mal Exp $
//
//
//---------------------------------------------------------------------------
#include <fstream>
#include <cstdlib>
#include "BasicEvent.hh"

#include "SequentialEventQueue.hh"


class SequentialInputQueue : public SequentialEventQueue {
public:
  SequentialInputQueue();
  ~SequentialInputQueue();

  inline BasicEvent* getAndSeek() {
    register BasicEvent* retVal;
    return (retVal = get(), gotoNext(), retVal);
  }

  inline BasicEvent* getEvent() {
    return getAndSeek();
  }

  BasicEvent* find(VTime, findMode_t = EQUAL);
  
  inline BasicEvent* find(BasicEvent *event, findMode_t mode = EQUAL) {
    return SequentialEventQueue::find(event, mode);
  }
  
  BasicEvent* getInsert();

  VTime lastExecuted;
};

#endif





