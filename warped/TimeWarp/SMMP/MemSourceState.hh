//-*-c++-*-
#ifndef MEMSOURCESTATE_HH
#define MEMSOURCESTATE_HH
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
// $Id: MemSourceState.hh,v 1.1 1998/03/07 21:05:01 dmadhava Exp $
//
//
//---------------------------------------------------------------------------

#ifdef __GNUG__
#include <MLCG.h>
#else
#include "../rnd/MLCG.h"
#endif

#include "QueuingTypes.hh"
#include "BasicState.hh"
#include "IIR.cc"

class MemSourceState : public BasicState {
public:
  MemSourceState();
  ~MemSourceState();
  
  int maxMemRequests;   // The number of tokens that the object
                        // will produce
  int numMemRequests;   // The number of tokens produced so far
                        // producing tokens
  VTime oldDelay;     
  MLCG  *gen;  
  VTime localdelay;
  IIR <double> filter;
  int numTokens;
  int maxTokens;
 
  void serialize(std::ofstream* ckFile, int mysize);
  void deserialize(std::ifstream* inFile);
  MemSourceState& operator=(MemSourceState& thisState);
  void copyState(BasicState *);
  int  getSize() const;
};

#endif



