#ifndef UNIX_IPC_SEMAPHORE_HH
#define UNIX_IPC_SEMAPHORE_HH
#include "config.hh"

#ifdef CLUMPS_DPC

#include <fstream>
#include "MutexInterface.hh"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/errno.h>
#include <errno.h>

class UnixIPCSemaphore : public MutexInterface {

public :
  UnixIPCSemaphore();
  ~UnixIPCSemaphore();
  int open(key_t key);
  bool lock(void);
  bool unlock(void);
  void close(void);
  int create(key_t key, int intval);
  void remove(void);
  
private :

  key_t mykey ;
  int myid ;

  int create(int intval);
  void changeSemVal(int value);
};

#endif

#endif
