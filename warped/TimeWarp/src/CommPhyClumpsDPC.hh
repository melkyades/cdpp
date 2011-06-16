#ifndef COMM_PHY_CLUMPS_DPC_HH
#define COMM_PHY_CLUMPS_DPC_HH

#include "config.hh"

#if defined(CLUMPS_DPC)

#include "ClumpsDefines.hh"

#include <fstream>
#include <sys/time.h>
#include <unistd.h>
#include <cstdlib>

#include "UnixIPCSemaphore.hh"
#include "UnixIPCSharedMemory.hh"

#ifdef MPI
#include "CommPhyMPI.hh"
#elif defined(TCPLIB)
#include "CommPhyTCPMPL.hh"
#endif

class CommPhyClumpsDPC : public CommPhyInterface {

public :
  CommPhyClumpsDPC();
  ~CommPhyClumpsDPC();
  void physicalInit(int *argc, char ***argv);
  int physicalGetId() const ;
  void physicalSend(char *buffer, int size, int id);
  char * physicalProbeRecv();
  bool physicalProbeRecvBuffer(char *buffer, int size, bool& sizeStatus);
  void physicalFinalize();

private :

  /********  send functionality   ********/

  // next slot for send data structure
  int sendNextSlot ;

  // next slot for sem send data structure
  int sendSemNextSlot ;

  // sem array for send
  UnixIPCSemaphore sendSemArr[DPC_SEM_NUM];

  // the keys of the send semaphores
  int sendSemKeyArr[DPC_SEM_NUM];

  // a single send shared mem
  UnixIPCSharedMemory sendShm ;

  // single send shared mem's key
  int sendShmKey ;

  // send buffer data structure
  // will attach segments of sendShm in these array slots
  char *sendShmArr[DPC_SEM_NUM * DPC_SEM_MAG_FACTOR];

  // shm mem send
  void shSend(char *buffer, int size, int id);


  // init send shm
  // called by establishChannels
  void sendShmInit();

  // int send semaphores
  // called by establishChannels
  void sendSemInit();

  void removeSendChannels();

  
  
  /********  recv functionality   ********/

  // next slot for recv data structure
  int recvNextSlot ;

  // next slot for sem recv data structure
  int recvSemNextSlot ;

  // sem array for recv
  UnixIPCSemaphore recvSemArr[DPC_SEM_NUM];

  // the keys of the recv semaphores
  int recvSemKeyArr[DPC_SEM_NUM];

  // a single recv shared mem
  UnixIPCSharedMemory recvShm ;

  // single recv shared mem's key
  int recvShmKey ;

  // recv buffer data structure
  // will attach segments of recvShm in these array slots
  char *recvShmArr[DPC_SEM_NUM * DPC_SEM_MAG_FACTOR];

#ifdef CLUMPS_DPC_REG_RETR
  static int flip ;
#else
  ifstream INFILE ;
  static int pollFreq ;
  int countPolls ;
#endif

  
  // shm recv
  char *shRecv();

  // init recv shm
  // called by establishChannels
  void recvShmInit();

  // int recv semaphores
  // called by establishChannels
  void recvSemInit();

  void removeRecvChannels();

  

  /********  common functionality   ********/
  // this will update map
  // and return num readyMsgs 
  int locateOthers();

  // this will establish channels
  // and return num readyMsgs  
  int establishChannels();

  // will wait for all ready msgs
  void syncAll(int readyMsgsGot);

  // gives random value
  int getRnd();

  // init rnd func
  void rndInit();

  // lp id
  int id ;

  // commsize
  int commSize ;

  // numBuddies, lastBuddy 
  int numBuddies, lastBuddy ;

  
  enum {
    machineNameSize = 256
  };
  
  enum DPCMsgType {
    LOCATIONMSG,
    READYMSG,
    KEYMSG,
    FINAL
  };

  struct DPCMsg {
    DPCMsgType type ;
    char machine[machineNameSize];
    int keys[DPC_SEM_NUM+1]; // 1 for the key of single shared mem
    int id ; // sender id
  };
  char machine[machineNameSize];

  // the all imp map
  bool *map ;


  
#ifdef MPI
  CommPhyMPI physicalComm ;
#elif defined(TCPLIB)
  CommPhyTCPMPL physicalComm ;
#else
#error "Neither MPI nor TCP_MPL defined"
#endif

#ifdef CLUMPS_DPC_LOG
  ofstream OFILE ;
#endif


  void saveTwInitMsg( char *);
  char *twInitMsgsBuffer[DPC_MAX_TW_INIT_MSGS] ;
  int numTwInitMsgs ;
  int indexTwInitMsgsBuffer ;

  bool keyGot ;
};


#endif

#endif
