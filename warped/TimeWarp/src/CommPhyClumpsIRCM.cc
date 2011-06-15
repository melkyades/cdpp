#include "config.hh"

#if defined(CLUMPS_ICM) && defined(CLUMPS_ICM_IRCM)

#include "CommPhyClumpsIRCM.hh"


CommPhyClumpsIRCM::CommPhyClumpsIRCM(){
}


CommPhyClumpsIRCM::~CommPhyClumpsIRCM(){
}


void
CommPhyClumpsIRCM::physicalInit(int *argc, char ***argv) {
  physicalComm.physicalInit(argc,argv) ;
  id = physicalComm.physicalGetId() ;

  for(int i=0; i<RECV_BUF_SIZE ; i++) {
    recvBuffer[i].empty = true ;
  }
  twNextRecvSlot = nextRecvSlot = 0 ;

  jobDoneMutex.lock();
  jobDone = false ;
  jobDoneMutex.unlock();
  
  if(pthread_create(&receiver,NULL,ClumpsIRCMStart,(void *)this) != 0) {
    cerr <<"****** ERROR: ICM_IRCM " << id
	 <<" Cannot create receiver thread ! ******" << endl ;
    abort();
  }
}


int
CommPhyClumpsIRCM::physicalGetId() const{
  return(physicalComm.physicalGetId()); 
}


void
CommPhyClumpsIRCM::physicalSend(char *buffer, int size, int id){
  physicalComm.physicalSend(buffer, size, id) ;
}


char *
CommPhyClumpsIRCM::physicalProbeRecv(){
  char *msg = NULL ;
  
  mutexArray[twNextRecvSlot].lock();
  
  if(recvBuffer[twNextRecvSlot].empty == false) {
    msg = recvBuffer[twNextRecvSlot].msg ;
    recvBuffer[twNextRecvSlot].empty = true ;
  }
  
  mutexArray[twNextRecvSlot].unlock();
  
  if(msg != NULL) {
    if(twNextRecvSlot == RECV_BUF_SIZE - 1) {
      twNextRecvSlot = 0 ;
    }else{
      twNextRecvSlot++ ;
    }
  }
  
  return msg ;
}


bool
CommPhyClumpsIRCM::physicalProbeRecvBuffer(char *,int ,
					  bool &){
  cerr <<"****** ICM_IRCM ERROR : physicalProbeRecvBuffer called ******\n";
  abort();
}


void
CommPhyClumpsIRCM::physicalFinalize() {

  jobDoneMutex.lock();
  jobDone = true ;
  jobDoneMutex.unlock();
  
  if(pthread_join(receiver,NULL) != 0) {
    cerr <<"****** ERROR: ICM_IRCM " << id
	 <<" Cannot join receiver thread ! ******" << endl ;
    abort();
  }

  physicalComm.physicalFinalize() ;
  
}


void
CommPhyClumpsIRCM::IRCMThreadStart(){
  cout <<"****** IRCM " << id <<" Receiver Thread Started ******\n";

  bool iamDone = false ;
  char *newMsg = NULL ;
  
  while(iamDone == false){

    newMsg = physicalComm.physicalProbeRecv();
    
    if(newMsg != NULL) {
      bool msgInserted = false ;
      while(msgInserted == false) {
	mutexArray[nextRecvSlot].lock();
	if(recvBuffer[nextRecvSlot].empty == true) {
	  msgInserted = true ;
	  recvBuffer[nextRecvSlot].msg = newMsg ;
	  recvBuffer[nextRecvSlot].empty = false ;
	  newMsg = NULL ;
	}else{
	  cerr <<"****** ICM_IRCM " << id
	       <<" receiver thread reporting buffer full ******\n";
	}
	mutexArray[nextRecvSlot].unlock();
      }
      if(nextRecvSlot == RECV_BUF_SIZE - 1) {
	nextRecvSlot = 0 ;
      }else{
	nextRecvSlot++ ;
      }
    }else{
      jobDoneMutex.lock();
      if(jobDone == true) {
	iamDone = true ;
      }
      jobDoneMutex.unlock();
    }
    
  }
}






void *
ClumpsIRCMStart(void * obj) {
  CommPhyClumpsIRCM *newObj = (CommPhyClumpsIRCM *)obj ;
  newObj->IRCMThreadStart() ;
  return NULL ;
}



#endif








