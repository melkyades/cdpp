//-*-c++-*-
#ifndef SERVEROBJECT_CC
#define SERVEROBJECT_CC
// Copyright (c) 1994,1995 Ohio Board of Regents and the University of
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
// $Id: ServerObject.cc,v 1.1 1998/03/07 21:06:46 dmadhava Exp $
//
//---------------------------------------------------------------------------

#include "ServerObject.hh"
#include "ServerState.hh"

ServerObject::ServerObject(char *myName) {
  name = new char[strlen(myName)+1];
  strcpy(name,myName);
}

ServerObject::~ServerObject() {
  //cout << " destructing Server object " << endl ;
}

void
ServerObject::initialize() {
  delete ((ServerState *) getCurrentState())->gen;
  ((ServerState *) getCurrentState())->gen = new MLCG(id, (id + 1));
}

void
ServerObject::executeProcess(){
 
  BasicEvent *recvEvent ;

#ifdef DEBUG  
  cout << "Entering executeProcess in ServerObject " << endl ;
#endif 

  recvEvent = getEvent();
       
  if ( recvEvent == NULL ) {
    cerr << name << " scheduled with no events to execute" << endl;
  }
  else {

    BasicEvent *newEvent = (BasicEvent *)new char[recvEvent->size];
    memcpy( newEvent, recvEvent, recvEvent->size );


    switch(serverDist){
    case UNIFORM :
      {
	Uniform uniform(first, second, ((ServerState *) state->current)->gen);
	delay = uniform();
	break;
      }
    case NORMAL :
      {
	Normal normal(first, second, ((ServerState *) state->current)->gen);
	delay = normal();
	break;
      }
    case BINOMIAL :
      {
	Binomial binomial((int)first,second, ((ServerState *) state->current)->gen);
	delay = binomial();
	break;
      }
    case POISSON :
      {
	Poisson poisson(first, ((ServerState *) state->current)->gen);
	delay = poisson();
	break;
      }
    case EXPONENTIAL :
      {
	NegativeExpntl expo(first, ((ServerState *) state->current)->gen);
	delay = expo();
	break;
      }
    case FIXED:
      delay = (int) first;
      break;
    default :
      cerr << "Invalid Delay type!!!!!!!!!!!!!!" << endl;
      delay = 0;
      break;
    }       
    
#ifdef DEBUG
      cout<<"-->Finished setting up delay:new delay = " << (int) delay << endl ;
#endif
    newEvent->recvTime = getLVT() + (int) delay;
     
    // send event to server or join or fork as specified by user
    newEvent->dest = dest ;
    newEvent->sender = id;
    sendEvent(newEvent);
#ifdef DEBUG
    cout << "Server-Message from " << newEvent->sender << " to " 
	 << newEvent->dest << endl ;
    cout << "Server-Message id " << newEvent->eventId << ", sendtime : " 
	 << newEvent->sendTime <<", recvtime : " << newEvent->recvTime 
	 << endl ;
#endif
  }  
};
 
void
ServerObject::setServerDistribution(distribution_t distribution, 
				    double firstParameter,
				    double secondParameter,
				    double (*func)() ) {
  serverDist   = distribution ; 
  first        = firstParameter ;        
  second       = secondParameter ;       
  userFunction = func;

};

BasicState*
ServerObject::allocateState() {
  return (BasicState *) new ServerState;
}

#endif
