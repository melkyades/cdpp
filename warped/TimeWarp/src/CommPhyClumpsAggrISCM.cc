#include "config.hh"
#if defined(CLUMPS_ICM) && defined(CLUMPS_ICM_AGGR_ISCM)

#include "CommPhyClumpsAggrISCM.hh"


CommPhyClumpsAggrISCM::CommPhyClumpsAggrISCM(){
}


CommPhyClumpsAggrISCM::~CommPhyClumpsAggrISCM(){
  
}


void
CommPhyClumpsAggrISCM::physicalInit(int *argc, char ***argv) {
  physicalComm.physicalInit(argc,argv) ;
  id = physicalComm.physicalGetId() ;

#ifdef CLUMPS_AGGR_ISCM_LOG
  char tmpfile[256];
  sprintf(tmpfile,"/home/gsharma/research/warped/TimeWarp/RAID/clumps_log.%d",
	  id);
  LOGFILE.open(tmpfile);
  if(!LOGFILE.good()){
    cerr <<"AGGR_ISCM: could not open log file " << endl ;
    abort();
  }
#endif

  commSize = physicalComm.physicalGetSize() ;
  
  sendADT = new sendBuffer*[commSize] ;
  sendMutexArr = new PthreadMutex*[commSize] ;
  nextSendSlot = new int*[commSize] ;
  twNextSendSlot = new int*[commSize] ;
  twNextSendSlotMutex = new PthreadMutex*[commSize] ;
  
  int i1, i2 ;
  for(i1=0; i1<commSize ; i1++) {
    if(i1 != id) {
      sendADT[i1] = new sendBuffer ;
      sendMutexArr[i1] = new PthreadMutex ;
      sendADT[i1]->dest = i1 ;
      
      for(i2=0; i2<SEND_BUF_SIZE; i2++) {
	sendADT[i1]->buffer[i2].empty = true ;
      }
	
      nextSendSlot[i1] = new int ;
      *nextSendSlot[i1] = 0 ;
      twNextSendSlot[i1] = new int ;
      *twNextSendSlot[i1] = 0 ;
      twNextSendSlotMutex[i1] = new PthreadMutex ;
      
    }else{
      sendADT[i1] = NULL ;
      sendMutexArr[i1] = NULL ;
      nextSendSlot[i1] = NULL ;
      twNextSendSlot[i1] = NULL ;
      twNextSendSlotMutex[i1] = NULL ;
    }
  }

  for(i1=0 ; i1<RECV_BUF_SIZE ; i1++) {
    recvADT[i1].empty = true ;
  }
  twNextRecvSlot = 0 ;
  
  jobDoneMutex.lock();
  jobDone = false ;
  jobDoneMutex.unlock();

  if(pthread_create(&sender,NULL,ClumpsAggrISCMStart,(void *)this) != 0) {
    cerr <<"****** ERROR: ICM_AggrISCM " << id
	 <<" Cannot create sender thread ! ******" << endl ;
    abort();
  }
}


int
CommPhyClumpsAggrISCM::physicalGetId() const{
  return(physicalComm.physicalGetId()); 
}



void
CommPhyClumpsAggrISCM::physicalSend(char *inmsg, int insize, int inid){

  bool iamDone = false ;
  if(insize > MAX_MSG_SIZE) {
    cerr <<"AGGR_ISCM " << id
	 <<" physicalSend got a message bigger than MAX_MSG_SIZE = "
	 << MAX_MSG_SIZE
	 <<"\nsize = " << insize
	 << endl ;
    abort();
  }
  
  twNextSendSlotMutex[inid]->lock();
  int currentSlot = *twNextSendSlot[inid] ;
  twNextSendSlotMutex[inid]->unlock();

  while(iamDone == false) {
    sendMutexArr[inid]->lock();

    if(sendADT[inid]->buffer[currentSlot].empty == true) {
      iamDone = true ;
      sendADT[inid]->buffer[currentSlot].empty = false ;
      memcpy(sendADT[inid]->buffer[currentSlot].msg,inmsg,insize);
      sendADT[inid]->buffer[currentSlot].size = insize ;
    }else{
      cerr <<"AGGR_ISCM " << id
	   <<" send buffer full ! " << endl ;
    }
    
    sendMutexArr[inid]->unlock();
    
  }

  if(currentSlot == SEND_BUF_SIZE - 1) {
    twNextSendSlotMutex[inid]->lock();
    *twNextSendSlot[inid] = 0 ;
    twNextSendSlotMutex[inid]->unlock();
  }else{
    twNextSendSlotMutex[inid]->lock();
    (*twNextSendSlot[inid])++ ;
    twNextSendSlotMutex[inid]->unlock();
  }
}

char *
CommPhyClumpsAggrISCM::physicalProbeRecv(){

  char *retVal = NULL ;
  bool sizeStatus ;
  
  if(recvADT[twNextRecvSlot].empty == false) {
    retVal = new char[recvADT[twNextRecvSlot].size];
    memcpy(retVal,recvADT[twNextRecvSlot].msg,recvADT[twNextRecvSlot].size);
    recvADT[twNextRecvSlot].empty = true ;
  }else{
    
    if(physicalComm.physicalProbeRecvBuffer(tempAggrRecvBuffer,
					    tempAggrBufferSize,
					    sizeStatus) == true) {
      if(sizeStatus == false) {
	cerr <<"****** ICM_AggrISCM "<< id
	     <<" ERROR : sizeStatus is false" << endl ;
	abort();
      }

      deaggregateAndInsert();
      
      if(recvADT[twNextRecvSlot].empty == true) {
	cerr <<"****** ICM_AggrISCM " << id 
	     <<" ERROR : problem with deaggregation\n";
	abort();
      }

      retVal = new char[recvADT[twNextRecvSlot].size];
      memcpy(retVal,recvADT[twNextRecvSlot].msg,recvADT[twNextRecvSlot].size);
      recvADT[twNextRecvSlot].empty = true ;
      
    } // if(physicalComm.physicalProbeRecvBuffer ... 
  }

  if(retVal != NULL){
    if(twNextRecvSlot == RECV_BUF_SIZE - 1) {
      twNextRecvSlot = 0 ;
    }else{
      twNextRecvSlot++;
    }
  }

  return retVal ;
}


void
CommPhyClumpsAggrISCM::deaggregateAndInsert() {

  char *walker = tempAggrRecvBuffer ;
  int countMsgs ;
  memcpy((char*)&countMsgs,walker,sizeof(int));
  walker += sizeof(int);
  int tmpSlot = twNextRecvSlot ;

  for(int i=0 ; i<countMsgs ; i++) {

    if(recvADT[tmpSlot].empty == false ){
      cerr <<"AGGR_ISCM " << id
	   <<" problem with empty in recvADT " << endl ;
      abort();
    }
    
    //    recvADT[tmpSlot].size = *((int*)walker) ;
    memcpy((char*)&recvADT[tmpSlot].size,walker,sizeof(int));
    walker += sizeof(int);
    memcpy(recvADT[tmpSlot].msg,walker,recvADT[tmpSlot].size);
    walker += recvADT[tmpSlot].size ;
    recvADT[tmpSlot].empty = false ;

    if(tmpSlot == RECV_BUF_SIZE - 1) {
      tmpSlot = 0 ;
    }else{
      tmpSlot++ ;
    }
  }

}


bool
CommPhyClumpsAggrISCM::physicalProbeRecvBuffer(char *,int ,
					  bool &){
  cerr <<"****** ICM_AggrISCM ERROR : physicalProbeRecvBuffer called ******\n";
  abort();
}


void
CommPhyClumpsAggrISCM::physicalFinalize() {

  jobDoneMutex.lock();
  jobDone = true ;
  jobDoneMutex.unlock();


  if(pthread_join(sender,NULL) != 0) {
    cerr <<"****** ERROR: ICM_AggrISCM " << id
	 <<" Cannot join sender thread ! ******" << endl ;
    abort();
  }
#ifdef CLUMPS_AGGR_ISCM_LOG
  LOGFILE.close();
#endif
  physicalComm.physicalFinalize() ;
  
}


int
CommPhyClumpsAggrISCM::setSendADTSlots(int currentDest) {

  int numMsgsToSend = 0 ;
  int currentTwNextSendSlot = -1 ;
  
  twNextSendSlotMutex[currentDest]->lock();
  currentTwNextSendSlot = *twNextSendSlot[currentDest] ;
  twNextSendSlotMutex[currentDest]->unlock();
  
  if(currentTwNextSendSlot < *nextSendSlot[currentDest]){
    numMsgsToSend = SEND_BUF_SIZE - *nextSendSlot[currentDest] ;
    if(numMsgsToSend > MAX_MSGS_IN_AGG) {
      numMsgsToSend  = MAX_MSGS_IN_AGG ;
    }
    *nextSendSlot[currentDest] += numMsgsToSend ;
    if(*nextSendSlot[currentDest] == SEND_BUF_SIZE ) {
      *nextSendSlot[currentDest] = 0 ;
    }
  }else if(currentTwNextSendSlot > *nextSendSlot[currentDest]){
    numMsgsToSend = currentTwNextSendSlot - *nextSendSlot[currentDest];
    if(numMsgsToSend > MAX_MSGS_IN_AGG) {
      numMsgsToSend  = MAX_MSGS_IN_AGG ;
    }
    *nextSendSlot[currentDest] += numMsgsToSend ;
  }else{
    numMsgsToSend = 0 ;
  }

  return numMsgsToSend ;
}


void
CommPhyClumpsAggrISCM::aggregateAndSend(int numMsgsToSend,int currentDest,
					int startSendSlot) {

  char *walker = tempAggrSendBuffer ;
  memcpy(walker,(char*)&numMsgsToSend,sizeof(int));
  walker += sizeof(int) ;

  sendMutexArr[currentDest]->lock();

  int tmpSlot = startSendSlot;
  for(int i = 0; i<numMsgsToSend ; i++,tmpSlot++) {

    if(sendADT[currentDest]->buffer[tmpSlot].empty == true) {
      cerr << "AGGR_ISCM " << id
	   << " problem with empty field of sendbuffer " << endl ;
      abort();
    }
    
    sendADT[currentDest]->buffer[tmpSlot].empty = true ;
    memcpy(walker,(char*)&sendADT[currentDest]->buffer[tmpSlot].size,
	   sizeof(int));

    walker += sizeof(int);
    memcpy(walker,sendADT[currentDest]->buffer[tmpSlot].msg,
	   sendADT[currentDest]->buffer[tmpSlot].size);
    walker += sendADT[currentDest]->buffer[tmpSlot].size ;
  }
	  
  sendMutexArr[currentDest]->unlock();
  int sendMsgSize = walker - tempAggrSendBuffer ;
  physicalComm.physicalSend(tempAggrSendBuffer, sendMsgSize,
			    currentDest);
}


void
CommPhyClumpsAggrISCM::AggrISCMThreadStart(){
  cout <<"****** AggrISCM " << id <<" Sender Thread Started ******\n";

  bool iamDone = false ;
  int currentDest  = -1 ;
  int numMsgsToSend = 0 ;
  int startSendSlot = -1 ;
  int terminateCount = commSize - 1 ;
  
  while(iamDone == false) {
    currentDest = 0 ;

    while(currentDest < commSize && iamDone == false){
      numMsgsToSend  = 0 ;
      startSendSlot = -1 ;
      
      if(currentDest != id) {
	startSendSlot = *nextSendSlot[currentDest] ;
	numMsgsToSend = setSendADTSlots(currentDest);
	if(numMsgsToSend == 0) {
	  jobDoneMutex.lock();
	  if(jobDone == true){
	    --terminateCount ;
	  }
	  jobDoneMutex.unlock();
	  if(terminateCount == 0) {
	    iamDone = true ;
	  }
	}else{
	  aggregateAndSend(numMsgsToSend,currentDest,startSendSlot);
	}

      } // if(currentDest != id) 
      
      currentDest++ ;
    } // while(currentDest < commSize)

  } // while(iamDone == false) 
  
}






void *
ClumpsAggrISCMStart(void * obj) {
  CommPhyClumpsAggrISCM *newObj = (CommPhyClumpsAggrISCM *)obj ;
  newObj->AggrISCMThreadStart() ;
  return NULL ;
}



#endif
