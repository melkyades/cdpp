//-*-c++-*-
#ifndef POLICESERVEROBJECT_CC
#define POLICESERVEROBJECT_CC
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
//
// $Id: PoliceServerObject.cc,v 1.2 1998/05/07 22:43:37 gsharma Exp $
//
//
// 
//
//---------------------------------------------------------------------------

#include "PoliceServerObject.hh"

void
PoliceServerObject::initialize(){

  switch(serverDist){
  case UNIFORM :
    uniform = new Uniform(first, second, (((PoliceServerState *)state->current)->gen));
    break;
  case NORMAL :
    normal = new Normal(first, second, (((PoliceServerState *)state->current)->gen));
    break;
  case BINOMIAL :
    binomial = new Binomial((int)first,second, (((PoliceServerState *)state->current)->gen));
    break;
  case POISSON :
    poisson = new Poisson(first, (((PoliceServerState *)state->current)->gen));
    break;
  case EXPONENTIAL :
    expo = new NegativeExpntl(first, (((PoliceServerState *)state->current)->gen));
    break;
  case FIXED:
    break;
  default :
    cerr << "Invalid Delay type!!!!!!!!!!!!!!" << endl;
    exit(-1);
    break;
  }       
  
}

void
PoliceServerObject::executeProcess(){
 
  PoliceToken *recvEvent ;

#ifdef OBJECTDEBUG  
  cout << "Entering executeProcess in ServerObject " << endl ;
#endif 

  recvEvent = (PoliceToken *) getEvent();
       
  if ( recvEvent != NULL ) {
    PoliceToken *newEvent = (PoliceToken *)new char[recvEvent->size];
    memcpy( newEvent, recvEvent, recvEvent->size);
    
    switch(serverDist){
    case UNIFORM :
      delay = uniform->operator()();
      break;
    case NORMAL :
      delay = normal->operator()();
      break;
    case BINOMIAL :
      delay = binomial->operator()();
      break;
    case POISSON :
      delay = poisson->operator()();
      break;
    case EXPONENTIAL :
      delay = expo->operator()();
      break;
    case FIXED:
      delay = (int) first;
      break;
    default :
      cerr << "Invalid Delay type!!!!!!!!!!!!!!" << endl;
      delay = 0;
      break;
    }       

    
#ifdef OBJECTDEBUG
      cout<<"-->Finished setting up delay:new delay = " << (int) delay << endl ;
#endif
    newEvent->recvTime = getLVT() + (int) delay;
     
    // send event to server or join or fork as specified by user
    newEvent->dest = dest ;
    
    if(newEvent->priority == 1) ((PoliceServerState *)state->current)->numPriorityCalls++;
    sendEvent(newEvent);
#ifdef OBJECTDEBUG
    cout << "Server-Message from " << newEvent->sender << " to " 
	 << newEvent->dest << endl ;
    cout << "Server-Message id " << newEvent->eventId << ", sendtime : " 
	 << newEvent->sendTime <<", recvtime : " << newEvent->recvTime 
	 << endl ;
#endif
  }  
};


void
PoliceServerObject::setServerDistribution(distribution_t distribution, 
			    double firstParameter, double secondParameter,
                            double (*func)() ) {
  serverDist   = distribution ; 
  first        = firstParameter ;        
  second       = secondParameter ;       
  userFunction = func;

};
#endif








