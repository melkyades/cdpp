//-*-c++-*-
#ifndef BASESEQUENTIAL_CC
#define BASESEQUENTIAL_CC
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
// $Id: BaseSequential.cc,v 1.3 1999/10/28 04:55:40 mal Exp $
//
//
//---------------------------------------------------------------------------

#include "BaseSequential.hh"


InputQInterface BaseSequential::inputQ ;

void
BaseSequential::deAllocateState(BasicState *) {
  cerr << name
       << ": Error! BasicTimeWarp::allocateState() called\n";
  cerr << "Maybe new style of state saving is not fully implemented yet\n";
  abort();
}

BasicState*
BaseSequential::getCurrentState() const {
  cerr << name
       << ": Error! BaseSequential::getCurrentState() called\n";
  cerr << "Maybe new style of state saving is not fully implemented yet\n";
  abort();
  return NULL;
}

void
BaseSequential::timeWarpInit() {
  cerr << name
       << ": Error! BasicTimeWarp::timeWarpInit() called\n";
  cerr << "Maybe new style of state saving is not fully implemented yet\n";
  abort();
}

#endif
