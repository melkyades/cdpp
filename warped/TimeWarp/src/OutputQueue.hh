//-*-c++-*-
#ifndef OUTPUT_QUEUE_HH
#define OUTPUT_QUEUE_HH
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
// $Id: OutputQueue.hh,v 1.8 1999/10/24 23:26:15 ramanan Exp $
//
//
//---------------------------------------------------------------------------
#include <fstream.h>
#include "BasicEvent.hh"
#include "SortedList.hh"
#include <iostream.h>
#include "ObjectRecord.hh"
#include "StateManager.hh"
#include "BasicState.hh"

extern int BasicEventCompareSend( const BasicEvent*, const BasicEvent*);

class OutputQueue:public SortedList< BasicEvent > {

public:

  OutputQueue() { 
    setFunc( BasicEventCompareSend ); 
  };

  ~OutputQueue();
  
  void reset();

  BasicEvent*  find(VTime, findMode_t = EQUAL );  // nicer interface to find.

  void setFile(ofstream *outfile) {
    lpFile = outfile;
  };

#ifdef STATEDEBUG

  void gcollect(Container<BasicEvent>*&, ObjectRecord*, bool, 
		StateManager* );
#else

  void gcollect(VTime, Container<BasicEvent>*&, ObjectRecord*, bool);

#endif

  void gcollect(VTime gtime, ObjectRecord* simArray);

  // Member Data
  ObjectRecord* simArray;
};

#endif
