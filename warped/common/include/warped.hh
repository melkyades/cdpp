#ifndef WARPED_HH
#define WARPED_HH

#define VERSION 1.02
#define PATCH_NUMBER 0

// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.
//
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
// $Id: warped.hh,v 1.4 1999/10/29 23:21:13 ramanan Exp $
//
//---------------------------------------------------------------------------

#include <iostream>
#include <strstream>
#include <string.h>
#include <values.h>
#include <ctype.h>
#include <cstdlib>

// #if !defined (__GNUC_MINOR__) || __GNUC_MINOR__ < 6
// typedef int bool;
// #define false 0
// #define true 1
// #endif

#ifndef NULL
#define NULL 0
#endif

#ifdef __GNUC_MINOR__
typedef long long SequenceCounter;
#else
typedef long int SequenceCounter;
#endif

const char CHECKPOINT_DELIMITER = '_';

#include "DefaultVTime.hh"

template <class type>
inline const type&
MIN_FUNC(const type& x, const type& y) {
  if (x < y) { return x; }
  return y;
}

enum SIGN       {POSITIVE, NEGATIVE};
enum listMode_t {START, CURRENT, END};
enum findMode_t {LESS, LESSEQUAL, EQUAL, GREATEREQUAL, GREATER};


inline std::ostream&
operator<<(std::ostream& os, const SIGN s) {
  switch (s) {
  case POSITIVE:
    os << '+';
    break;
  case NEGATIVE:
    os << '-';
    break;
  default:
    os << "(invalid SIGN value)" << std::endl;
    break;
  }
  return os;
}
#endif







