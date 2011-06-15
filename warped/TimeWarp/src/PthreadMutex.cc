#include "PthreadMutex.hh"

PthreadMutex::PthreadMutex(){
  if(pthread_mutex_init(&mutex,NULL) != 0) {
    cerr << "*** PthreadMutex class could not init a mutex ! ***" << endl ;
    abort();
  }

}

PthreadMutex::~PthreadMutex(){

}

bool
PthreadMutex::lock(){
  if(pthread_mutex_lock(&mutex) != 0) {
    cerr << "*** PthreadMutex class could not lock a mutex ! ***" << endl ;
    abort();
  }
  
  return true ;

}

bool
PthreadMutex::unlock(){
  if(pthread_mutex_unlock(&mutex) != 0) {
    cerr << "*** PthreadMutex class could not unlock a mutex ! ***" << endl ;
    abort();
  }

  return true ;

}


pthread_mutex_t *
PthreadMutex::giveMeMutex() {
  return &mutex ;
}

