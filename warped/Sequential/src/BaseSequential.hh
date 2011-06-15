//-*-c++-*-
#ifndef BASESEQUENTIAL_HH
#define BASESEQUENTIAL_HH
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
// $Id: BaseSequential.hh,v 1.2 1999/10/28 04:55:41 mal Exp $
//
//
//---------------------------------------------------------------------------
#include "warped.hh"
#include <fstream.h>
#include "SequentialObjectRecord.hh"
#include "InputQInterface.hh"

class SequentialObjectRecord;
class BasicState;
class BasicEvent;

class BaseSequential {
  friend class SequentialLP;
public:
  char *name;
  int    id;       // A unique number for each process
  
#ifdef OBJECTDEBUG
  ofstream outFile;
#endif
  virtual ~BaseSequential(){};
  
  virtual void recvEvent( BasicEvent * )=0;
  virtual void sendNegEvent(BasicEvent *) {};
  virtual void simulate(){};
  virtual void initialize(){};
  virtual void finalize(){};
  
  SequentialObjectRecord *commHandle;
  
  static InputQInterface inputQ;
  
#ifdef LPDEBUG
  virtual void setFile(ofstream *) {
    cerr << name << ": Error! BaseSequential::setFile called!" << endl;
    abort();
  };
#endif
  
  virtual BasicState* allocateState() = 0;
  virtual void deAllocateState(BasicState *);
  virtual BasicState* getCurrentState() const;
  
protected:
  virtual void timeWarpInit();
};

#endif


