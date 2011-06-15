#include "config.hh"

#ifdef CLUMPS_DPC

#include <stdio.h>

#include "UnixIPCSemaphore.hh"

#ifdef SUNOS
union semun {
  int val;
  struct semid_ds *buf;
  ushort *array ;
};
#endif


static struct sembuf semopstruct[1] = { 
  {0,99,0} 
} ;



UnixIPCSemaphore::UnixIPCSemaphore(){
}

UnixIPCSemaphore::~UnixIPCSemaphore(){
}

int
UnixIPCSemaphore::create(key_t newKey, int intval){
  mykey = newKey ;
  return(create(intval));
}

int
UnixIPCSemaphore::open(key_t newKey){
  mykey = newKey ;
  if ( mykey == IPC_PRIVATE ){
      return(-1);
  }
  else if ( mykey == (key_t) -1 ){
     return(-1);
  }
  int tempid ;
  if ( ( tempid = semget(mykey,1,0)) < 0 ){
    return(-1);
  }
  
  myid = tempid ;

  return(myid);
  
}

void
UnixIPCSemaphore::remove(){
  union semun semargs;
  if ( semctl(myid,0,IPC_RMID,semargs) < 0 ){
    perror("UnixIPCSemaphore::remove can't remove the sem. IPC_RMID");
  }
}

void
UnixIPCSemaphore::close(){
  
}

bool
UnixIPCSemaphore::lock(){
  changeSemVal(-1);

  return true ;
}


bool
UnixIPCSemaphore::unlock(){
  changeSemVal(1);

  return true ;
}


/*******  private functions *******/

int
UnixIPCSemaphore::create(int initval){

  int tempid ;
  
  if( mykey == IPC_PRIVATE ){
    return( -1 ) ;
  }
  else if( mykey == (key_t) -1 ){
    return( -1 ) ;
  }
  if((tempid = semget( mykey,1,0600|IPC_CREAT)) < 0 ){
    return( -1 ) ;
  }

 union semun semargs;
 semargs.val = initval ;

 
 if ( semctl(tempid,0,SETVAL,semargs) < 0 ){
   perror("UnixIPCSemaphore::create cannot SETVAL on the semaphore 0");
 }
 
 myid = tempid ;

 return(myid);
 
}


void
UnixIPCSemaphore::changeSemVal(int value){

  semopstruct[0].sem_op = value ;

  if ( semop(myid,&semopstruct[0],1) < 0 ){
    perror("UnixIPCSemaphore::changeSemVal semop error accessing semaphore ");
  }
  

}

#endif
