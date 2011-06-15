#include "LTSFInputQueue.hh"
#include "BasicEvent.hh"
#include "BasicState.hh"
#include "StateManager.hh"

//const MAXINPUTS = 10;
const MAXDESTINATIONS = 48;
const MESSAGESPERTIME = 3;
main() {

  LTSFInputQueue inputQ;
  //  BasicEvent* inputs[MAXINPUTS];
  BasicEvent* current;
  long long eventId = 0;
  long long useEventId = 0;
  int i, choice;
  int cancels = 0;
  int sender, dest;
  VTime time, sendTime, recvTime;
  VTime pinfinity = PINFINITY;
  VTime timeRestored;
  inputQ.initialize(MAXDESTINATIONS);
  inputQ.initialized = true;
  bool finished = false;
  bool inThePast = false;
  bool automaticEventIds = false;
  StateManager<BasicState> state[MAXDESTINATIONS];

  for (i = 0; i < MAXDESTINATIONS; i++) {
    state[i].current->lVT = 0;
    state[i].saveState();
  }

  char ans;
  cout << "Use Automatic Event Id's? (y/n)";
  cin >> ans;
  if (ans == 'y' || ans == 'Y') {
    automaticEventIds = true;
  }
  cout << "\n" << MAXDESTINATIONS << " input processes" << endl;

  while (finished != true) {
    cout << "\n"
	 << "1 - insert a message\n"
	 << "2 - process next message\n"
	 << "3 - cancel a message\n"
	 << "4 - gcollect to a time\n" 
	 << "5 - finished\n" 
	 << endl;
    cin >> choice;
    switch (choice) {
    case 1:
      cout << "Send time: ";
      cin >> sendTime;
      cout << "Recieve time: ";
      cin >> recvTime;
      cout << "Source: ";
      cin >> sender;
      cout << "Destination: ";
      cin >> dest;
      
      if (automaticEventIds == true) {
	eventId++;
      }
      else {
	cout << "Event: ";
	cin >> eventId;
      }
      cout << "\n\nSendTime: " << sendTime
	   << " RecieveTime: " << recvTime
	   << " Source: " << sender
	   << " Destination: " << dest 
	   << " EventId: " << eventId
	   << endl;
      current = new BasicEvent;
      current->sender = sender;
      current->dest = dest;
      current->sendTime = sendTime;
      current->recvTime = recvTime;
      current->eventId = eventId;
      current->size = sizeof(BasicEvent);
      inThePast = inputQ.insert(current, dest);
      if (inThePast == true) {
	cout << "\nMessage in the past\n" << endl;
	cout << "rollback[ " << dest << " ] : " << recvTime << endl;
	timeRestored = state[dest].restoreState(recvTime);
	cout << "Restoring state " << timeRestored << endl;
      }
      break;
    case 2:
      while (inputQ.get() != NULL && 
	     inputQ.get()->alreadyProcessed == true) {
	inputQ.seek(1,CURRENT);
      }
      if (inputQ.get() != NULL) {	
	dest = inputQ.get()->dest;
	current = inputQ.getAndSeek(dest,dest);
	cout << "Processing Next Message\n";
	state[dest].current->lVT = current->recvTime;
	state[dest].saveState();
      } 
      else {
	cout << "Nothing to execute\n";
      }
      break;
    case 3:
      cout << "Send time: ";
      cin >> sendTime;
      cout << "Recieve time: ";
      cin >> recvTime;
      cout << "Source: ";
      cin >> sender;
      cout << "Destination: ";
      cin >> dest;
      cout << "EventId: ";
      cin >> eventId;
      cout << "\n\nAntiMessage "
	   << "SendTime: " << sendTime
	   << " RecieveTime: " << recvTime
	   << " Source: " << sender
	   << " Destination: " << dest 
	   << " EventId: " << eventId << endl;
      current = new BasicEvent;
      current->sender = sender;
      current->dest = dest;
      current->sendTime = sendTime;
      current->recvTime = recvTime;
      current->eventId = eventId;
      current->sign = NEGATIVE;
      current->size = sizeof(BasicEvent);
      inThePast = inputQ.insert(current, dest);
      if (inThePast == true) {
	cout << "\nMessage in the past\n" << endl;
	cout << "rollback[ " << dest << " ] : " << recvTime << endl;
	timeRestored = state[dest].restoreState(recvTime);
	cout << "Restoring state " << timeRestored << endl;
      }
       break;
    case 4:
      cout << "\nGarbage collect to time :";
      cin >> time;
      inputQ.gcollect(time);
      break;
    case 5:
      finished = true;
      break;
    default:
      cout << "\nYou can't choose that you idiot!" << endl;
      if (cin.fail()) cin.get();
      cin.clear();
      break;
    }
    inputQ.print();
    cout << "CurrentObj[] is \n";
    inputQ.printCurrentObj();
    cout << "lVTArray[] is \n";
    inputQ.printlVTArray();
    if (inThePast == true) {
      inputQ.printSmallQ(dest);
      inThePast = false;
    }
  }
  while (inputQ.get() != NULL) {
    inputQ.seek(1,CURRENT);
    if (inputQ.get()->alreadyProcessed != true) {
      dest = inputQ.get()->dest;
      inputQ.getAndSeek(dest,dest);
      cout << "Processing Next Message\n";
    }
  }

  
}
