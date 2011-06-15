
#include "PingObject.hh"

pingObject::pingObject(int myID, int myDest, int numBallsRHS, int numObjectsRHS)
{
	numBalls = numBallsRHS ;

	id = myID ;
	dest = myDest;
	numObjects = numObjectsRHS;
}

void
	pingObject::initialize()
{

  	//object with id 0 kick starts the simulation
	if(id == 0)
	{
		dest = 1;
		numWaiting = 0;


		while ( numBalls-- && dest < numObjects )
		{

			numWaiting++;

			//allocate space for message
			BasicEvent *newMsg = (BasicEvent*) new char[sizeof(BasicEvent)] ;
			new (newMsg) BasicEvent();

      			//set destination
			newMsg->dest = dest ;
      			//set receive time of message to be sent

			newMsg->recvTime = ((pingObjectState*)state->current)->lVT + 1 ;

			//fill up size field
			newMsg->size = sizeof(BasicEvent);

			sendEvent(newMsg);

			dest++;
		}
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

		if(id != 0 )
		{
	 	 	//we want to send another event
			BasicEvent *newMsg = (BasicEvent*) new char[sizeof(BasicEvent)] ;
			new (newMsg) BasicEvent();

			newMsg->dest = dest;
			newMsg->recvTime = ((pingObjectState*)state->current)->lVT + 1 ;
			newMsg->size = sizeof(BasicEvent);
			sendEvent(newMsg);
		} else {
		
			//cout << numBalls << endl;

			numWaiting--;

			if (numWaiting == 0 )
			{
				dest = 1;

				while ( numBalls > 0 && dest < numObjects )
				{

					numBalls--;
					numWaiting++;

					//allocate space for message
					BasicEvent *newMsg = (BasicEvent*) new char[sizeof(BasicEvent)] ;
					new (newMsg) BasicEvent();

      					//set destination
					newMsg->dest = dest ;

					//set receive time of message to be sent
					newMsg->recvTime = ((pingObjectState*)state->current)->lVT + 1 ;

					//fill up size field
					newMsg->size = sizeof(BasicEvent);

					sendEvent(newMsg);

					dest++;
				}


			}

		}
	}	  

}

void
	pingObject::finalize()
{
	cout << "Finish: " << id << endl;
}


pingObject::~pingObject()
{
}

BasicState*
pingObject::allocateState() {
	return new BasicState();
}
