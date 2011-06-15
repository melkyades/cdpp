#ifndef COMM_PHY_CLUMPS_AGGR_ISCM_HH
#define COMM_PHY_CLUMPS_AGGR_ISCM_HH

#include "config.hh"

#if defined(CLUMPS_ICM) && defined(CLUMPS_ICM_AGGR_ISCM)

#include "CommPhyInterface.hh"
#include <fstream.h>
#include "ClumpsDefines.hh"
#include <pthread.h>

#ifdef TCPLIB
#include "CommPhyTCPMPL.hh"
#endif

#include "PthreadMutex.hh"


class CommPhyClumpsAggrISCM :  public CommPhyInterface {

  friend void *ClumpsAggrISCMStart(void * obj) ;
  
public :
  
  CommPhyClumpsAggrISCM();
  ~CommPhyClumpsAggrISCM();
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
   pthread_t sender ;
  
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
  };
  
  recvBuffer recvADT[RECV_BUF_SIZE] ;
  int twNextRecvSlot ;


#ifdef TCPLIB
  CommPhyTCPMPL physicalComm ;
#endif
  
  void aggregateAndSend(int numMsgsToSend,int currentDest,int startSendSlot) ;
  int setSendADTSlots(int currentDest) ;
  void deaggregateAndInsert() ;
  
  void AggrISCMThreadStart();

#ifdef CLUMPS_AGGR_ISCM_LOG
  ofstream LOGFILE ;
#endif

};
  


#endif
#endif
