#ifndef PTHREAD_MUTEX_HH
#define PTHREAD_MUTEX_HH

#include <pthread.h>
#include <iostream.h>
#include "MutexInterface.hh"

class PthreadMutex : public MutexInterface {

public :
  PthreadMutex() ;
  ~PthreadMutex() ;

  bool lock();
  bool unlock();

  pthread_mutex_t * giveMeMutex();
  
private :
  pthread_mutex_t mutex ;

};


#endif
