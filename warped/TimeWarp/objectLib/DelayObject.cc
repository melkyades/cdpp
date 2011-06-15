//-*-c++-*-
#ifndef DELAYOBJECT_CC
#define DELAYOBJECT_CC
// Copyright (c) 1994,1995 Ohio Board of Regents and the University of
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
//
// $Id: DelayObject.cc,v 1.1 1998/03/07 21:06:27 dmadhava Exp $
//
//---------------------------------------------------------------------------

// This is the code for a "delay" object in a queuing simulation.  It
// receives tokens, and distributes them to one output, with a delay from
// a chosen distribution.
 
#include "DelayObject.hh"
#include "DelayState.hh"

DelayObject::DelayObject(char *myName) {
  name = new char[strlen(myName)+1];
  strcpy(name,myName);
}

DelayObject::~DelayObject() {
  //cout << "--> Delay destroyed!" << endl;
}

void
DelayObject::initialize() {
  ((DelayState *) getCurrentState())->gen = new MLCG(id , (id + 1));
}

void 
DelayObject::executeProcess() {
  
  BasicEvent *recvEvent;
  
  recvEvent = getEvent();
  if ( recvEvent == NULL ) {
    cerr << name << " Scheduled for execution with no events "<< endl;
  }
  else {  
    // have to take care of how fork distributes the tokens here
    // include a field to specify how to distribute - to one or more
    // fork outputs....
    register double localDelay = 0 ;
    
    BasicEvent *newEvent = (BasicEvent *)new char[recvEvent->size];
    memcpy( newEvent, recvEvent, recvEvent->size);
    
    switch(delayDist){
    case UNIFORM :
      {
	Uniform uniform(first, second, ((DelayState *) state->current)->gen);
	localDelay = uniform();
	break;
      }
    case NORMAL :
      {
	Normal normal(first, second, ((DelayState *) state->current)->gen);
	localDelay = normal();
	break;
      }
    case BINOMIAL :
      {
	Binomial binomial((int)first, second, ((DelayState *) state->current)->gen);
	localDelay = binomial();
	break;
      }
    case POISSON :
      {
	Poisson poisson(first, ((DelayState *) state->current)->gen);
	localDelay = poisson();
	break;
      }
    case EXPONENTIAL :
      {
	NegativeExpntl expo(first, ((DelayState *) state->current)->gen);
	localDelay = expo();
	break;
      }
    case FIXED:
      localDelay = (int) first;
      break;
    default :
      cerr << "Invalid Delay type!!!!!!!!!!!!!!" << endl;
      localDelay = 0;
      break;
    }       

    newEvent->dest = dest;
    
    // we want the token to get there at the exact scheduled time 
    newEvent->recvTime = ((DelayState *) state->current)->oldDelay +
      (int) localDelay; 
    ((DelayState *) state->current)->oldDelay = newEvent->recvTime ;
    sendEvent(newEvent);
#ifdef DEBUG
    cout << "Message from " << newEvent->sender << " to " 
	 << newEvent->dest << endl ;
    cout << "Message id " << newEvent->eventId << ", sendtime : " 
	 << newEvent->sendTime
	 <<", recvtime : " << newEvent->recvTime << endl ;
#endif  
    
    
  }
}

void 
DelayObject::setDelayDistribution(distribution_t distribution, 
				  double firstParameter, 
				  double secondParameter) 
{
  delayDist = distribution;     // global variables
  first     = firstParameter;   // global variables
  second    = secondParameter;  // global variables
}

BasicState*
DelayObject::allocateState() {
  return (BasicState *) new DelayState();
}
  
#endif













