//-*-c++-*-
#ifndef POLICESOURCEOBJECT_CC
#define POLICESOURCEOBJECT_CC
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
//
// $Id: PoliceSourceObject.cc,v 1.2 1998/05/07 22:43:39 gsharma Exp $
//
// 
//
//---------------------------------------------------------------------------

#include "PoliceSourceObject.hh"


PoliceSourceObject::PoliceSourceObject(int myid, const char* myname, 
				       int numCars) {
  int i ;
#ifdef OBJECTDEBUG
  cout << "--> Constructing Police Source" << endl ;
#endif

  name = new char[strlen(myname)+1];
  strcpy(name,myname);
#ifdef OBJECTDEBUG
  cout << name << " created ! " << endl ;
#endif
  numberOfCars = numCars ;
  id = myid ;
  callsRecord = new int[numberOfCars];
  for(i=0;i< numberOfCars;i++) callsRecord[i]=0;

}

void
PoliceSourceObject::initialize(){
  BasicEvent * newToken = new BasicEvent;
  newToken->sender = id;
  newToken->dest = id ;
  newToken->size = sizeof(*newToken);
  newToken->recvTime = ((PoliceSourceState *)state->current)->lVT ;
  newToken->sendTime = ((PoliceSourceState *)state->current)->lVT ;
#ifdef LTSFSCHEDULER
  newToken->alreadyProcessed = false ;
  inputQ.insert(newToken, localId); // inserting into input queue
                                    // to have fully event driven system 
#else
  inputQ.insert(newToken);
#endif

  switch(sourcedist){
  case UNIFORM :
    uniform = new Uniform(first, second, (((PoliceSourceState *)state->current)->gen));
    break;
  case NORMAL :
    normal = new Normal(first, second, (((PoliceSourceState *)state->current)->gen));
    break;
  case BINOMIAL :
    binomial = new Binomial((int)first,second,(((PoliceSourceState *)state->current)->gen));
    break;
  case POISSON :
    poisson = new Poisson(first, (((PoliceSourceState *)state->current)->gen));
    break;
  case EXPONENTIAL :
    expo = new NegativeExpntl(first, (((PoliceSourceState *)state->current)->gen));
    break;
  case FIXED :
    break;
  default :
    cerr << "Improper Distribution for a Source Object!!!" << endl;
    exit(-1);
    break;
  }       
  
}

PoliceSourceObject::~PoliceSourceObject(){

#ifdef OBJECTDEBUG
  register int i ;
#endif
  
#ifdef OBJECTDEBUG
   cout << "--> Destructing Police Source" << endl ;
#endif
   cout << "Source " << name << " produced "
	<< ((PoliceSourceState *)state->current)->numTokens << endl ; 

#ifdef OBJECTDEBUG
   for(i=0;i < numberOfCars;i++) 
     cout << "Car Number(" << i << ") processed " << callsRecord[i] 
	  << " calls " << endl ;
#endif
   delete [] callsRecord ;
   //   delete [] name ;
  
}

// closed network model so source accepts messages also.


int 
PoliceSourceObject::setDelay(){
  double ldelay ;

  switch(sourcedist){
  case UNIFORM :
    ldelay = uniform->operator()();
    break;
  case NORMAL :
    ldelay = normal->operator()();
    break;
  case BINOMIAL :
    ldelay = binomial->operator()();
    break;
  case POISSON :
    ldelay = poisson->operator()();
    break;
  case EXPONENTIAL :
    ldelay = expo->operator()();
    break;
  case FIXED :
    ldelay = (int)first ;
    break;
  default :
    cerr << "Improper Distribution for a Source Object!!!" << endl;
    break;
  }
  return ( (int) ldelay );
  
}


void 
PoliceSourceObject::newMsg(int) {
  PoliceToken *newToken = new PoliceToken;
  BasicEvent* dummyToken = new BasicEvent ;
  int ldelay;
  
  ldelay = setDelay();

  newToken->recvTime = getLVT() + ldelay;
  newToken->dest = dest;
  newToken->stationNumber = id ;
  newToken->carId = setDelay() % numberOfCars ;

  dummyToken->recvTime = newToken->recvTime ;
  dummyToken->dest = id ;
  dummyToken->sender = id ;

  callsRecord[newToken->carId] = callsRecord[newToken->carId] + 1;
  
  newToken->callId = ((PoliceSourceState *)state->current)->lVT % 1000 ; 
             
  newToken->priority = setDelay() % 3;
       
  newToken->support = ((PoliceSourceState *)state->current)->lVT % 2 ;
#ifdef LTSFSCHEDULER
  newToken->alreadyProcessed = false ;
#endif
  newToken->size = sizeof(*newToken);
  dummyToken->size = sizeof(*dummyToken);

#ifdef OBJECTDEBUG
  cout << "CarId = " << newToken->carId << " CallId =   " 
       << newToken->callId << " sent by " 
       << name << newToken->stationNumber << " priority = "
       << newToken->priority << endl ;
#endif
  sendEvent(newToken);
  ((PoliceSourceState *)state->current)->CallsPending = false;

  ((PoliceSourceState *)state->current)->numTokens++;
#ifdef LTSFSCHEDULER  
  dummyToken->alreadyProcessed = false ;
#endif
  dummyToken->sendTime = ((PoliceSourceState *)state->current)->lVT; 

  ((PoliceSourceState *)state->current)->lVT = ((PoliceSourceState *)state->current)->lVT + (int) ldelay;
  //sendEvent(dummyToken);
#ifdef LTSFSCHEDULER
  inputQ.insert(dummyToken, localId);
#else
  inputQ.insert(dummyToken);
#endif
}


void 
PoliceSourceObject::executeProcess(){  
  PoliceToken * revEvent ;
  revEvent = (PoliceToken *) getEvent();
  if(revEvent != NULL){ 

  register int callType;

     if ( ((PoliceSourceState *)state->current)->numTokens < ((PoliceSourceState *)state->current)->maxTokens ) { 
      callType = setDelay()%3;
      newMsg(callType);
      }
    else {
      // atleast one cycle completed 
      if(((PoliceSourceState *)state->current)->hours != SHIFT){
            ((PoliceSourceState *)state->current)->hours++ ;
            ((PoliceSourceState *)state->current)->numTokens = 0 ;
            callType = setDelay()%3;
            newMsg(callType);
#ifdef OBJECTDEBUG
            cout <<"new Assignment CarId = "<<((PoliceSourceState *)state->current)->numTokens-1  
		 << endl ;
#endif
      }
    }
  }
}


void 
PoliceSourceObject::setDistribution( distribution_t distribution, 
			       double firstParameter, 
			       double secondParameter ) {
#ifdef OBJECTDEBUG  
  cout << "--> inside setsourceDistribution " << endl ;
#endif   
   
  sourcedist = distribution ; // global variables
  first      = firstParameter ; // global variables
  second     = secondParameter ;  // global variables

};

#endif






