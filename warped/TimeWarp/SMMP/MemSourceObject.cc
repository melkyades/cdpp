//-*-c++-*-
#ifndef MEMSOURCEOBJECT_CC
#define MEMSOURCEOBJECT_CC
#include "MemSourceObject.hh"
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
// $Id: MemSourceObject.cc,v 1.2 1998/07/17 22:36:34 gsharma Exp $
//
//
//---------------------------------------------------------------------------

#include "MemSourceObject.hh"
#include "MemSourceState.hh"

MemSourceObject::MemSourceObject(char *myName, int max) {
  name = new char[strlen(myName)+1];
  strcpy(name,myName);
  
  state->nameOfObject = new char[strlen(myName)+1];
  strcpy(state->nameOfObject,myName);
  
  maxMemRequests = max;
  
  // set to an invalid distribution
  sourcedist = ALL;
  //cout << "Constructing source object " << endl;
}

MemSourceObject::~MemSourceObject() { 
  //  delete [] name;
  
  cout << ((MemSourceState *) state->current)->filter.getAvg()
       << " average access time." << endl;
  cout << "Produced " << ((MemSourceState *) state->current)->numMemRequests
       << " requests." << endl;
}

void 
MemSourceObject::initialize(){

  delete ((MemSourceState *) state->current)->gen; //delete old copy
  ((MemSourceState *) state->current)->gen = new MLCG(id, id+1);

  MemRequest *firstEvent = (MemRequest *) new char[sizeof(MemRequest)];
  new (firstEvent) MemRequest();

  firstEvent->recvTime = id+1;
  firstEvent->dest = id;  
  firstEvent->processor = id;
  firstEvent->size = sizeof(*firstEvent);
  firstEvent->eventId = 0;
  firstEvent->sender = id;
  firstEvent->startTime = 0;
#ifdef LTSFSCHEDULER
  firstEvent->alreadyProcessed = false ;
  //inputQ.insert(firstEvent, localId);
#endif
  sendEvent( firstEvent );
}

void 
MemSourceObject::executeProcess(){ 
  
  MemRequest* received ;
  received = (MemRequest *)getEvent();
  if(received != NULL){
    ((MemSourceState *)state->current)->filter.update((double)(state->current->lVT - received->startTime ) );

    int requestsCompleted = ((MemSourceState*)state->current)->numMemRequests ;
    if (requestsCompleted < maxMemRequests ) {

      if(requestsCompleted % 2000 == 0) {
	cout <<"MemSourceObject id " << id
	     <<" completed request "
	     <<requestsCompleted
	     << endl ;
      }
	
      MemRequest * newMemRequest = (MemRequest *) new char[sizeof(MemRequest)];
      new (newMemRequest) MemRequest();
      double ldelay=0;
      
      // check to see if we've sent all of the memRequests
      // check to see how many memRequests we should have sent for the time
      // we're currently at
      
      switch(sourcedist){
      case UNIFORM :
	{
	  Uniform uniform(first, second, ((MemSourceState *) state->current)->gen);
	  ldelay = uniform();
	  break;
	}
      case NORMAL :
	{
	  Normal normal(first, second, ((MemSourceState *) state->current)->gen);
	  ldelay = normal();
	  break;
	}
      case BINOMIAL :
	{
	  Binomial binomial((int)first, second, ((MemSourceState *) state->current)->gen);
	  ldelay = binomial();
	  break;
	}
      case POISSON :
	{
	  Poisson poisson(first, ((MemSourceState *) state->current)->gen);
	  ldelay = poisson();
	  break;
	}
      case EXPONENTIAL :
	{
	  NegativeExpntl expo(first, ((MemSourceState *) state->current)->gen);
	  ldelay = expo();
	  break;
	}
      case FIXED :
	ldelay = (int)first ;
	break;
      default :
	cerr << "Improper Distribution for a Source Object!!!" << endl;
	exit(-1);
	break;
      }       
      
      // we want the memRequest to get there at the exact scheduled time
      newMemRequest->recvTime = getLVT() + (int) ldelay;
      newMemRequest->dest = dest;  
      newMemRequest->processor = id;
      
      newMemRequest->size = sizeof(MemRequest);
      newMemRequest->startTime = getLVT();

      ((MemSourceState *) state->current)->numMemRequests++;

      ((MemSourceState *) state->current)->oldDelay = state->current->lVT ;
#ifdef LTSFSCHEDULER
      newMemRequest->alreadyProcessed = false ;
#endif
      sendEvent(newMemRequest);
    }
    else {
#ifdef OBJECTDEBUG
      outFile << "I think I'm done!(id = " << id << ")" << endl;
      outFile << "state: " << state->current->numMemRequests << endl;
      outFile << "max: " << maxMemRequests << endl;
#endif
    }

  }
  else {
#ifdef OBJECTDEBUG
    outFile << "going idle" << endl;
#endif
  }
};


BasicState*
MemSourceObject::allocateState() {
  return new MemSourceState();
}

void 
MemSourceObject::setDistribution( distribution_t distribution, 
			       double firstParameter, 
			       double secondParameter ) {
#ifdef DEBUG  
  cout << "--> inside setsourceDistribution " << endl ;
#endif   
   
  sourcedist = distribution ; // global variables
  first      = firstParameter ; // global variables
  second     = secondParameter ;  // global variables

};
#endif

