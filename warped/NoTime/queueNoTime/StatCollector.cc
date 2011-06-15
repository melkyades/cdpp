#include "StatCollector.hh"
#include "StatCollectorState.hh"
#include <iostream.h>
#include <stdio.h>

StatCollector::StatCollector(int myId, int servs) {
  id = myId;
  noServs = servs;

#ifdef QUEUE_DUMP
  statFile = new FileQueue[1];
  statFile[0].open("Queue.stats");
#endif  
}

StatCollector::~StatCollector() {
#ifdef QUEUE_DUMP
  delete [] statFile;
#endif
}

void
StatCollector::initialize() {
#ifdef QUEUE_DUMP
  FileData *header = new FileData;
  header->line = new char[160];
  sprintf(header->line, "SNo.     Entry Time    Queue Time    Pro. Time    ServerId   Exit Time\n"
	  "------   ----------    ----------    ---------    --------   ---------\n\n");
  header->length = strlen(header->line);
  header->time   = getLVT();
  statFile[0].insert(header);
#endif
}

void
StatCollector::executeProcess() {
  StatEvent *recvEvent;
  recvEvent = (StatEvent*) getEvent();
  int noClients;

  if (recvEvent != NULL) {
    
    noClients = ++((StatCollectorState*)state->current)->noClientsRecv;

    // calculate the average arrival rate of queue entities
    ((StatCollectorState*)state->current)->avgArrTime = ((float) noClients) / recvEvent->entryTime;
    
    // Calculate the throughput = No of jobs / Time taken
    ((StatCollectorState*)state->current)->throughPut = ((float) noClients) / recvEvent->exitTime;

    // Total Server Utilization Time
    ((StatCollectorState*)state->current)->utilTime += (recvEvent->exitTime - recvEvent->waitTime);

    ((StatCollectorState*)state->current)->avgServTime = ((StatCollectorState*)state->current)->utilTime/ ((float) noClients);

    // Calculate the average system times (time spent by an item in the queue) 
    //Calculated incrementally to avoid integer overflow
    ((StatCollectorState*)state->current)->avgSysTime = (((StatCollectorState*)state->current)->avgSysTime * (noClients-1))/noClients + ((float)(recvEvent->exitTime - recvEvent->entryTime))/noClients;
  }

#ifdef QUEUE_DUMP
  if (recvEvent != NULL) {
    FileData* statLine = new FileData;
    char buffer[128];
    sprintf(buffer, "%6d       %5d       %5d         %5d         %3d    %8d\n",
	    noClients, recvEvent->entryTime, 
	    (recvEvent->waitTime - recvEvent->entryTime),
	    (recvEvent->exitTime - recvEvent->waitTime), recvEvent->sender,
	    recvEvent->exitTime);
    statLine->length = strlen(buffer) + 1;
    statLine->line = new char[statLine->length];
    memcpy(statLine->line, buffer, statLine->length);
    statLine->time = getLVT();
    statFile[0].insert(statLine);
  }
#endif
  
}

void
StatCollector::finalize() {
  // Output the collected stats
  float Util;
  float qLength;
  // Using Little`s law
  qLength = ((StatCollectorState*)state->current)->avgArrTime * ((StatCollectorState*)state->current)->avgSysTime;
  int elapsedTime = (int)(((StatCollectorState*)state->current)->noClientsRecv/((StatCollectorState*)state->current)->throughPut);

  Util = ((float)((StatCollectorState*)state->current)->utilTime)/(elapsedTime*noServs);

  cout << "Total number of Clients serviced = " 
       << ((StatCollectorState*)state->current)->noClientsRecv << endl;
  cout << "Average arrival time of objects in Queue = "
       << ((StatCollectorState*)state->current)->avgArrTime << endl;
  cout << "Average service time of objects in Queue = "
       << ((StatCollectorState*)state->current)->avgServTime << endl;
  cout << "Average time spent by objects in Queue = "
       << ((StatCollectorState*)state->current)->avgSysTime << endl;
  cout << "Throughput = "
       << ((StatCollectorState*)state->current)->throughPut << endl;
  cout << "Utilization = " << Util << endl;
  cout << "Average queue length using Little's Law = "
       << qLength << endl;

#ifdef QUEUE_DUMP
  statFile[0].close();
#endif
  
}

BasicState*
StatCollector::allocateState() {
  return new StatCollectorState();
}
