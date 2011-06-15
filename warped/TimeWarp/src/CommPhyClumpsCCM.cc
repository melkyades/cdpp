#include "config.hh"
#if defined(CLUMPS_ICM) && defined(CLUMPS_ICM_CCM)

#include "CommPhyClumpsCCM.hh"


CommPhyClumpsCCM::CommPhyClumpsCCM(){
}


CommPhyClumpsCCM::~CommPhyClumpsCCM(){
  
}


void
CommPhyClumpsCCM::physicalInit(int *argc, char ***argv) {

  /* Change */
  
  physicalComm.physicalInit(argc,argv) ;
  id = physicalComm.physicalGetId() ;

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

  /* Fine till here
     Do recv side init here
  */

  for(i1=0 ; i1<RECV_BUF_SIZE ; i1++) {
    recvADT[i1].empty = true ;
  }
  twNextRecvSlot = 0 ;
  nextRecvSlot  = 0 ;

  jobDoneMutex.lock();
  jobDone = false ;
  jobDoneMutex.unlock();
  
  if(pthread_create(&commThread,NULL,ClumpsCCMStart,(void *)this) != 0) {
    cerr <<"****** ERROR: ICM_CCM " << id
	 <<" Cannot create sender thread ! ******" << endl ;
    abort();
  }
}


int
CommPhyClumpsCCM::physicalGetId() const{
  /* No Change */
  
  return(physicalComm.physicalGetId()); 
}

void
CommPhyClumpsCCM::physicalSend(char *inmsg, int insize, int inid){

  /* No Change */

  bool iamDone = false ;
  if(insize > MAX_MSG_SIZE) {
    cerr <<"_CCM " << id
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
      cerr <<"_CCM " << id
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




bool
CommPhyClumpsCCM::physicalProbeRecvBuffer(char *,int ,
					  bool &){

  /* NO Change */
  cerr <<"****** ICM_CCM ERROR : physicalProbeRecvBuffer called ******\n";
  abort();
}


void
CommPhyClumpsCCM::physicalFinalize() {
  /* No Change */
  
  jobDoneMutex.lock();
  jobDone = true ;
  jobDoneMutex.unlock();


  if(pthread_join(commThread,NULL) != 0) {
    cerr <<"****** ERROR: ICM_CCM " << id
	 <<" Cannot join sender thread ! ******" << endl ;
    abort();
  }

  physicalComm.physicalFinalize() ;
  
}

int
CommPhyClumpsCCM::setSendADTSlots(int currentDest) {
  /* No Change */
  
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
CommPhyClumpsCCM::aggregateAndSend(int numMsgsToSend,int currentDest,
					int startSendSlot) {
  /* No Change */
  
  char *walker = tempAggrSendBuffer ;
  memcpy(walker,(char*)&numMsgsToSend,sizeof(int));
  walker += sizeof(int) ;

  sendMutexArr[currentDest]->lock();

  int tmpSlot = startSendSlot;
  for(int i = 0; i<numMsgsToSend ; i++,tmpSlot++) {

    if(sendADT[currentDest]->buffer[tmpSlot].empty == true) {
      cerr << "_CCM " << id
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
CommPhyClumpsCCM::CCMThreadStart(){

  /* Change */
  

  cout <<"****** CCM " << id <<" Sender Thread Started ******\n";
  
  bool iamDone = false ;
  
  int currentDest  = 0 ;
  
  
  while (iamDone == false) {

    while(retrieveNetMessages() > 0) { }
    // try to send messages
    if(currentDest != id && checkSendADT(currentDest) > 0) {
      
      // try to receive messages
      while(retrieveNetMessages() > 0) { }
    }

    ++currentDest ;
    if(currentDest == commSize) {
      currentDest = 0 ;
    }

    jobDoneMutex.lock();
    if(jobDone == true){
      finalFlush();
      iamDone = true ;
    }
    jobDoneMutex.unlock();
    
  }
}


char *
CommPhyClumpsCCM::physicalProbeRecv(){
  /* This is the TW thread.
     So I'll just poll the receive buffer and pick up
     messages if any exist.
     The CCM Thread would have already deaggregated the messages.
     
  */

  char *retVal = NULL ;
  
  recvADT[twNextRecvSlot].lok.lock();

  if(recvADT[twNextRecvSlot].empty == false) {

    retVal = new char[recvADT[twNextRecvSlot].size];
    memcpy(retVal,recvADT[twNextRecvSlot].msg,recvADT[twNextRecvSlot].size);
    recvADT[twNextRecvSlot].empty = true ;
  }
  
  recvADT[twNextRecvSlot].lok.unlock();

  if(retVal != NULL) {
    if(twNextRecvSlot == RECV_BUF_SIZE - 1) {
      twNextRecvSlot = 0 ;
    }else{
      twNextRecvSlot++;
    }
  }

  return retVal ;
}



int
CommPhyClumpsCCM::deaggregateAndInsert() {
  char *walker = tempAggrRecvBuffer ;
  int countMsgs ;
  memcpy((char*)&countMsgs,walker,sizeof(int));
  walker += sizeof(int);

  bool space = false ;
  
  for(int i=0 ; i<countMsgs ; i++) {

    recvADT[nextRecvSlot].lok.lock();
    space = recvADT[nextRecvSlot].empty ;
    recvADT[nextRecvSlot].lok.unlock();

    while(space == false){
      cerr <<"_CCM " << id
	   <<" no space in recvADT " << endl ;
      recvADT[nextRecvSlot].lok.lock();
      space = recvADT[nextRecvSlot].empty ;
      recvADT[nextRecvSlot].lok.unlock();
    }

    recvADT[nextRecvSlot].lok.lock();

    memcpy((char*)&recvADT[nextRecvSlot].size,walker,sizeof(int));
    walker += sizeof(int);
    memcpy(recvADT[nextRecvSlot].msg,walker,recvADT[nextRecvSlot].size);
    walker += recvADT[nextRecvSlot].size ;
    recvADT[nextRecvSlot].empty = false ;
    recvADT[nextRecvSlot].lok.unlock();

    if(nextRecvSlot == RECV_BUF_SIZE - 1) {
      nextRecvSlot = 0 ;
    }else{
      nextRecvSlot++ ;
    }
    
  }

  return countMsgs ;
}


int
CommPhyClumpsCCM::retrieveNetMessages(){
  /* Change */

  int numMsgsFound  = 0 ;
  bool sizeStatus ;

  if(physicalComm.physicalProbeRecvBuffer(tempAggrRecvBuffer,
					  tempAggrBufferSize,
					  sizeStatus) == true) {
    if(sizeStatus == false) {
      cerr <<"****** ICM_CCM "<< id
	   <<" ERROR : sizeStatus is false" << endl ;
      abort();
    }

    numMsgsFound = deaggregateAndInsert();

  }

  return numMsgsFound ;
}


void
CommPhyClumpsCCM::finalFlush(){

  /* No Change */
  
  for(int i = 0 ; i < commSize ; i++){

    if(i != id) {
      while(checkSendADT(i) > 0) ;

    }
  }

  
}


int
CommPhyClumpsCCM::checkSendADT(int currentDest) {

  /* No Change */
     
  int numMsgsToSend  = 0 ;
  int startSendSlot = -1 ;

  startSendSlot = *nextSendSlot[currentDest] ;
  numMsgsToSend = setSendADTSlots(currentDest);
  
  if(numMsgsToSend != 0) {
    aggregateAndSend(numMsgsToSend,currentDest,startSendSlot);
  }

  return numMsgsToSend ;
}






void *
ClumpsCCMStart(void * obj) {
  /* No Change */
  
  CommPhyClumpsCCM *newObj = (CommPhyClumpsCCM *)obj ;
  newObj->CCMThreadStart() ;
  return NULL ;
}



#endif




