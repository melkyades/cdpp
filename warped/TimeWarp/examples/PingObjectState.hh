#ifndef PINGOBJECTSTATE_HH
#define PINGOBJECTSTATE_HH

#include "BasicState.hh"


class pingObjectState : public BasicState
{
  
  public :
  
  pingObjectState(){};
  ~pingObjectState(){};
  
  int numBallsRecvd ;
  int numBallsSent ;
  
  void copyState(BasicState * rhs) {
    *this = *((pingObjectState *) rhs);
  }

  int getSize() const {
    return sizeof(pingObjectState);
  }
  
  
  private :

};

#endif
