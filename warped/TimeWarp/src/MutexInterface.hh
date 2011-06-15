#ifndef MUTEX_INTERFACE_HH
#define MUTEX_INTERFACE_HH

#include <pthread.h>
#include <iostream.h>

class MutexInterface {

public :
  virtual ~MutexInterface() {}

  virtual bool lock() =  0;
  virtual bool unlock() = 0;

protected :
  MutexInterface() {}
};

#endif
