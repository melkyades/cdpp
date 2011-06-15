#include "config.hh"

#ifdef CLUMPS_DPC

#include "UnixIPCSharedMemory.hh"
#include <assert.h>
#include <iostream.h>

UnixIPCSharedMemory::UnixIPCSharedMemory(){
  mykey = 0 ;
  mysize = 0 ;
}

UnixIPCSharedMemory::~UnixIPCSharedMemory(){
}

void
UnixIPCSharedMemory::setSize(int newSize){
  mysize = newSize ;
}

void
UnixIPCSharedMemory::setKey(key_t newKey){
  mykey = newKey ;
}

char *
UnixIPCSharedMemory::getAddr()const{
  return myaddr ;
}

int
UnixIPCSharedMemory::create(key_t key, int size){
  mykey = key ;
  mysize = size ;

  return(create());
}


int
UnixIPCSharedMemory::open(){
  assert(mykey > 0);
  assert(mysize > 0);
  int tempid = shmget(mykey, mysize, 0);
  
  if(tempid < 0){
    cerr << "UnixIPCSharedMemory::open error in doing shmget " << endl;
  }else{
    myid = tempid ;
  }
  
  return myid ;
}


char *
UnixIPCSharedMemory::attach(){
  char *ret = NULL;

  ret = (char *)shmat(myid, NULL, 0);
  if ( ret == NULL || ret == (char *)-1 ){
    cerr << "UnixIPCSharedMemory::attach failed shmat system call " << endl;
  } else {
    myaddr = ret;
  }
  return ret;
}

bool
UnixIPCSharedMemory::detach(){
  int ret;

  ret = shmdt(myaddr);
  if ( ret == -1 ){
    cerr << "UnixIPCSharedMemory::detach shmdt failed system call" << endl;
    return false;
  } else {
    return true;
  }
}

void
UnixIPCSharedMemory::remove(){
  struct shmid_ds buf;
  int ret;

  ret = shmctl(myid, IPC_RMID, &buf);
  if ( ret == -1){
    cerr << "UnixIPCSharedMemory::removeShm error in doing shmctl with IPC_RMID" << endl;
  }
}


/*******  private functions ***********/


int
UnixIPCSharedMemory::create(){
  assert(mykey > 0);
  assert(mysize > 0);

  int tempid = shmget(mykey, mysize, IPC_CREAT | 0600);

  if(tempid < 0){
    cerr << "UnixIPCSharedMemory::create error in doing shmget " << endl;
  }else{
    myid = tempid ;
  }
  
  return myid ;
}

#endif
