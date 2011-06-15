//-*-c++-*-
#ifndef RAIDPROCESS_CC
#define RAIDPROCESS_CC
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
// $Id: RAIDProcess.cc,v 1.2 1998/04/20 18:36:53 cyoung Exp $
//
//
//
//---------------------------------------------------------------------------

#include "RAIDProcess.hh"
#include "FileQueue.hh"
#include "FileData.hh"
#include "RAIDProcessState.hh"

RAIDProcess::RAIDProcess(int myid, const char* myname, int mydest,
				   int maxDisks, DISK_TYPE theDisk, 
				   int maxrequests, int firstDisk) {
#ifdef VERIFY 
  outFileQ = new FileQueue(myname);
  numOutFiles = 1;
#endif
  id          = myid;
  dest        = mydest;
  maxRequests = maxrequests;
  MAXDISKS    = maxDisks;
  firstDiskId = firstDisk;
  diskType    = theDisk;
  
  name        = new char[strlen(myname)+1];
  strcpy(name, myname);
}

void
RAIDProcess::configureRAIDProcess(int myid, const char* myname, int mydest,
				   int maxDisks, DISK_TYPE theDisk,
				   int maxrequests, int firstDisk) {
  newConfiguration(myid,myname,mydest,maxDisks,theDisk,maxrequests,firstDisk);
}

void 
RAIDProcess::newConfiguration(int, const char* myname, int,
				   int, DISK_TYPE theDisk,
				   int, int firstDisk) {

  int i;

  state->nameOfObject = new char[strlen(myname)+1];
  strcpy(state->nameOfObject,myname);
  ((RAIDProcessState *) state->current)->firstDiskId = firstDisk;

  switch (theDisk) {
  case FUJITSU:
    MAXCYLINDER  = 1944;
    MAXTRACK     = 20;
    MAXSECTOR    = 88;
    break;
  case FUTUREDISK:
    MAXCYLINDER  = 2500;
    MAXTRACK     = 20;
    MAXSECTOR    = 132;
    break;
  case LIGHTNING:
    MAXCYLINDER  = 949;
    MAXTRACK     = 14;
    MAXSECTOR    = 48;
    break;
  }
 
  TOTALSTRIPESIZE = (MAXDISKS-1)*MAXCYLINDER*MAXTRACK*MAXSECTOR;
  //  MAXSTRIPESIZE = 2*MAXDISKS;
  MAXSTRIPESIZE = MAXDISKS;

  DiscreteUniform read(0,1, ((RAIDProcessState *) state->current)->gen);
  
  // Add some randomness to the simulation.
  for ( i = 0; i < id%40; i++) {
    read();
  }

  // Ignore the first read completion.
  ((RAIDProcessState *) state->current)->numReads--;

  // make the process look for the first Request.
  ((RAIDProcessState *) state->current)->size = 1;
  // set the number of disks in the state.
  ((RAIDProcessState *) state->current)->numDisks = MAXDISKS;
  // set Disk Requests
  ((RAIDProcessState *) state->current)->diskRequests = new int[MAXDISKS];
  
  for (i = 0; i < MAXDISKS; i++) {
    ((RAIDProcessState *) state->current)->diskRequests[i] = 0;
  }

#ifdef OBJECTDEBUG
  DiscreteUniform cylinder(0,MAXDISKS*MAXCYLINDER, ((RAIDProcessState *) state->current)->gen);
  DiscreteUniform sector(0,MAXSECTOR, ((RAIDProcessState *) state->current)->gen);
  DiscreteUniform size(1,MAXSTRIPESIZE, ((RAIDProcessState *) state->current)->gen);
  outFile << "For [" << MAXDISKS << "] disks, each with [" << MAXTRACK 
	  << "] tracks with [" << MAXSECTOR << "] sectors\n";

  outFile << "cylinder = " << cylinder() << " sector = " 
	  << sector() << " size = " << size()
	  << "\n";
#endif
}

RAIDProcess::~RAIDProcess() { 
    cout << "--> Destructing source" << "\n" ;
    cout << name <<" produced " << ((RAIDProcessState *) state->current)->numRequests 
	 << " requests" << "\n";
    cout << name <<" received " << ((RAIDProcessState *) state->current)->numReads
	 << " reads" << "\n";
    cout << name <<" received " << ((RAIDProcessState *) state->current)->numWrites  
	 << " writes" << "\n";
#ifdef OBJECTDEBUG
    outFile.close();
#endif
};

void
RAIDProcess::initialize() {

  newConfiguration(id, name, dest, MAXDISKS, diskType, maxRequests,
		   firstDiskId);
    
  delete ((RAIDProcessState *) getCurrentState())->gen;
  ((RAIDProcessState *) getCurrentState())->gen = new MLCG(id, (id + 1));

  RAIDRequest* newRequest = (RAIDRequest *) new char[sizeof(RAIDRequest)];
  new (newRequest) RAIDRequest();

  newRequest->sourceId = id;
  newRequest->serverId = -1;
  newRequest->sinkId   = -1;
  newRequest->startCylinder = -1;
  newRequest->startStripe  = -1;
  newRequest->startSector  = -1;
  newRequest->sizeRead     = 1;
  newRequest->logicalSector = -1;
  newRequest->recvTime = 1;
  newRequest->read = true;
  newRequest->dest = id;  
  newRequest->parityMsg = false;
  newRequest->beginningOfStrype = -1;
  
  newRequest->size = sizeof(RAIDRequest);
  ((RAIDProcessState *) state->current)->beginningOfStrype = newRequest->beginningOfStrype;
  ((RAIDProcessState *) state->current)->stopProcessing = false;
  sendEvent(newRequest);

};

void 
RAIDProcess::executeProcess(){  
     
  RAIDRequest* recvEvent;

  recvEvent = (RAIDRequest*) getEvent();

  if (recvEvent != NULL) {

#ifdef VERIFY
      strstream str;
      str << "Received Message from " << recvEvent->sender
	  << " at time " << recvEvent->recvTime 
	  << "\n"
	  << *((RAIDRequest*)recvEvent) << '\n';
      if (((RAIDProcessState *) state->current)->stopProcessing == true) {
	str << " and I'm stopped";
      }
      str << '\n' << '\0' << ends;
      FileData *outputLine = new FileData;
      outputLine->line = str.str();
      outputLine->length = strlen(outputLine->line) +1;
      outputLine->time = state->current->lVT;
      outFileQ[0].insert(outputLine);
#endif

#ifdef OBJECTDEBUG
    outFile << name << *((RAIDProcessState *) state->current) << "\n";
    outFile << "Received " << *recvEvent << " at "
	    << state->current->lVT << "\n";
#endif
    if (((RAIDProcessState *) state->current)->stopProcessing == true) {
      //      *lpFile << name << ":Stopped" << endl;
      //      *lpFile << name << ":" << *recvEvent << "\n";

#ifdef OBJECTDEBUG
//      outFile << name << " stopped!\n";
#endif
      return;
    }
    if (recvEvent->tokenNumber != ((RAIDProcessState *) state->current)->numRequests) {
      //    if (recvEvent->beginningOfStrype != ((RAIDProcessState *) state->current)->beginningOfStrype) {
      //      *lpFile << name << ":Stopping\n";
      //      *lpFile << name << ":recvEvent [" << recvEvent->beginningOfStrype
      //	      << "] State [" << ((RAIDProcessState *) state->current)->beginningOfStrype << "]"
      //	      << "\n";
#ifdef VERIFY
      strstream str;
      str << "Stopping due to reciving token #[" << recvEvent->tokenNumber
	  << "] from Disk " << recvEvent->sender  << " at time "
	  << recvEvent->recvTime << " when token#[" 
	  << ((RAIDProcessState *) state->current)->numRequests
	  << "] was expected\n" << '\0' << ends;
      //      str << "Stopping due to reciving [" << recvEvent->beginningOfStrype
      //	  << "] when strype requested was [" 
      //	  << ((RAIDProcessState *) state->current)->beginningOfStrype << "]\n" << '\0' << ends;
      FileData *outputLine = new FileData;
      outputLine->line = str.str();
      outputLine->length = strlen(outputLine->line) +1;
      outputLine->time = ((RAIDProcessState *) state->current)->lVT;
      outFileQ[0].insert(outputLine);
#endif
      ((RAIDProcessState *) state->current)->stopProcessing = true;
      return;
    }

    // keep memory from being munged on errors and first event
    if ( (recvEvent->sender - firstDiskId) >= 0 && 
	 (recvEvent->sender - firstDiskId) < MAXDISKS) {
      ((RAIDProcessState *) state->current)->diskRequests[recvEvent->sender-firstDiskId]++;
    }
    else {
      if (recvEvent->sender != id) {
	cout << name << " ignoring event from " << recvEvent->sender
	     << " in diskRequests count\n";
      }
    }
    // If this is a write then parity information will be expected.
    if ( (recvEvent->read == false) && (((RAIDProcessState *) state->current)->paritySize == -1)) { 
      // our first write message comes back home, lets see how many
      // paritys were generated.
      ((RAIDProcessState *) state->current)->paritySize =  recvEvent->sizeParity;

    }


    if (recvEvent->parityMsg == true) {
      // Its a parity msg so decrement the parity
      ((RAIDProcessState *) state->current)->paritySize--;

    }
    else {
      ((RAIDProcessState *) state->current)->size--;
    }

    if (recvEvent->read == true) {
      // see if the operation is complete
      if ( (((RAIDProcessState *) state->current)->size == 0) && (((RAIDProcessState *) state->current)->paritySize == -1 )) {
	((RAIDProcessState *) state->current)->diskOperationPending = false;
	((RAIDProcessState *) state->current)->numReads++;
	((RAIDProcessState *) state->current)->size = 0;
#ifdef VERIFY
	strstream str;
	str << "Finished " << ((RAIDProcessState *) state->current)->numRequests << " Requests!"
	    << "\n" << '\0' << ends;
	FileData *outputLine = new FileData;
	outputLine->line = str.str();
	outputLine->length = strlen(outputLine->line) +1;
	outputLine->time = ((RAIDProcessState *) state->current)->lVT;
	outFileQ[0].insert(outputLine);
#endif
	int i;
	for (i = 0; i < MAXDISKS; i++) {
	  ((RAIDProcessState *) state->current)->diskRequests[i] = 0;
	}

if (((RAIDProcessState *) state->current)->numRequests%5000 == 0) {
	  cout << "[" <<name << "] disk operation [" 
	       << ((RAIDProcessState *) state->current)->numRequests << "] done!" << endl;
	}

	if (((RAIDProcessState *) state->current)->numRequests < maxRequests) newMsg();
      } else {
#ifdef VERIFY
      strstream str;
      int i;
      str << " waiting on " << ((RAIDProcessState *) state->current)->size << " Requests!"
	  << "\n";
      for (i = 0; i < ((RAIDProcessState *) state->current)->numDisks; i++) {
	str << i + ((RAIDProcessState *) state->current)->firstDiskId << ": " 
	    << ((RAIDProcessState *) state->current)->diskRequests[i] << "\t\t";
	if (i%4 == 0 && i !=0 ) str << "\n";
      }
      str << "\n" << '\0' << ends;

      FileData *outputLine = new FileData;
      outputLine->line = str.str();
      outputLine->length = strlen(outputLine->line) +1;
      outputLine->time = ((RAIDProcessState *) state->current)->lVT;
      outFileQ[0].insert(outputLine);
#endif
      }
    } // event is a write
    else {
      // see if the operation is complete
      if ( (((RAIDProcessState *) state->current)->size == 0) && (((RAIDProcessState *) state->current)->paritySize == 0)) {
	((RAIDProcessState *) state->current)->diskOperationPending = false;
	((RAIDProcessState *) state->current)->numWrites++;
	((RAIDProcessState *) state->current)->size = 0;
	((RAIDProcessState *) state->current)->paritySize = -1;
#ifdef VERIFY
	strstream str;
	str << "Finished " << ((RAIDProcessState *) state->current)->numRequests << " Requests!"
	    << "\n" << '\0' << ends;
	FileData *outputLine = new FileData;
	outputLine->line = str.str();
	outputLine->length = strlen(outputLine->line) +1;
	outputLine->time = ((RAIDProcessState *) state->current)->lVT;
	outFileQ[0].insert(outputLine);
#endif
	int i;
	for (i = 0; i < MAXDISKS; i++) {
	  ((RAIDProcessState *) state->current)->diskRequests[i] = 0;
	}

	if (((RAIDProcessState *) state->current)->numRequests%5000 == 0) {
	  cout << "[" << name << "] disk operation [" 
	       << ((RAIDProcessState *) state->current)->numRequests << "] done!" << "\n";
	}
	if (((RAIDProcessState *) state->current)->numRequests < maxRequests) {
	  newMsg();
	}
      } // size and parity size is not equal to zero
      else {
#ifdef VERIFY
      strstream str;
      int i;
      str << " waiting on " << ((RAIDProcessState *) state->current)->size
	  << " requests and " << ((RAIDProcessState *) state->current)->paritySize 
	  << " parity requests!" 
	  << "\n";
      for (i = 0; i < ((RAIDProcessState *) state->current)->numDisks; i++) {
	str << i + ((RAIDProcessState *) state->current)->firstDiskId << ": " 
	    << ((RAIDProcessState *) state->current)->diskRequests[i] << "\t\t";
	if (i%4 == 0 && i !=0 ) str << "\n";
      }
      str << "\n" << '\0' << ends;
      FileData *outputLine = new FileData;
      outputLine->line = str.str();
      outputLine->length = strlen(outputLine->line) +1;
      outputLine->time = ((RAIDProcessState *) state->current)->lVT;
      outFileQ[0].insert(outputLine);
#endif
#ifdef OBJECTDEBUG
	outFile << " waiting on " << ((RAIDProcessState *) state->current)->size
		<< " requests and " << ((RAIDProcessState *) state->current)->paritySize 
		<< " parity requests!" << "\n";
#endif	  
      }
    } // if (read)
  }
}

void 
RAIDProcess::newMsg() {
  RAIDRequest* newRequest = (RAIDRequest *) new char[sizeof(RAIDRequest)];
  new (newRequest) RAIDRequest();

  int ldelay;

  newRequest->sourceId = id;
  newRequest->serverId = -1;
  newRequest->sinkId   = -1;
  newRequest->startCylinder = -1;

  DiscreteUniform stripe(0,TOTALSTRIPESIZE, ((RAIDProcessState *) state->current)->gen);
  DiscreteUniform read(0,1, ((RAIDProcessState *) state->current)->gen);
  DiscreteUniform size(1,MAXSTRIPESIZE, ((RAIDProcessState *) state->current)->gen);
  
  newRequest->startStripe  = (int) stripe();
  newRequest->startSector  = -1;
  newRequest->sizeRead     = (int) size();
  newRequest->logicalSector = -1;

  if ( newRequest->sizeRead == 0 ) {
    cerr << "\n\n\n\nHold IT! sizeRead = 0\n\n\n\n\n" <<"\n";
  }

  // If we try and read off the end of the array then move our starting
  // location, so that we can still read the same amount of data. 
  if ( (newRequest->startStripe + newRequest->sizeRead) >= TOTALSTRIPESIZE) {
    newRequest->startStripe = MAXSTRIPESIZE -  newRequest->sizeRead;
  }

  newRequest->beginningOfStrype = newRequest->startStripe;
  newRequest->tokenNumber = ((RAIDProcessState *) state->current)->numRequests + 1;
  ((RAIDProcessState *) state->current)->beginningOfStrype = newRequest->beginningOfStrype;
  ((RAIDProcessState *) state->current)->size    = newRequest->sizeRead;

  ldelay = msgDelay();
  newRequest->recvTime = getLVT() + ldelay;
      
  newRequest->dest = dest;  
      
  newRequest->sizeParity = 0;

  newRequest->parityMsg = false;

  if ( read() == 0) {
    newRequest->read = false;
    ((RAIDProcessState *) state->current)->read = false;
  } else {
    newRequest->read = true;
    ((RAIDProcessState *) state->current)->read = true;
  }

#ifdef OBJECTDEBUG
  outFile << "RAIDSource [ " << name 
	  <<  "] sending message: start stripe [ "
	  << newRequest->startStripe << " ] start sector [ "
	  << newRequest->startSector;
  if (newRequest->read == true) { 
    outFile << " ] size read [ "  << newRequest->sizeRead << " ]";
  } else {
    outFile << " ] size write [" << newRequest->sizeRead << " ]";
  }
  outFile << " at " << getLVT() << "\n";
#endif

  newRequest->size = sizeof(RAIDRequest);
  //  outFile << "request: " << *newRequest << "\n";
      
#ifdef DEBUG
  cout << "RAIDSource [" << name << "] sending message from " 
       << newRequest->sender << " to " << newRequest->dest << "\n" ;
  cout << "Message id " << newRequest->eventId << ", sendtime : " 
       << newRequest->sendTime << ", recvtime : " << newRequest->recvTime 
       << "\n" ;
#endif

  ((RAIDProcessState *) state->current)->diskOperationPending = true;
  ((RAIDProcessState *) state->current)->beginningOfStrype = newRequest->startStripe;
  ((RAIDProcessState *) state->current)->strypeSize = newRequest->sizeRead;
  ((RAIDProcessState *) state->current)->numRequests++;

  sendEvent(newRequest);

}

int 
RAIDProcess::msgDelay() {

  double ldelay;

  switch(sourcedist){
  case UNIFORM :
    {
      Uniform uniform(first, second, ((RAIDProcessState *) state->current)->gen);
      ldelay = uniform();
      break;
    }
  case NORMAL :
    {
      Normal normal(first, second, ((RAIDProcessState *) state->current)->gen);
      ldelay = normal();
      break;
    }
  case BINOMIAL :
    {
      Binomial binomial((int)first, second, ((RAIDProcessState *) state->current)->gen);
      ldelay = binomial();
      break;
    }
  case POISSON :
    {
      Poisson poisson(first, ((RAIDProcessState *) state->current)->gen);
      ldelay = poisson();
      break;
    }
  case EXPONENTIAL :
    {
      NegativeExpntl expo(first, ((RAIDProcessState *) state->current)->gen);
      ldelay = expo();
      break;
    }
  case FIXED :
    ldelay = (int)first ;
    break;
  default :
    ldelay = 0;
    cerr << "Improper Distribution for a Source Object!!!" << "\n";
    break;
  }
  return ( (int) ldelay );
     
}

void 
RAIDProcess::setDistribution( distribution_t distribution, 
			      double firstParameter, 
			      double secondParameter ) {
#ifdef DEBUG  
  cout << "--> inside setsourceDistribution " << "\n" ;
#endif   
   
  sourcedist = distribution; // global variables
  first      = firstParameter; // global variables
  second     = secondParameter;  // global variables
};

BasicState*
RAIDProcess::allocateState() {
  return new RAIDProcessState();
}

#endif
