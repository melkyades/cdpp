//-*-c++-*-
#ifndef FORKOBJECT_CC
#define FORKOBJECT_CC
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
// $Id: ForkObject.cc,v 1.1 1998/03/07 21:06:31 dmadhava Exp $
//
//---------------------------------------------------------------------------

// This is the code for a "fork" object in a queuing simulation.  It
// receives tokens, and distributes them to one or more outputs, with
// a chosen distribution.
 
#include "ForkObject.hh"
#include "ForkState.hh"
#include <values.h>

#ifdef VERIFY
#include "FileQueue.hh"
#include "FileData.hh"
#endif

ForkObject::ForkObject(char *myName) {
  name = new char[strlen(myName)+1];
  strcpy(name,myName);
  
#ifdef VERIFY
  outFileQ = new FileQueue[1](myName);
  numOutFiles = 1;
#endif
}

ForkObject::~ForkObject(){
  //cout << "--> Fork destroyed!" << endl;
  delete [] fanOutIds;
}

void
ForkObject::initialize() {
  delete ((ForkState *) state->current)->gen;
  ((ForkState *) state->current)->gen = new MLCG(id, (id + 1));
}

void
ForkObject::executeProcess() {
  BasicEvent *recvEvent;

  int i ;
  recvEvent = getEvent();
  if ( recvEvent == NULL ) {
    cerr << name << " Scheduled for execution with no events " << endl;
  }
  else {  
 
    // have to take care of how fork distributes the tokens here
    // include a field to specify how to distribute - to one or more
    // fork outputs....

    BasicEvent *newEvent = (BasicEvent *)new char[recvEvent->size];
    memcpy( newEvent, recvEvent, recvEvent->size);
    
    double randNum;
    
    switch(forkType){
    case ALL : // fork sends BasicEvent to all its outputs
      
      for(i=0; i< numFanOut ; i++)
	{
	  BasicEvent *newEvent = (BasicEvent *)new char[recvEvent->size];
	  memcpy( newEvent, recvEvent, recvEvent->size);
	  
	  // assignment of different destinations fanoutIds[] contains the
	  // multiple dests. of the fork object.
	  newEvent->dest = fanOutIds[i] ;

	  // we want the token to get there at the exact scheduled time 
	  newEvent->recvTime += ((ForkState *) state->current)->localDelay; 

	  ((ForkState *) state->current)->lastOutput = newEvent->dest;
	}
      break ;

    case ALTERNATE : // Alternate between the first two choices

      if (((ForkState *) state->current)->lastOutput != fanOutIds[0]) {
	newEvent->dest = fanOutIds[0];  // assume the simulation object id's
				// match the forks outputs
      } else {
	newEvent->dest = fanOutIds[1];  // assume the simulation object id's
				// match the forks outputs
      }


      newEvent->recvTime = recvEvent->recvTime + ((ForkState *) state->current)->localDelay;
				// we want the 
				//token to get there at
				// exact scheduled time 
      ((ForkState *) state->current)->lastOutput = newEvent->dest;

#ifdef DEBUG
      cout << "Message from " << newEvent->sender << " to " 
	   << newEvent->dest << endl ;
      cout << "Message id " << newEvent->eventId << ", sendtime : " 
	   << newEvent->sendTime
	   <<", recvtime : " << newEvent->recvTime << endl ;
#endif  

      break ;

    case ROUNDROBIN : // RoundRobin scheme "What else?"

      if (((ForkState *) state->current)->lastOutput == -1){
	newEvent->dest = fanOutIds[0];
      } else {
	((ForkState *) state->current)->lastOutput++ ; // increment to maintain RR scheme
	((ForkState *) state->current)->lastOutput=((ForkState *) state->current)->lastOutput % numFanOut ;
	newEvent->dest = fanOutIds[((ForkState *) state->current)->lastOutput];
      }

     newEvent->recvTime = recvEvent->recvTime+((ForkState *) state->current)->localDelay; 
				// we want the 
				//token to get there at
				// exact scheduled time 
      break ;

    case UNIFORM : // Uniform distribution
      {
	Uniform uniform(first, second, ((ForkState *) state->current)->gen);
	newEvent->dest = fanOutIds[(int) uniform()];
	((ForkState *) state->current)->lastOutput = newEvent->dest;
	break;
      }
    case NORMAL : // Normal distribution
      {
	Normal normal(first, second, ((ForkState *) state->current)->gen);
	newEvent->dest = fanOutIds[(int) normal()];
	((ForkState *) state->current)->lastOutput = newEvent->dest;
	break;
      }
    case BINOMIAL : // Binomial distribution
      {
	Binomial binomial((int)first,second, ((ForkState *) state->current)->gen);
	newEvent->dest = fanOutIds[(int) binomial()];
	((ForkState *) state->current)->lastOutput = newEvent->dest;
	break;
      }
    case POISSON : // Poisson distribution
      {
	Poisson poisson(first, ((ForkState *) state->current)->gen);
	newEvent->dest = fanOutIds[(int) poisson()];
	((ForkState *) state->current)->lastOutput = newEvent->dest;
	break;
      }
    case EXPONENTIAL : // Exponential distribution
      {
	NegativeExpntl expo(first, ((ForkState *) state->current)->gen);
	newEvent->dest = fanOutIds[(int) expo()];
	((ForkState *) state->current)->lastOutput = newEvent->dest;
	break;
      }
    case FIXED: // Fixed output
      newEvent->dest = dest;
      ((ForkState *) state->current)->lastOutput = newEvent->dest;
      break;

    case CONDITIONAL : // Fill in the blank
      cerr << "ERROR: Conditional Distribution unimplemented!!" << endl;
      break ;
    
    case SPECIAL:
      {
	Uniform uniform(first, second, ((ForkState *) state->current)->gen);
	randNum = uniform();
	if( first < 2 * randNum){
	  newEvent->dest = fanOutIds[0];
	}
	else{
	  newEvent->dest = fanOutIds[1];
	}
	break;
      }
    default:
      cerr << "ARRGH! error in " << name << endl;
    }
    // cout << "fork sending " << *newEvent << endl;
#ifdef VERIFY
    strstream str;
    str << "Sending to: " << newEvent->dest 
	<< " at " << ((ForkState *) state->current)->lVT << " " << randNum 
	<< '\n' << '\0'<< ends;
    FileData *outputLine = new FileData;
    outputLine->line = str.str();
    outputLine->length = strlen(outputLine->line) +1;
    outputLine->time = ((ForkState *) state->current)->lVT;
    outFileQ[0].insert(outputLine);
#endif
    sendEvent(newEvent);
  }
}

void
ForkObject::setForkDistribution(distribution_t choice, int numberOut, 
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
    fanOutIds[i] = outputIds[i] ;
  }
 
}

BasicState*
ForkObject::allocateState() {
  return (BasicState *) new ForkState();
}

#endif
