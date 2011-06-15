#ifndef COMM_PHY_CLUMPS_IRCM_HH
#define COMM_PHY_CLUMPS_IRCM_HH

#include "config.hh"
#if defined(CLUMPS_ICM) && defined(CLUMPS_ICM_IRCM)

#include "CommPhyInterface.hh"
#include <fstream.h>
#include "ClumpsDefines.hh"
#include <pthread.h>

#ifdef TCPLIB
#include "CommPhyTCPMPL.hh"
#endif

#include "PthreadMutex.hh"


class CommPhyClumpsIRCM :  public CommPhyInterface {

  friend void *ClumpsIRCMStart(void * obj) ;
  
public :
  
  CommPhyClumpsIRCM();
  ~CommPhyClumpsIRCM();
  void physicalInit(int *argc, char ***argv);
  int physicalGetId() const ;
  void physicalSend(char *buffer, int size, int id);
  char * physicalProbeRecv();
  bool physicalProbeRecvBuffer(char *buffer, int size, bool& sizeStatus);
  void physicalFinalize();
 

private :
  int id ;
  struct recvRecord {
    bool empty ;
    char *msg;
  };
  recvRecord recvBuffer[RECV_BUF_SIZE];
  PthreadMutex mutexArray[RECV_BUF_SIZE];
  pthread_t receiver ;
  int twNextRecvSlot, nextRecvSlot ;
  PthreadMutex jobDoneMutex ;
  bool jobDone ;
  
#ifdef TCPLIB
  CommPhyTCPMPL physicalComm ;
#endif

  void IRCMThreadStart();

};

#endif

#endif
