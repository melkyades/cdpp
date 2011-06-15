#include "LogicalProcess.hh"
#include "NoTime.hh"
#include "SimulationObj.hh"
#include "BasicEvent.hh"

#ifdef MPI
extern "C" {
#include "mpi.h"
}
#endif

class TestProcess : public SimulationObj {
public:
  TestProcess(int myId) {
    name           = "TestProcess";
    iterationCount = 10000;
    id             = myId;
    executeCount   = 0;
  }

  ~TestProcess() {};

  void sendEventTo(int destination) {
    BasicEvent* tempEvent = (BasicEvent *) new char [sizeof(BasicEvent)];
    new (tempEvent) BasicEvent();
    
    tempEvent->recvTime = getLVT() + 1;
    tempEvent->dest     = destination;
    tempEvent->size     = sizeof(BasicEvent);
    
    sendEvent(tempEvent);
  }
  
  void executeProcess() {
    BasicEvent *recv = getEvent();

    if (iterationCount > 0) {
      sendEventTo((id == 0) ? 1 : 0);
      iterationCount--;
      executeCount++;
    }
  }
  
  void initialize() {
    if (id == 0) {
      sendEventTo(0);
    }
  }

  void finalize() {
    cout << "Execute count = " << executeCount << endl;
  }
  
  BasicState* allocateState() {
    return new BasicState;
  }

private:
  int iterationCount;
  int executeCount;
};

main(int argc, char *argv[]) {
  int id = 0;

#ifdef MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
#endif

  if (id == 0) {
    LogicalProcess lp(2, 1, 2);
    TestProcess tp1(0);
    
    lp.registerObject(&tp1);
    
    lp.allRegistered();
    lp.simulate();
  }
  else {
    LogicalProcess lp(2, 1, 2);
    TestProcess tp2(1);
    lp.registerObject(&tp2);
    
    lp.allRegistered();
    lp.simulate();
  }
}

// End of source code
