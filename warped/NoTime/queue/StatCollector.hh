#ifndef STATCOLLECTOR_HH
#define STATCOLLECTOR_HH

#include "SimulationObj.hh"
#include "Types.hh"
#include <fstream>
#include "FileQueue.hh"

// This object collects statistics of the clients from the server. The 
// parameters that we are looking for are in the state class of this object.

class StatCollector : public SimulationObj {
public:
  StatCollector(int myId, int servs);
  ~StatCollector();

  void initialize();
  void executeProcess();
  void finalize();
  BasicState* allocateState();

#ifdef QUEUE_DUMP  
  FileQueue* statFile;
#endif  
private:
  int noServs;
};

#endif
