#ifndef UNIX_IPC_SHARED_MEMORY_HH
#define UNIX_IPC_SHARED_MEMORY_HH
#include "config.hh"

#ifdef CLUMPS_DPC

#include <fstream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

class UnixIPCSharedMemory {

public:
  UnixIPCSharedMemory();
  ~UnixIPCSharedMemory();

  void setKey(key_t newKey);
  void setSize(int newSize);
  int create(key_t key, int size);
  int open();
  char *getAddr()const;
  char *attach();
  bool detach();
  void remove();

private :
  int create();
  int myid, mysize ;
  key_t mykey ;
  char *myaddr ;
};

#endif
#endif
