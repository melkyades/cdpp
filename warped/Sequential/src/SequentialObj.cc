//-*-c++-*-
#ifndef SEQUENTIALOBJ_CC
#define SEQUENTIALOBJ_CC
// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
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
// $Id: SequentialObj.cc,v 1.4 1999/10/28 04:55:46 mal Exp $
//
//---------------------------------------------------------------------------

#include "SequentialObj.hh"

SequentialObj::SequentialObj():stdoutFileQ(-1){
  state          = new SequentialState;
  state->current = NULL;

  numInFiles   = 0;
  numOutFiles  = 0;
  eventCount   = 0 ; 
  eventCounter = 0ll;

  inFileQ      = NULL;
  outFileQ     = NULL;
}

SequentialObj::SequentialObj( int tempId ):stdoutFileQ(-1){
  id = tempId;
  state          = new SequentialState;
  state->current = NULL;
  
  numInFiles   = 0;
  numOutFiles  = 0;
  eventCount   = 0 ; 
  eventCounter = 0ll;
}

SequentialObj::~SequentialObj(){
#ifdef LPDEBUG
  *lpFile << "all done!\n";
#endif
  
  outputGcollect(PINFINITY);
  inputGcollect(PINFINITY);
  
  if (inFileQ != NULL) {
    delete [] inFileQ;
  }
  
  if (outFileQ != NULL) {
    delete [] outFileQ;
  }
  
  delete state->current;
  delete state;
}

BasicEvent*
SequentialObj::getEvent(){
  register BasicEvent *toExecute;
  
  //  if ((toExecute = inputQ.getEvent()) != NULL) {
  //inoutQ.getAndSeek() has been changed to getEvent

  if ((toExecute = inputQ.getEvent()) != NULL) {
    state->current->lVT = toExecute->recvTime;
    toExecute->alreadyProcessed = true;
  }
  
  return(toExecute);
}

void
SequentialObj::sendEvent(BasicEvent* toSend ) {
  int receiver     = toSend->dest;

  toSend->sendTime = state->current->lVT;
  toSend->eventId  = eventCounter;
  toSend->sender   = id;
  
  eventCounter++;
  // we now have all the info we need.  We want to send the positive
  // message for real. grab the communication handle of the receiving
  // object and send message send the positve message.
  toSend->alreadyProcessed = false;
  commHandle[receiver].ptr->recvEvent(toSend);
}

void 
SequentialObj::simulate(){
  for (int i = 0; i < numInFiles; i++) {

    //inFileQ[i].storePos(inputQ.get()->recvTime,
    //inputQ.get() has been changed to inputQ.peekEvent()

    inFileQ[i].storePos(inputQ.peekEvent()->recvTime,
			inFileQ[i].access().tellg());
  }
  executeProcess();
}

int
SequentialObj::inputGcollect(VTime gtime) {
#ifdef LPDEBUG
  *lpFile << name << ":input time: " << gtime << endl;
#endif
  
  for(register int i = 0; i < numInFiles; i++) {
    inFileQ[i].gcollect(gtime);
  }
  
  return 0;
}

void
SequentialObj::outputGcollect(VTime gtime) {
  for(register int i = 0; i < numOutFiles; i++) {
    outFileQ[i].gcollect(gtime);
  }
}

void 
SequentialObj::timeWarpInit() {
  state->current        = allocateState();
  state->current->dirty = false;
  state->current->lVT   = ZERO;
}  

void
SequentialObj::deAllocateState(BasicState *oldState) {
  delete oldState;
}

BasicState* 
SequentialObj::getCurrentState() const  {
  return state->current;
}

#endif
