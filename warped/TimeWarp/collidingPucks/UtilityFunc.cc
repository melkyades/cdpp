#ifndef UTILITYFUNC_CC
#define UTILITYFUNC_CC

#include <iostream.h>

int getSimTime(double actualTime){
  int simTime ;

  double tmp ;

  tmp = actualTime * 10 ;


  simTime = (int) tmp ;
  if (simTime == 0 ) {
    simTime = 1 ;
  }

  if (simTime < 0 ){
    cout << " Wrong Projection " << endl ;
    simTime = (-1)*simTime ;
  }

  return simTime ;
}

#endif
