//-*-c++-*-
#ifndef JOINOBJECT_CC
#define JOINOBJECT_CC
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
// $Id: JoinObject.cc,v 1.1 1998/03/07 21:06:36 dmadhava Exp $
//
//
//---------------------------------------------------------------------------

#include "JoinObject.hh"
#include "JoinState.hh"

JoinObject::JoinObject(char *myName) {
  name = new char[strlen(myName)+1];
  strcpy(name,myName);
}

JoinObject::~JoinObject() { 
  //cout << " destructing Join object " << endl; 
}

void
JoinObject::executeProcess(){
  joinDelay = 0;

  BasicEvent *recvEvent ;
#ifdef DEBUG
  cout << "Entering executeProcess in JoinObject " << endl ;
#endif
  
  recvEvent = getEvent();
  if ( recvEvent == NULL ) {
    cerr << name << " scheduled for execution with no events " << endl;
  }
  else {  
    // send event to server or join or fork or queue as specified by user
    BasicEvent *newEvent = (BasicEvent *)new char[recvEvent->size];
    memcpy( newEvent, recvEvent, recvEvent->size);
    
    newEvent->dest = dest;
    newEvent->recvTime = newEvent->recvTime + joinDelay ;
    
    sendEvent(newEvent);
  }
}

void
JoinObject::setJoinDestination(int destination, int delay){
  dest = destination;
  joinDelay = delay ;
}

BasicState*
JoinObject::allocateState() {
  return (BasicState *) new JoinState();
}

#endif
