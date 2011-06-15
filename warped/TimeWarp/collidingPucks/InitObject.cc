#ifndef INITOBJECT_CC
#define INITOBJECT_CC

#include "InitObject.hh"
#include "tableEvent.hh"

InitObject::InitObject(int identifier , char *myName ):mlcg(){
  name = new char[strlen(myName)+1];
  strcpy(name,myName);

  state->nameOfObject = new char[strlen(myName)+1];
  strcpy(state->nameOfObject, myName);
  id = identifier;

  ifstream inputFile("collidingPucks.config");
  initConfig.open("initState");
  inputFile >> noOfSectorsPerRow >> sectorWidth >>noOfPucks ;
  //    noOfSectorsPerRow = 2;
  //  sectorWidth = 100;
  //  noOfPucks = 10;
  totalNoOfSectors = (noOfSectorsPerRow * noOfSectorsPerRow);
  
  getSector  = new Uniform(1 , totalNoOfSectors + 1, &mlcg);
  getTheta   = new Uniform(0 , 359 , &mlcg);
  setVelocity = new Uniform( 1 , sectorWidth * noOfSectorsPerRow /4 , &mlcg);
  setXLoc = new Uniform(0 , 1 , &mlcg);
  setYLoc = new Uniform(0, 1 , &mlcg);
}

InitObject::~InitObject(){
  //delete [] name;
  delete getSector ;
  delete getTheta ;
  delete setVelocity ;
  delete setXLoc ;
  delete setYLoc ;
}

void 
InitObject::initialize(){

  int puckCount ;
  int puck = 0 ;
  for (puckCount = 0 ; puckCount < noOfPucks ; puckCount++){
    tableEvent *nextEvent = (tableEvent *)new char[sizeof(tableEvent)];
    new (nextEvent) tableEvent();
    nextEvent->recvTime = getLVT() + 1    ;
    nextEvent->dest = (int)getSector->operator()();
    nextEvent->eventType = 0 ;
    nextEvent->velocity = getVelocity();
    nextEvent->theta = (int)getTheta->operator()();
    nextEvent->entryPoint = -1 ;
    nextEvent->sine = sin(nextEvent->theta*M_PI/180);
    nextEvent->size = sizeof(tableEvent);
    if (nextEvent->sine <0 ){
      nextEvent->sine = -1 * nextEvent->sine;
    }
    nextEvent->cosine = cos(nextEvent->theta*M_PI/180);
    if (nextEvent->cosine <0 ){
      nextEvent->cosine = -1 * nextEvent->cosine;
    }
    nextEvent->sectorCreatedBy = nextEvent->dest ;
    nextEvent->puckId = puck++ ;
    nextEvent->xCord = getXLocation(nextEvent->dest);
    nextEvent->yCord = getYLocation(nextEvent->dest);
    sendEvent(nextEvent);
                     
    initConfig << nextEvent->puckId  << " " << "("
               << nextEvent->xCord  << ","<< nextEvent->yCord << ")"
               << nextEvent->velocity << " "
	       << nextEvent->theta <<endl;   
  }
  initConfig.flush();

  tableEvent *selfEvent = (tableEvent*) new char[sizeof(tableEvent)];
  new (selfEvent) tableEvent();
  selfEvent->dest = id;
  selfEvent->recvTime = getLVT() + 10 ;
  selfEvent->size = sizeof(tableEvent);
  sendEvent(selfEvent);

}


void
InitObject::executeProcess(){

  tableEvent *recvEvent = (tableEvent *)getEvent();

  if (recvEvent != NULL ){
    if (recvEvent->dest == id ){
      tableEvent *selfEvent = (tableEvent*) new char[sizeof(tableEvent)];
      new (selfEvent) tableEvent();
      selfEvent->dest = id ;
      selfEvent->recvTime = getLVT() + 100 ;
      selfEvent->size = sizeof(tableEvent) ;
      sendEvent(selfEvent);


    }
  }
}

int
InitObject::getVelocity(){
  int velocity ;
  velocity = (int)setVelocity->operator()();
  return velocity ;
}
int 
InitObject::getXLocation(int sectorId){
  int Xindex ;
  int Xcord ;

  Xindex = (sectorId - 1) % noOfSectorsPerRow ;

  setXLoc->low(Xindex*sectorWidth);
  setXLoc->high((Xindex*sectorWidth) + sectorWidth);
  Xcord = (int)setXLoc->operator()();
  return Xcord ;
}

int
InitObject::getYLocation(int sectorId){

  int Yindex ;
  int Ycord ;

  Yindex = (sectorId - 1);
 
  setYLoc->low(Yindex*sectorWidth);
  setYLoc->high(Yindex*sectorWidth + sectorWidth);
  Ycord = (int)setYLoc->operator()();
  return Ycord ;

}

#endif


   
