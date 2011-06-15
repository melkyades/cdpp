#ifndef PINGOBJECT_HH
#define PINGOBJECT_HH

#include "SimulationObj.hh"
#include "BasicEvent.hh"
#include "PingObjectState.hh"

class pingObject : public SimulationObj
{
  
  public :
  
  pingObject(int,int,int,char*);
  ~pingObject();
  
  void initialize();
  void executeProcess();
  void finalize();

  BasicState* allocateState();
  
private :
  
  int numBalls ;
  int dest ;
  int tempNumBallsRecvd ;
};

#endif
