//-*-c++-*-
#ifndef RAIDDISKSTATE_CC
#define RAIDDISKSTATE_CC
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
// $Id: RAIDDiskState.cc
//
//
//---------------------------------------------------------------------------

#include "RAIDDiskState.hh"

RAIDDiskState::RAIDDiskState() {
  currentCylinder = 0;
  myTime          = 0;
}

RAIDDiskState::RAIDDiskState( const RAIDDiskState& ) { 
  cerr << "RAIDServer copy constructor has not been implemented" << endl;
}

RAIDDiskState&
RAIDDiskState::operator=(RAIDDiskState& thisState){
  (BasicState &)*this = (BasicState &)thisState;
  this->currentCylinder = thisState.currentCylinder;
  this->myTime = thisState.myTime;
  return *this;
}

ostream&
operator<< (ostream& os, const RAIDDiskState& rds) {
  os << "currentCylinder: " << rds.currentCylinder << " myTime: " 
     << rds.myTime << " " << (BasicState& ) rds;
  
  return( os );
}

void 
RAIDDiskState::print(ostream& os) {
  os << *this;
}

void
RAIDDiskState::copyState(BasicState *rhs) {
  *this = *((RAIDDiskState *) rhs);
}

int
RAIDDiskState::getSize() const {
  return sizeof(RAIDDiskState);
}

#endif
