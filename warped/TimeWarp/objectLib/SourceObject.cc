//-*-c++-*-
#ifndef SOURCEOBJECT_CC
#define SOURCEOBJECT_CC
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
// $Id: SourceObject.cc,v 1.1 1998/03/07 21:06:55 dmadhava Exp $
//
//
//---------------------------------------------------------------------------

#include "SourceObject.hh"
#include "SourceState.hh"

void 
SourceObject::executeProcess(){ 
  BasicEvent* newToken = new BasicEvent();
  double ldelay = 0;
  Uniform uniform(first, second, ((SourceState *) state->current)->gen);
  Normal normal(first, second, ((SourceState *) state->current)->gen);
  Binomial binomial((int)first, second, ((SourceState *) state->current)->gen);
  Poisson poisson(first, ((SourceState *) state->current)->gen);
  NegativeExpntl expo(first, ((SourceState *) state->current)->gen);
  
  // check to see if we've sent all of the tokens
  if ( ((SourceState *) state->current)->numTokens < ((SourceState *) state->current)->maxTokens ) {
    // check to see how many tokens we should have sent for the time
    // we're currently at
#ifdef DEBUG
    cout << "--> Entering executeprocess in sourceobject " << endl ;
#endif
    switch(sourcedist){
    case UNIFORM :
      ldelay = uniform();
      break;
    case NORMAL :
      ldelay = normal();
      break;
    case BINOMIAL :
      ldelay = binomial();
      break;
    case POISSON :
      ldelay = poisson();
      break;
    case EXPONENTIAL :
      ldelay = expo();
      break;
    case FIXED :
      ldelay = (int)first ;
      break;
    default :
      cerr << "Improper Distribution for a Source Object!!!" << endl;
      exit(-1);
      break;
    }       
#ifdef DEBUGi
    cout << "-->Finished setting up delay:new delay = " << ldelay 
	 << endl ;
#endif
    // we want the token to get there at the exact scheduled time
    newToken->recvTime = ((SourceState *) state->current)->oldDelay + (int) ldelay;
    
    newToken->dest = dest;  
    
    newToken->size = sizeof(*newToken);
    
    
#ifdef DEBUG
    cout << "Source sending message from " << newToken->sender 
	 << " to " << newToken->dest << endl ;
     cout << "Message id " << newToken->eventId << ", sendtime : " 
	  << newToken->sendTime <<", recvtime : " 
	  << newToken->recvTime << endl ;
#endif
    ((SourceState *) state->current)->numTokens++;
    ((SourceState *) state->current)->lVT = ((SourceState *) state->current)->lVT + (int) ldelay;
    ((SourceState *) state->current)->oldDelay = ((SourceState *) state->current)->lVT ;
    sendEvent(newToken);
  }
}

SourceObject::SourceObject(char *myName) {
  // cout << "--> Constructing source" << endl;

  // set to an invalid distribution
  sourcedist = ALL;
  // creating the dummy token 
  BasicEvent * newToken = new BasicEvent;
  newToken->dest = 0 ;
  newToken->size = sizeof(*newToken);
  newToken->recvTime = 0 ;

  name = new char[strlen(myName)+1];
  strcpy(name,myName);
  ((SourceState *) state->current)->gen = new MLCG(id, id+1);
};

SourceObject::~SourceObject() { 
}

void 
SourceObject::setDistribution(distribution_t distribution, 
					    double firstParameter, 
			                    double secondParameter) {
#ifdef DEBUG  
  cout << "--> inside setsourceDistribution " << endl ;
#endif   
  
  sourcedist = distribution ; // global variables
  first      = firstParameter ; // global variables
  second     = secondParameter ;  // global variables
  
}

BasicState*
SourceObject::allocateState() {
  return new SourceState();
}

#endif
