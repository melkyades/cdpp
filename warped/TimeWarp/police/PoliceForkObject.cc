//-*-c++-*-
#ifndef POLICEFORKOBJECT_CC
#define POLICEFORKOBJECT_CC
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
// $Id: PoliceForkObject.cc,v 1.2 1998/05/07 22:43:35 gsharma Exp $
//
//---------------------------------------------------------------------------

// This is the code for a "fork" object in a queuing simulation.  It
// receives tokens, and distributes them to one or more outputs, with
// a chosen distribution.
 
#include "PoliceForkObject.hh"

PoliceForkObject::PoliceForkObject(char *myname){
  name = new char[strlen(myname)+1];
  strcpy(name,myname);
#ifdef DEBUG
    cout << "--> PoliceFork created!" << endl;
#endif
  };


PoliceForkObject::~PoliceForkObject(){
    delete [] fanOutIds ;
#ifdef DEBUG
    cout << "--> PoliceFork destroyed!" << endl;
#endif
  };

inline 
void PoliceForkObject::executeProcess() {

  PoliceToken *recvEvent;

  recvEvent = (PoliceToken *) getEvent();
  if ( recvEvent != NULL ) {
    
    // have to take care of how fork distributes the tokens here
    // include a field to specify how to distribute - to one or more
    // fork outputs....

    PoliceToken *newEvent = (PoliceToken *)new char[recvEvent->size];
    memcpy( newEvent, recvEvent, recvEvent->size);
        
    // select dest depending on priority

//  if (newEvent->priority == 1){
#ifdef DEBUG
  cout << "Priority 1 message received" << endl ;
#endif
    newEvent->dest = fanOutIds[((PoliceForkState *)state->current)->lastOutput];
    ((PoliceForkState *)state->current)->lastOutput++ ; // increment to maintain RR scheme
    ((PoliceForkState *)state->current)->lastOutput = ((PoliceForkState *)state->current)->lastOutput % numFanOut ;
    
    newEvent->recvTime = recvEvent->recvTime + ((PoliceForkState *)state->current)->localDelay; 
				// we want the 
				//token to get there at
				// exact scheduled time 
    
    sendEvent(newEvent); 
      
  }
  
};

void 
PoliceForkObject::setForkDistribution(distribution_t choice, int numberOut, 
				int *outputIds, double firstParameter,
				double secondParameter)
{
  int i ;
 
  forkType   = choice ; // global variables
  first      = firstParameter ; // global variables
  second     = secondParameter ;  // global variables
  numFanOut  = numberOut ;
  fanOutIds = new int[numberOut];
  for(i=0 ; i < numberOut; i++) {
    fanOutIds[i] = outputIds[i] ;}
 
}
#endif













