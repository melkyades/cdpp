#include "StatCollector.hh"
#include "StatCollState.hh"
#include "Types.hh"

StatCollector::StatCollector(int myId) {
  id = myId;
}

void
StatCollector::executeProcess() {
  MessEvent *recvEvent = (MessEvent*)getEvent();
  int numMess;

  if (recvEvent != NULL) {
    if (recvEvent->finalDest == recvEvent->sender) {
      // The packet reached its destination
      numMess = ++((StatCollState*)state->current)->noMessages;
      ((StatCollState*)state->current)->avgMessDelay = 
	((StatCollState*)state->current)->avgMessDelay*(numMess-1)/numMess + ((float)(getLVT()-1-recvEvent->startTime))/numMess;
    } else {
      // This packet has been lost due to buffer overflow at one of the 
      // routers
      ((StatCollState*)state->current)->noLostMess++;
    }
  }
}

void
StatCollector::finalize() {

  cout << " *** SIMULATION RESULTS *** \n\n";
  cout << "Total number of messages received = " 
       << ((StatCollState*)state->current)->noMessages << endl;
  cout << "Average message latency = " 
       << ((StatCollState*)state->current)->avgMessDelay << endl;  
  cout << "----------------------------------------" << endl;
  cout << "Number of messages Lost due to buffer overflow = "
       << ((StatCollState*)state->current)->noLostMess << endl;
}

BasicState*
StatCollector::allocateState() {
  return new StatCollState();
}
