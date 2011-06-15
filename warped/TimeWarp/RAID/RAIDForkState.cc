//-*-c++-*-
#ifndef RAIDFORKSTATE_CC
#define RAIDFORKSTATE_CC
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
// $Id: RAIDForkState.cc
//
//---------------------------------------------------------------------------

#include "RAIDForkState.hh"

RAIDForkState::RAIDForkState()  {
  lastOutput = 0;
  localdelay = 0;
  myTime     = 0;
} 

RAIDForkState&
RAIDForkState::operator=(RAIDForkState& thisState){
  (BasicState &)*this = (BasicState &)thisState;
  this->lastOutput    = thisState.lastOutput;
  this->localdelay    = thisState.localdelay;
  this->myTime        = thisState.myTime;
  
  return *this;
}

void
RAIDForkState::copyState(BasicState* rhs) {
  *this = *((RAIDForkState *) rhs);
}

int
RAIDForkState::getSize() const {
  return sizeof(RAIDForkState);
}

ostream&
operator<< (ostream& os, const RAIDForkState& rfs) {
  
  os << "lastOutput: " << rfs.lastOutput 
     << " localdelay: " << rfs.localdelay << " myTime: " 
     << rfs.myTime << " " << (BasicState& ) rfs;
  
  return( os );
}

void
RAIDForkState::print(ostream& os) {
  os << *this; 
}

#endif
