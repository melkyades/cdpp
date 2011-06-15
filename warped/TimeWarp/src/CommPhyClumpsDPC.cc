#include "config.hh"

#if defined(CLUMPS_DPC)

#include "CommPhyClumpsDPC.hh"

#ifdef CLUMPS_DPC_REG_RETR
int CommPhyClumpsDPC::flip = 0 ;
#else
int CommPhyClumpsDPC::pollFreq = 0 ;
#endif

CommPhyClumpsDPC::CommPhyClumpsDPC(){
}

CommPhyClumpsDPC::~CommPhyClumpsDPC(){
}


void
CommPhyClumpsDPC::physicalInit(int *argc, char ***argv){
  physicalComm.physicalInit(argc,argv);
  id = physicalComm.physicalGetId();

  keyGot = false ;
  commSize = physicalComm.physicalGetSize();

#ifdef CLUMPS_DPC_LOG
  char tmpFile[256];
  sprintf(tmpFile,"/home/gsharma/research/warped/TimeWarp/RAID/dpc.%d",id);
  OFILE.open(tmpFile);
  if(!OFILE.good()){
    cerr <<"CLUMPS_DPC " << id
	 <<" could not open dfcfile " << endl ;
    abort();
  }
#endif

#ifdef CLUMPS_DPC_INFREQ_RETR
  INFILE.open(DPC_INFREQ_RETR_FILE) ;
  if(!INFILE.good()) {
    cerr << "CommPhyClumpsDPC: Cannot open DPC_INFREQ_RETR_FILE !" << endl ;
    abort();
  }
  INFILE >> pollFreq ;
  countPolls = 0 ;
  cout <<"CommPhyClumpsDPC: polling interval is : " << pollFreq << endl ;
  
#endif

  
  map = new bool[commSize];
  for(int i=0 ; i< commSize ; i++) {
    map[i] = false ;
  }
  
  int readyMsgsGot = locateOthers();

#ifdef CLUMPS_DPC_LOG
  OFILE <<" locateOthers got " <<  readyMsgsGot << " readyMsgs . " << endl ;
#endif


  numTwInitMsgs = 0 ;
  for(int i=0; i<DPC_MAX_TW_INIT_MSGS ; i++) {
    twInitMsgsBuffer[i] = NULL ;
  }
  indexTwInitMsgsBuffer = 0 ;


  
  numBuddies = 0 ;
  lastBuddy = -1 ;
  
  for(int i=0 ; i<commSize ; i++) {
    if(map[i] == true) {
      numBuddies++ ;
      lastBuddy = i ;
    }
  }

#ifdef CLUMPS_DPC_LOG
  for(int i=0 ; i<commSize ; i++) {
    if(map[i] == true) {
      OFILE << "map[" << i << "] = " << "true" << endl ;
    }else{
      OFILE << "map[" << i << "] = " << "false" << endl ;
    }
  }
  OFILE << "numBuddies = " << numBuddies << " lastBuddy = " << lastBuddy
	<< endl ;
#endif
  
  readyMsgsGot += establishChannels();

#ifdef CLUMPS_DPC_LOG
  OFILE <<"after establishChannels  " <<  readyMsgsGot << " readyMsgs . "
	<< endl ;
#endif
  

  syncAll(readyMsgsGot);

  cout <<"\n********  Clumps DPC Strategy  **********\n";
}


int
CommPhyClumpsDPC::establishChannels(){

  sendNextSlot = recvNextSlot = 0;
  sendSemNextSlot = recvSemNextSlot = 0 ;

  rndInit();

  recvSemInit();
  recvShmInit();

  // now i'll send a message with the keys and also wait for a message
  int readyMsgsGot = 0 ;
  
  DPCMsg *toSend = new DPCMsg ;
  toSend->type = KEYMSG ;
  toSend->id = id ;
  toSend->keys[0] = recvShmKey ;
  for(int i=0 ; i<DPC_SEM_NUM ; i++) {
    toSend->keys[i+1] = recvSemKeyArr[i] ;
  }

  for(int i=0 ; i<commSize ; i++) {
    if(map[i] == true) {
      physicalComm.physicalSend((char *)toSend,sizeof(DPCMsg),i);
    }
  }

  delete [] toSend ;
  toSend = NULL ;
  
  int numKeyMsgs = numBuddies ;
  
  DPCMsg *toRecv = NULL ;
  int i1 ;
  if(keyGot == false) {
    while(numKeyMsgs > 0) {
      
      while((toRecv = (DPCMsg *)physicalComm.physicalProbeRecv()) == NULL) ;
      
      switch(toRecv->type) {
      case READYMSG :
	readyMsgsGot++ ;
	break ;
	
      case KEYMSG :
	numKeyMsgs-- ;
	// decompose message and retrieve keys
	// fill in recv key arr and recv shm key
	sendShmKey  = toRecv->keys[0] ;
	
	for(i1=0 ; i1<DPC_SEM_NUM ; i1++) {
	  sendSemKeyArr[i1] = toRecv->keys[i1+1] ;
	}
	
	sendSemInit();
	sendShmInit();
	break ;
	
      default :
	
	numTwInitMsgs++ ;
	saveTwInitMsg((char *)toRecv);
	break ;
      }
      
      delete [] toRecv ;
      toRecv = NULL ;
    }
  }
  
  return readyMsgsGot ;
}


void
CommPhyClumpsDPC::saveTwInitMsg(char *msg){
  if(numTwInitMsgs<DPC_MAX_TW_INIT_MSGS) {
    twInitMsgsBuffer[numTwInitMsgs - 1] = msg ;
  }else{
    cerr << "** ERR: twInitMsgsBuffer full !  ******" << endl ;
    abort();
  }
}


void
CommPhyClumpsDPC::sendSemInit() {
  int i;

  for( i=0 ; i<DPC_SEM_NUM ; i++) {
    sendSemArr[i].open(sendSemKeyArr[i]);
  }
}

void
CommPhyClumpsDPC::sendShmInit() {
  sendShm.setKey(sendShmKey);
  sendShm.setSize(DPC_SEM_NUM * MAX_MSG_SIZE * DPC_SEM_MAG_FACTOR);
  sendShm.open();
  sendShm.attach();

  for(int i=0; i<DPC_SEM_NUM*DPC_SEM_MAG_FACTOR ; i++) {
    sendShmArr[i] = sendShm.getAddr() + i*MAX_MSG_SIZE ;
  }
}


void
CommPhyClumpsDPC::recvSemInit() {

  int key ;
  int i;
  for(i=0 ; i<DPC_SEM_NUM ; i++) {
    key = getRnd();
    while(recvSemArr[i].open(key) > 0) key = getRnd();
    while(recvSemArr[i].create(key,DPC_SEM_INIT_VAL) <= 0) key = getRnd();

    recvSemKeyArr[i] = key ;
  }
}


void
CommPhyClumpsDPC::recvShmInit() {

  int key = getRnd();

  while(recvShm.create(key,DPC_SEM_NUM * MAX_MSG_SIZE * DPC_SEM_MAG_FACTOR)
	<= 0) {
    key = getRnd() ;
  }

  recvShmKey = key ;
  
  recvShm.attach();
  
  for(int i=0; i<DPC_SEM_NUM * DPC_SEM_MAG_FACTOR ; i++) {
    recvShmArr[i] = recvShm.getAddr() + i * MAX_MSG_SIZE ;
    *((bool *)recvShmArr[i]) = true ;
  }
  
}

void
CommPhyClumpsDPC::syncAll(int readyMsgsGot){

  DPCMsg *toSend = new DPCMsg ;
  toSend->type = READYMSG ;
  for(int i=0 ; i<commSize ; i++) {
    if(i != id) {
      physicalComm.physicalSend((char *)toSend,sizeof(DPCMsg),i);
    }
  }
  
  DPCMsg *toRecv = NULL ;
  
  while(readyMsgsGot < commSize - 1) {
#ifdef CLUMPS_DPC_LOG
    OFILE <<"WAITING on " << commSize - 1 - readyMsgsGot
	  <<" readyMsgs " << endl ;
#endif
    while((toRecv = (DPCMsg *)physicalComm.physicalProbeRecv()) == NULL) ;

    switch(toRecv->type) {
    case READYMSG :
      readyMsgsGot++ ;
      break ;
    default :
      cerr <<"DPC syncAll recvd msg with non READYMSG type\n";
      abort();
      break ;
    }
    
    delete [] toRecv ;
    toRecv = NULL ;
  } // while(readyMsgsGot < commSize - 1) 

  delete [] toSend ;

  
}

int
CommPhyClumpsDPC::locateOthers(){

  gethostname(machine,machineNameSize);

  DPCMsg *toSend = new DPCMsg ;
  toSend->type = LOCATIONMSG ;
  toSend->id = id ;
  strcpy(toSend->machine,machine);
   
  for(int i=0 ; i<commSize ; i++) {
    if(i != id) {
      physicalComm.physicalSend((char *)toSend,sizeof(DPCMsg),i);
    }
  }

  int initMsgsGot = 0, readyMsgsGot =0  ;
  DPCMsg *toRecv = NULL ;

  int i1 ;
  
  while(initMsgsGot < commSize - 1) {

    while((toRecv = (DPCMsg *)physicalComm.physicalProbeRecv()) == NULL) ;

    switch(toRecv->type) {
    case LOCATIONMSG :
      initMsgsGot++ ;
      if(strcmp(machine,toRecv->machine) == 0) {
	map[toRecv->id] = true ;
      }
      break ;
    case READYMSG :
      readyMsgsGot++ ;
      break ;
    case KEYMSG :
      cout <<"";
      keyGot = true ;
      sendShmKey  = toRecv->keys[0] ;
      
      for(i1=0 ; i1<DPC_SEM_NUM ; i1++) {
	sendSemKeyArr[i1] = toRecv->keys[i1+1] ;
      }
      
      sendSemInit();
      sendShmInit();
      break ;
      
    default :
      cerr <<"\n\nUNKNOWN MSG \n\n" ; abort();
      break ;
    }

    delete [] toRecv ;
    toRecv = NULL ;
  } // while(initMsgsGot < commSize - 1) 

  delete [] toSend ;

  return readyMsgsGot ;
}



int
CommPhyClumpsDPC::physicalGetId() const {
  return physicalComm.physicalGetId();
}



void
CommPhyClumpsDPC::physicalSend(char *inbuffer, int insize, int inid){
  //physicalComm.physicalSend(inbuffer, insize, inid);
  //return ;

  if(map[inid] == true) {
    // inid is a buddy
    shSend(inbuffer, insize, inid);
  }else{
    // inid is across the network
    physicalComm.physicalSend(inbuffer, insize, inid);
  }

}


void
CommPhyClumpsDPC::shSend(char *inbuffer, int insize, int){
  bool iamDone = false ;

  while(iamDone == false) {

    sendSemArr[sendSemNextSlot].lock();

    
    if(*((bool *)sendShmArr[sendNextSlot]) == true) {
      iamDone = true ;
      *((bool *)sendShmArr[sendNextSlot]) = false ;
      memcpy((char*)(sendShmArr[sendNextSlot]+sizeof(bool)),(char*)&insize,
	     sizeof(int)) ;
      memcpy(sendShmArr[sendNextSlot]+sizeof(bool)+sizeof(int),
	     inbuffer,insize);
    }else{
      cerr <<"Send Buffer full ! " << endl ;
    }
    
    sendSemArr[sendSemNextSlot].unlock();
  }

  if(sendNextSlot == (DPC_SEM_NUM * DPC_SEM_MAG_FACTOR) - 1 ) {
    sendNextSlot = 0 ;
  }else{
    sendNextSlot++ ;
  }

  if(sendNextSlot % DPC_SEM_MAG_FACTOR == 0) {
    if(sendSemNextSlot == DPC_SEM_NUM - 1 ) {
      sendSemNextSlot = 0 ;
    }else{
      sendSemNextSlot++ ;
    }
  }

}



#ifdef CLUMPS_DPC_REG_RETR
char *
CommPhyClumpsDPC::physicalProbeRecv(){
  //return physicalComm.physicalProbeRecv();
  
  char *retVal = NULL ;


  if(numTwInitMsgs>0){
    --numTwInitMsgs ;
    retVal = twInitMsgsBuffer[indexTwInitMsgsBuffer++];
  }else{

    if(flip == 0 ) {
      flip = 1 ;
      retVal = shRecv();
    }else{
      flip = 0 ;
      retVal = physicalComm.physicalProbeRecv();
    }
    
    if(retVal == NULL) {
      if(flip == 0 ) {
	flip = 1 ;
	retVal = shRecv();
      }else{
	flip = 0 ;
	retVal = physicalComm.physicalProbeRecv();
      }
    }


  }


  return retVal ;
}

#else
char *
CommPhyClumpsDPC::physicalProbeRecv(){
  //return physicalComm.physicalProbeRecv();
  
  char *retVal = NULL ;
  if(numTwInitMsgs>0){
    --numTwInitMsgs ;
    retVal = twInitMsgsBuffer[indexTwInitMsgsBuffer++];
  }else{

    if(countPolls == 0) {
      countPolls = pollFreq ;
      retVal = physicalComm.physicalProbeRecv();
    }else{
      --countPolls ;
      retVal = shRecv();
    }
    
  }
  return retVal ;
}

#endif

char *
CommPhyClumpsDPC::shRecv(){
  char *retVal = NULL;
  int tempSize ;
  
  recvSemArr[recvSemNextSlot].lock();
  
  if(*((bool *)recvShmArr[recvNextSlot]) == false) {

    retVal = new char[MAX_MSG_SIZE] ;

    memcpy((char*)&tempSize,(char*)(recvShmArr[recvNextSlot]+sizeof(bool)),
	   sizeof(int));

    memcpy(retVal,recvShmArr[recvNextSlot]+sizeof(bool)+sizeof(int),
	   tempSize);
    *((bool *)recvShmArr[recvNextSlot]) = true ;
  }
  
  recvSemArr[recvSemNextSlot].unlock();

  if(retVal != NULL){

    if(recvNextSlot == DPC_SEM_NUM * DPC_SEM_MAG_FACTOR - 1 ) {
      recvNextSlot = 0 ;
    }else{
      recvNextSlot++ ;
    }
    
    if(recvNextSlot % DPC_SEM_MAG_FACTOR == 0) {
      if(recvSemNextSlot == DPC_SEM_NUM - 1 ) {
	recvSemNextSlot = 0 ;
      }else{
	recvSemNextSlot++ ;
      }
    }
    
  }
  
  return retVal ;
}



void
CommPhyClumpsDPC::physicalFinalize(){
  
#ifdef CLUMPS_DPC_LOG
  OFILE.close();
#endif
  
  removeRecvChannels();

  removeSendChannels();
  
  physicalComm.physicalFinalize();
}

void
CommPhyClumpsDPC::removeSendChannels(){

  sendShm.detach();

  for(int i=0 ; i<DPC_SEM_NUM; i++) sendSemArr[i].close() ;

}

void
CommPhyClumpsDPC::removeRecvChannels(){

  recvShm.detach();
  recvShm.remove();

  for(int i=0 ; i<DPC_SEM_NUM ; i++) {
    recvSemArr[i].remove();
  }

}


void
CommPhyClumpsDPC::rndInit() {

  struct timeval timeStruct;
  gettimeofday(&timeStruct, NULL);
  srand(timeStruct.tv_usec);
  
}

int
CommPhyClumpsDPC::getRnd() {

  int temp = rand() % MAX_RND_VAL ;
  while(temp <= 0) temp = rand() % MAX_RND_VAL  ;

  return temp ;
}

bool
CommPhyClumpsDPC::physicalProbeRecvBuffer(char *,
					  int , bool& ){
  cerr <<"CLUMPS_DPC physicalProbeRecvBuffer called" << endl ;
  abort();
}

#endif
