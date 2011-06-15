//-*-c++-*-
#ifndef ONE_SHOT_HH
#define ONE_SHOT_HH
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
// $Id: OneShot.hh,v 1.1 1998/03/07 21:10:20 dmadhava Exp $
//
//
//---------------------------------------------------------------------------

#include <iostream.h>
#include "bool.hh"

template< class Type >
class OneShot {
public:
  OneShot<Type>& operator=( const Type & ); 
  int operator==(  const Type & ); 
//  friend int operator==( const Type &, const OneShot<Type> & ); 
  int operator!=(  const Type & ); 
//  friend int operator!=( const Type &, const OneShot<Type> & ); 

  operator Type(){ return value; }

  void reset();
  bool didItChange();

  OneShot(){
    changed = false;
    value = INT_TO_BOOL(0);
  };

  OneShot(Type initialVal){
    value = initialVal;
    changed = false;
  };

  ~OneShot(){};

  Type value; // this needs to be private!
private:

  bool changed;
};

#include "OneShot.cc"

#endif
