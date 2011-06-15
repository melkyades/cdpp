#include "CollectStats.hh"
#include "CollectStatsState.hh"
#include "SmartPacket.hh"

CollectStats::CollectStats(int identifier) {
  id = identifier;
}

void
CollectStats::executeProcess() {
 SmartPacket *recvEvent = (SmartPacket *)getEvent();
 int numMess;
 
 if(recvEvent != NULL) {
   if(recvEvent->eventType == 0){
     ((CollectStatsState*)state->current)->noMessages++;
   }
   else {
     if(recvEvent->sinkId == recvEvent->sender) {
       //The packet has arrived at its destination
       ((CollectStatsState *)state->current)->noGotMessages++;
       numMess = ((CollectStatsState *)state->current)->noGotMessages + 1;
       ((CollectStatsState*)state->current)->avgMessLatency =                         ((CollectStatsState*)state->current)->avgMessLatency*(numMess-1)/numMess       + ((float)(getLVT()-1-recvEvent->startTime))/numMess;
        
       ((CollectStatsState*)state->current)->avgHops =                       
       ((CollectStatsState*)state->current)->avgHops*(numMess-1)/numMess
       +(float)(recvEvent->energy)/numMess;
        
     }
     else {
       //Packet discarded due to failure of Security test or
       // Lack of environment or packet running out of hops
       ((CollectStatsState*)state->current)->noLostMessages++;
     }
   }
 }
}

void
CollectStats::finalize() {
  int transPacks;
  transPacks = ((CollectStatsState*)state->current)->noMessages - 
              ((CollectStatsState*)state->current)->noGotMessages - 
              ((CollectStatsState*)state->current)->noLostMessages;

  cout << "      SIMULATION  RESULTS   \n\n";
  cout << "Total number of Smart packets = "
       << ((CollectStatsState*)state->current)->noMessages << endl;
  cout << "Number of Packets recieved at their destination = "
       << ((CollectStatsState*)state->current)->noGotMessages << endl;
  cout << "Number of Packets lost in transmission = "
       << ((CollectStatsState*)state->current)->noLostMessages << endl;
  cout << "Number of Packets still in transit at the end of Simulation = "
       << transPacks << endl; 
  cout << "Average message latency = "
       << ((CollectStatsState*)state->current)->avgMessLatency << endl;
  cout << "Average number of Hops/Packet before reaching destination = "
       << ((CollectStatsState*)state->current)->avgHops << endl;
  cout << "---------------------------------------------------" << endl;
}
  
