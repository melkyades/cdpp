
#include "PingObject.hh"

pingObject::pingObject(int myID, int myDest, int numBallsRHS,char* nameBuffer)
{
  //copy name of object to state variable
  state->nameOfObject = new char[strlen(nameBuffer)+1];
  strcpy(state->nameOfObject,nameBuffer);
  
  numBalls = numBallsRHS ;
  
  id = myID ;
  dest = myDest;
  
  tempNumBallsRecvd = numBallsRHS ;
  
}

void
pingObject::initialize()
{
  ((pingObjectState*)state->current)->numBallsRecvd = tempNumBallsRecvd ;
  ((pingObjectState*)state->current)->numBallsSent = 0 ;

  //object with id 0 kick starts the simulation
  if(id == 0)
    {
      //allocate space for message
      BasicEvent *newMsg = (BasicEvent*) new char[sizeof(BasicEvent)] ;
      new (newMsg) BasicEvent();
      
      //set destination
      newMsg->dest = id ;
      //set receive time of message to be sent
      newMsg->recvTime = ((pingObjectState*)state->current)->lVT + 1 ;
      //fill up size field
      newMsg->size = sizeof(BasicEvent);
      
      //set counts appropriately
      ((pingObjectState*)state->current)->numBallsRecvd++;
      //      ((pingObjectState*)state->current)->numBallsSent++;
      //send the event
      sendEvent(newMsg);
    }
}

void
pingObject::executeProcess()
{
  //we got an event
  //let's get the event
  BasicEvent *msgGot = getEvent();
  
  if(msgGot != NULL)
    {
      ((pingObjectState*)state->current)->numBallsRecvd-- ;
      
      if(id != 0 || (id == 0 && ((pingObjectState*)state->current)->numBallsRecvd != 0))
	{
	  //we want to send another event
	  BasicEvent *newMsg = (BasicEvent*) new char[sizeof(BasicEvent)] ;
	  new (newMsg) BasicEvent();
	  
	  newMsg->dest = dest;
	  newMsg->recvTime = ((pingObjectState*)state->current)->lVT + 1 ;
	  newMsg->size = sizeof(BasicEvent);
      
	  ((pingObjectState*)state->current)->numBallsSent++;
	  sendEvent(newMsg);
	}
    }	  
      
}

void
pingObject::finalize()
{
  //simulation is over 
  //let's see how we did
  cout << state->nameOfObject <<" received "
       <<numBalls - ((pingObjectState*)state->current)->numBallsRecvd
       << " balls and sent out "
       <<((pingObjectState*)state->current)->numBallsSent
       <<endl;
  if(((pingObjectState*)state->current)->numBallsRecvd != 0)
    {
      cout << "----------->  " 
	   <<state->nameOfObject
	   <<" FAILED TO COMPLETE SIMULATION ! " << endl;
    }

}


pingObject::~pingObject()
{
  delete [] state->nameOfObject ;
}

BasicState*
pingObject::allocateState() {
  return new pingObjectState();
}
