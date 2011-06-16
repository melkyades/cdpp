#ifndef COMM_PHY_CLUMPS_ISCM_HH
#define COMM_PHY_CLUMPS_ISCM_HH

#include "config.hh"

#if defined(CLUMPS_ICM) && defined(CLUMPS_ICM_ISCM)

#include "CommPhyInterface.hh"
#include <fstream>
#include "ClumpsDefines.hh"
#include <pthread.h>

#ifdef TCPLIB
#include "CommPhyTCPMPL.hh"
#endif

#include "PthreadMutex.hh"



class CommPhyClumpsISCM :  public CommPhyInterface {

  friend void *ClumpsISCMStart(void * obj) ;
  
public :
  
  CommPhyClumpsISCM();
  ~CommPhyClumpsISCM();
  void physicalInit(int *argc, char ***argv);
  int physicalGetId() const ;
  void physicalSend(char *buffer, int size, int id);
  char * physicalProbeRecv();
  bool physicalProbeRecvBuffer(char *buffer, int size, bool& sizeStatus);
  void physicalFinalize();
 


private :

  int id ;

  struct sendRecord {
    bool empty ;
    char msg[MAX_MSG_SIZE];
    int dest ;
    bool wakeSender ;
    int size ;
  };
  sendRecord sendBuffer[SEND_BUF_SIZE];
  PthreadMutex mutexArray[SEND_BUF_SIZE];
  pthread_t sender ;
  pthread_cond_t condWakeSender ;
  int twNextSendSlot, nextSendSlot ;
  PthreadMutex jobDoneMutex ;
  bool jobDone ;
  
#ifdef TCPLIB
  CommPhyTCPMPL physicalComm ;
#endif

  void ISCMThreadStart();

};
  


#endif

#endif
