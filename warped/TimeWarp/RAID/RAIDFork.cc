//-*-c++-*-
#ifndef RAIDFork_CC
#define RAIDFork_CC
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
//
// $Id: RAIDFork.cc,v 1.2 1998/04/20 18:36:52 cyoung Exp $
//
//
//---------------------------------------------------------------------------

#include "RAIDFork.hh"
#include "RAIDForkState.hh"

#ifdef VERIFY
#include "FileQueue.hh"
#include "FileData.hh"
#endif

RAIDFork::RAIDFork(int myid, char* myname, int numDisks, int numProcesses, int firstDiskId)  : NUMDISKS(numDisks), NUMPROCESSES(numProcesses), STARTDISKID(firstDiskId) {
  
  id = myid;
  name = new char[strlen(myname)+1];
  strcpy(name,myname);

#ifdef VERIFY 
  outFileQ = new FileQueue(myname);
  numOutFiles = 1;
#endif
  cout << "Creating RAIDFork " << id << endl;
  cout << name << " created! " << endl;
}

int
RAIDFork::getParityDiskId(int stripeUnit) {
  // This function calculates the placement of the parity information
  // for the stripe. This function implements a right--symmetric parity
  // placement policy where the parity information for the first disk
  // is placed on the last disk, and the parity information for the next
  // stripe is placed on the next to last disk (and so on). see RAID
  // documentation for a descriptive example.
  return(NUMDISKS - (stripeUnit/(NUMDISKS -1))%NUMDISKS -1 + STARTDISKID);
}

void
RAIDFork::initialize()
{
  state->nameOfObject = new char[strlen(name) + 1];
  strcpy(state->nameOfObject, name);
}

void
RAIDFork::calculateStripeInfo(int stripeUnit,int logicalSector, 
			      int& diskNum, int& parityDisk){

  // This function calculates the placement of the stripe unit on the
  // disk array.
  // The stripes are laid across the disk array and one stripe will act
  // as the parity stripe. Each sector on the disk is given a unique
  // number called the logical sector number.

  logicalSector = stripeUnit/(NUMDISKS - 1);
  diskNum = stripeUnit%(NUMDISKS -1) + STARTDISKID;
  parityDisk = getParityDiskId(stripeUnit);
  // If we are to the left of the parity stripe then we must take that
  // into account.
  if ( diskNum > parityDisk) diskNum++;
}

void
RAIDFork::executeProcess() {

  RAIDRequest * recvEvent = (RAIDRequest*) getEvent();
  if (recvEvent != NULL) {

   
   RAIDRequest * newMsg = (RAIDRequest *) new char[sizeof(RAIDRequest)];
    new (newMsg) RAIDRequest();

    *newMsg = *recvEvent;

    // Since we may have several events arriving at the same time, we
    // process some events later, but we can determine when since we
    // know how long they will be delayed.
    // If we are working in the future then continue to do so, but if
    // there has been a long enough delay between our last processed
    // message and the current one then use the current time.
    
    if (((RAIDForkState *) state->current)->myTime < state->current->lVT) {
      ((RAIDForkState *) state->current)->myTime = state->current->lVT;
    }
    
    ((RAIDForkState *) state->current)->myTime++;
    newMsg->recvTime = ((RAIDForkState *) state->current)->myTime;
 
    int firstStripeUnit = newMsg->startStripe;

    if (newMsg->sender == newMsg->sourceId) {
#ifdef OBJECTDEBUG
      if (newMsg->read == false) {
	outFile << "Received source message for a Write ";
      }
      else {
	outFile << "Received source message for a Read ";
      }
      outFile << "of size " << newMsg->sizeRead 
	      << " starting at stripe unit " 
	      << firstStripeUnit << " at time " << getLVT() << endl;
      outFile << *newMsg << "\n";
#endif

      if (newMsg->read == true) {
	int parityDiskId;
	for (int count = 0; count < newMsg->sizeRead; count++) {
	  newMsg->startStripe = firstStripeUnit + count;
	  calculateStripeInfo(newMsg->startStripe,newMsg->logicalSector,
			      newMsg->dest,parityDiskId);
	  
#ifdef OBJECTDEBUG
	  outFile << "Sending message for stripe : [ " 
		  << newMsg->startStripe
		  << "] at " << newMsg->recvTime << " to " 
		  << newMsg->dest << "(" << newMsg->eventId 
		  << ")" << endl;
#endif
	  
	
	  sendMsg(newMsg);

	  newMsg = (RAIDRequest *) new char[sizeof(RAIDRequest)];
	  new (newMsg) RAIDRequest();

	  *newMsg = *recvEvent;
	  newMsg->recvTime = ((RAIDForkState *) state->current)->myTime;
	  
	}
	if (newMsg->sizeRead > 0) {
	  delete [] (char *)newMsg; // Get rid of unused one
	}
      }
      else {
	// Its a write, so send the paritys.
	// See how many parity messages needed.
	int numParity = 0;
	int parityDiskId;

	// Kludge
	// calculate the number of parity messages needed
	int lastParityDiskId  = -1;
	int count;
	for (count = 0; count < newMsg->sizeRead; count++) {
	  parityDiskId = getParityDiskId(firstStripeUnit + count);

	  if (parityDiskId != lastParityDiskId) {
	    numParity++;
	    lastParityDiskId = parityDiskId;
	  }
	}

	lastParityDiskId = -1;
	for (count = 0; count < newMsg->sizeRead; count++) {
	  newMsg->startStripe = firstStripeUnit + count;
	  calculateStripeInfo(newMsg->startStripe,newMsg->logicalSector,
			      newMsg->dest,parityDiskId);

	  newMsg->serverId = newMsg->dest;
	  newMsg->parityMsg = false;
	  newMsg->sizeParity = numParity;

	  // Do we need different parity block yet.
	  if (parityDiskId != lastParityDiskId) {
	    RAIDRequest * parityMsg = 
	      (RAIDRequest *) new char[sizeof(RAIDRequest)];
	    new (parityMsg) RAIDRequest();

	    // send a request for the new information.
	    *parityMsg = *newMsg;
	    parityMsg->dest = parityDiskId;
	    parityMsg->serverId = parityMsg->dest;
	    parityMsg->parityMsg = true;

#ifdef OBJECTDEBUG
	    outFile << "Sending parity message for stripe : [ " 
		    << parityMsg->startStripe
		    << "] at " << parityMsg->recvTime << " to " 
		    << parityMsg->dest << "(" << parityMsg->eventId 
		    << ")" << endl;
#endif

	    lastParityDiskId = parityDiskId;

	    sendMsg(parityMsg);
	  }
#ifdef OBJECTDEBUG
	  outFile <<"Sending message for stripe : [ " << newMsg->startStripe
		  << "] at " << newMsg->recvTime << " to " 
		  << newMsg->dest << "(" << newMsg->eventId 
		  << ")" << endl;
#endif

	  sendMsg(newMsg);
	  newMsg = (RAIDRequest *) new char[sizeof(RAIDRequest)];
	  new (newMsg) RAIDRequest();

	  *newMsg = *recvEvent;
	  newMsg->recvTime = ((RAIDForkState *) state->current)->myTime;
	 
	}
	delete [] (char *)newMsg; // Get rid of last unsent message
      } // if (read)
    } // if (sender == source)
  }
}

void
RAIDFork::sendMsg(RAIDRequest *newRequest) {
#ifdef VERIFY
  strstream str;
  str << "Sending token [" << newRequest->tokenNumber << "] to: " 
      << newRequest->dest << " for process " << newRequest->sourceId
      << " at " << state->current->lVT << '\n' << '\0'<< ends;
  FileData *outputLine = new FileData;
  outputLine->line = str.str();
  outputLine->length = strlen(outputLine->line) +1;
  outputLine->time = state->current->lVT;
  outFileQ[0].insert(outputLine);
#endif
  sendEvent(newRequest);
}

BasicState*
RAIDFork::allocateState() {
  return new RAIDForkState();
}

#endif
