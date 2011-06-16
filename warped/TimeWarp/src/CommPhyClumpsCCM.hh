#ifndef COMM_PHY_CLUMPS_CCM_HH
#define COMM_PHY_CLUMPS_CCM_HH

#include "config.hh"

#if defined(CLUMPS_ICM) && defined(CLUMPS_ICM_CCM)



#include "CommPhyInterface.hh"
#include <fstream>
#include "ClumpsDefines.hh"
#include <pthread.h>

#ifdef TCPLIB
#include "CommPhyTCPMPL.hh"
#endif

#include "PthreadMutex.hh"


class CommPhyClumpsCCM :  public CommPhyInterface {

  friend void *ClumpsCCMStart(void * obj) ;
  
public :
  
  CommPhyClumpsCCM();
  ~CommPhyClumpsCCM();
  void physicalInit(int *argc, char ***argv);
  int physicalGetId() const ;
  void physicalSend(char *buffer, int size, int id);
  char * physicalProbeRecv();
  bool physicalProbeRecvBuffer(char *buffer, int size, bool& sizeStatus);
  void physicalFinalize();
 
private :

  int id ;
  PthreadMutex jobDoneMutex ;
  bool jobDone ;
  int commSize ;

  pthread_t commThread ;

  struct msgModule {
    bool empty ;
    char msg[MAX_MSG_SIZE];
    int size ;
  };
  
  struct sendBuffer {
    int dest ;
    msgModule buffer[SEND_BUF_SIZE];
  };

  sendBuffer **sendADT;
  PthreadMutex **sendMutexArr ;

  int **twNextSendSlot ;
  PthreadMutex **twNextSendSlotMutex ;

  int **nextSendSlot ;
  
  struct aggrHeader {
    int numMsgs ;
  };

  enum {
    tempAggrBufferSize = sizeof(int)+(MAX_MSG_SIZE*MAX_MSGS_IN_AGG)
    +(sizeof(int)*MAX_MSGS_IN_AGG) 
  };

  char tempAggrSendBuffer[tempAggrBufferSize] ;


  char tempAggrRecvBuffer[tempAggrBufferSize] ;
  
  struct recvBuffer {
    char msg[MAX_MSG_SIZE];
    int size ;
    bool empty ;

    /* new++ */
    PthreadMutex lok ;
    
  };
  
  recvBuffer recvADT[RECV_BUF_SIZE] ;
  int twNextRecvSlot ;

  /* new++ */
  int nextRecvSlot ;

#ifdef TCPLIB
  CommPhyTCPMPL physicalComm ;
#endif
  
  void aggregateAndSend(int numMsgsToSend,int currentDest,int startSendSlot) ;
  int setSendADTSlots(int currentDest) ;
  int deaggregateAndInsert() ;
  
  void CCMThreadStart();


  int checkSendADT(int currentDest);
  int retrieveNetMessages();
  void finalFlush();
} ;



#endif

#endif
