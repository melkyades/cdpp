#ifndef STATCOLLECTOR_HH
#define STATCOLLECTOR_HH

#include "SimulationObj.hh"
#include "Types.hh"
#include <fstream.h>
#include "FileQueue.hh"

// This object collects statistics of the clients from the server. The 
// parameters that we are looking for are in the state class of this object.

class StatCollector : public SimulationObj {
private:
  int noServs;
public:
  StatCollector(int , int );
  ~StatCollector();

  void initialize();
  void executeProcess();
  void finalize();
  BasicState* allocateState();

#ifdef QUEUE_DUMP  
  FileQueue* statFile;
#endif  
};

#endif
