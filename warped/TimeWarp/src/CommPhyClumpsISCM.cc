#include "config.hh"

#if defined(CLUMPS_ICM) && defined(CLUMPS_ICM_ISCM)

#include "CommPhyClumpsISCM.hh"


CommPhyClumpsISCM::CommPhyClumpsISCM(){
}


CommPhyClumpsISCM::~CommPhyClumpsISCM(){
}


void
CommPhyClumpsISCM::physicalInit(int *argc, char ***argv) {
  physicalComm.physicalInit(argc,argv) ;
  id = physicalComm.physicalGetId() ;

  for(int i=0; i<SEND_BUF_SIZE ; i++) {
    sendBuffer[i].empty = true ;
    sendBuffer[i].wakeSender = false ;
  }
  twNextSendSlot = nextSendSlot = 0 ;

  jobDoneMutex.lock();
  jobDone = false ;
  jobDoneMutex.unlock();

  pthread_cond_init(&condWakeSender,NULL);

  if(pthread_create(&sender,NULL,ClumpsISCMStart,(void *)this) != 0) {
    cerr <<"****** ERROR: ICM_ISCM " << id
	 <<" Cannot create sender thread ! ******" << endl ;
    abort();
  }
}


int
CommPhyClumpsISCM::physicalGetId() const{
  return(physicalComm.physicalGetId()); 
}



void
CommPhyClumpsISCM::physicalSend(char *buffer, int size, int id){
  // do stuff here

  bool iamDone = false ;

  while(iamDone == false) {

    mutexArray[twNextSendSlot].lock();
    if(sendBuffer[twNextSendSlot].empty == true) {
      iamDone = true ;
      sendBuffer[twNextSendSlot].empty = false ;
      memcpy(sendBuffer[twNextSendSlot].msg,buffer,size);
      sendBuffer[twNextSendSlot].dest = id ;
      sendBuffer[twNextSendSlot].size = size ;
      if(sendBuffer[twNextSendSlot].wakeSender == true) {
	sendBuffer[twNextSendSlot].wakeSender = false ;
	pthread_cond_signal(&condWakeSender);
      }
    }else{
    cerr <<"****** WARNING: ICM_ISCM " << id
	 <<" reporting BUFFER FULL ! ******" << endl ;
    }
    
    mutexArray[twNextSendSlot].unlock();
  }

  if(twNextSendSlot == SEND_BUF_SIZE - 1) {
    twNextSendSlot = 0 ;
  }else{
    twNextSendSlot++ ;
  }
}

char *
CommPhyClumpsISCM::physicalProbeRecv(){
  return(physicalComm.physicalProbeRecv()) ;
}


bool
CommPhyClumpsISCM::physicalProbeRecvBuffer(char *,int,
					  bool &){
  cerr <<"****** ICM_ISCM ERROR : physicalProbeRecvBuffer called ******\n";
  abort();
}


void
CommPhyClumpsISCM::physicalFinalize() {

  jobDoneMutex.lock();
  jobDone = true ;
  jobDoneMutex.unlock();

  pthread_cond_signal(&condWakeSender);

  if(pthread_join(sender,NULL) != 0) {
    cerr <<"****** ERROR: ICM_ISCM " << id
	 <<" Cannot join sender thread ! ******" << endl ;
    abort();
  }

  physicalComm.physicalFinalize() ;
  
}


void
CommPhyClumpsISCM::ISCMThreadStart(){
  cout <<"****** ISCM " << id <<" Sender Thread Started ******\n";

  // do stuff here

  bool iamDone = false ;
  char tmpMsg[MAX_MSG_SIZE] ;
  int tmpSize,tmpDest ;
  tmpSize = tmpDest = -1 ;

  while(iamDone == false) {
    bool msgToSend = false ;
    
    mutexArray[nextSendSlot].lock();
    
    if(sendBuffer[nextSendSlot].empty == false) {
      msgToSend = true ;
    }else{
      msgToSend = false ;
    }


    if(msgToSend == false) {
      jobDoneMutex.lock();
      if(jobDone == true) {
	iamDone = true ;
      }
      jobDoneMutex.unlock();
      
      if(iamDone == false) {
	sendBuffer[nextSendSlot].wakeSender = true ;
	pthread_cond_wait(&condWakeSender,
			  mutexArray[nextSendSlot].giveMeMutex());
	// either jobDone == true or empty == false
	if(sendBuffer[nextSendSlot].empty == false) {
	  msgToSend = true ;
	}

	jobDoneMutex.lock();
	if(jobDone == true) {
	  iamDone = true ;
	}
	jobDoneMutex.unlock();
	if(msgToSend == false && iamDone == false) {
	  cerr <<"****** ERROR: ICM_ISCM " << id
	       <<" detected a problem with condition variable ! ******"
	       << endl ;

	    abort();
	}
      } // if(iamDone == false)
    } // if(msgToSend == false) 
    
    if(msgToSend == true) {
      memcpy(tmpMsg,sendBuffer[nextSendSlot].msg,
	     sendBuffer[nextSendSlot].size);
      tmpSize = sendBuffer[nextSendSlot].size ;
      tmpDest = sendBuffer[nextSendSlot].dest ;
      sendBuffer[nextSendSlot].empty = true ;
    }
    
    mutexArray[nextSendSlot].unlock();

    if(msgToSend == true) {
      if(tmpSize < 0 || tmpDest < 0) {
	cerr <<"problem with tmpSize < 0 || tmpDest < 0\n";
	abort();
      }
      physicalComm.physicalSend(tmpMsg,tmpSize,tmpDest);
      tmpSize = tmpDest = -1 ;
      if(nextSendSlot == SEND_BUF_SIZE - 1) {
	nextSendSlot = 0 ;
      }else{
	nextSendSlot++ ;
      }
    }

  }
}






void *
ClumpsISCMStart(void * obj) {
  CommPhyClumpsISCM *newObj = (CommPhyClumpsISCM *)obj ;
  newObj->ISCMThreadStart() ;
  return NULL ;
}



#endif
