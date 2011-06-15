#ifndef INITOBJECT_CC
#define INITOBJECT_CC

#include "InitObject.hh" 
#include "OceanEvent.hh" 

InitObject::InitObject(int identifier  ,char *myName ):mlcg(){
  
  // sector id are same as object ids .. and it starts from 1 'coz 
  // 0 is the id of this object the init object

  name = new char[strlen(myName)+1];
  strcpy(name,myName);

  state->nameOfObject = new char[strlen(myName)+1];
  strcpy(state->nameOfObject,myName);
  id = identifier ; 

  ifstream inputFile("sharksWorld.config");
  initConfig.open("initState");
  inputFile >> noOfSectorsPerRow >> sectorWidth >>
    attackRange >> noOfFishes >> noOfSharks ;
 
  totalNoOfSectors = (noOfSectorsPerRow * noOfSectorsPerRow);

  getSector  = new Uniform(1 , totalNoOfSectors + 1, &mlcg);
  getTheta   = new Uniform(0 , 359 , &mlcg);
  setVelocity = new Uniform( 1 , sectorWidth * noOfSectorsPerRow /4 , &mlcg);
  setXLoc = new Uniform(0 , 1 , &mlcg);
  setYLoc = new Uniform(0, 1 , &mlcg);
}
  
InitObject::~InitObject(){
  delete getSector ;
  delete getTheta ;
  delete setVelocity ;
  delete setXLoc ; 
  delete setYLoc ; 
}

void
InitObject::initialize(){
    
  int sharkCount ; 
  int fishCount ;
  int creature = 0 ;
  
  for (sharkCount = 0 ; sharkCount < noOfSharks ; sharkCount ++){
    OceanEvent *nextEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
    new (nextEvent) OceanEvent();
    nextEvent->recvTime = getLVT() + 1    ; 
    nextEvent->dest = (int)getSector->operator()();
    nextEvent->eventType = 0 ; 
    nextEvent->ct = SHARK ; 
    nextEvent->velocity = getVelocity(SHARK);
    nextEvent->theta = (int)getTheta->operator()();
    nextEvent->entryPoint = -1 ; 
    nextEvent->sine = sin(nextEvent->theta*M_PI/180);
    nextEvent->size = sizeof(OceanEvent);
    if (nextEvent->sine <0 ){
      nextEvent->sine = -1 * nextEvent->sine;
    }
    nextEvent->cosine = cos(nextEvent->theta*M_PI/180);
    if (nextEvent->cosine <0 ){
      nextEvent->cosine = -1 * nextEvent->cosine;
    }
    nextEvent->sectorCreatedBy = nextEvent->dest ;
    nextEvent->creatureId = creature++ ;
    nextEvent->xCord = getXLocation(nextEvent->dest);
    nextEvent->yCord = getYLocation(nextEvent->dest);
    sendEvent(nextEvent);
    initConfig << nextEvent->ct << " " << "(" << nextEvent->xCord  << "," 
	       << nextEvent->yCord << ")" << nextEvent->creatureId << " "
               << nextEvent->velocity << " " 
               << nextEvent->theta <<endl;
  }
  for (fishCount = 0 ; fishCount < noOfFishes ; fishCount ++){
    OceanEvent *nextEvent =(OceanEvent *) new char[sizeof(OceanEvent)];
    new (nextEvent) OceanEvent();
    nextEvent->recvTime = getLVT() + 1  ; 
    nextEvent->dest = (int)getSector->operator()();
    nextEvent->eventType = 0 ; 
    nextEvent->ct = FISH ; 
    nextEvent->velocity = getVelocity(FISH);
    nextEvent->theta = (int)getTheta->operator()();
    nextEvent->entryPoint = -1 ; 
    nextEvent->sine = sin(nextEvent->theta*M_PI/180);
    nextEvent->size = sizeof(OceanEvent);
    if (nextEvent->sine < 0 ){
      nextEvent->sine = -1 * nextEvent->sine;
    }
    nextEvent->cosine =cos(nextEvent->theta*M_PI/180);
    if (nextEvent->cosine < 0 ){
      nextEvent->cosine = -1 * nextEvent->cosine;
    }
    nextEvent->sectorCreatedBy = nextEvent->dest ;
    nextEvent->creatureId = creature++ ;
    nextEvent->xCord = getXLocation(nextEvent->dest);
    nextEvent->yCord = getYLocation(nextEvent->dest);
    sendEvent(nextEvent);
    initConfig << nextEvent->ct << " " << "(" << nextEvent->xCord  << "," 
	       << nextEvent->yCord << ")" << nextEvent->creatureId << " "
               << nextEvent->velocity << " "  
                << nextEvent->theta <<endl; 
  }
  initConfig.flush();
  // sending one finish event to self 
  OceanEvent *selfEvent = (OceanEvent*) new char[sizeof(OceanEvent)];
  new (selfEvent) OceanEvent();
  selfEvent->dest = id ;
  selfEvent->recvTime = getLVT() + 10 ; ;
  selfEvent->size = sizeof(OceanEvent);
  sendEvent(selfEvent);
  
}


void
InitObject::executeProcess(){
  
  // this is to make GVT advance faster .
  OceanEvent *recvEvent = (OceanEvent *)getEvent();
  // cout << name << " " << getLVT() << ";" ; 
  if (recvEvent != NULL ){
    if (recvEvent->dest == id ){
      OceanEvent *selfEvent = (OceanEvent*) new char[sizeof(OceanEvent)];
      new (selfEvent) OceanEvent();
      selfEvent->dest = id ;
      selfEvent->recvTime = getLVT() + 100 ; ;
      selfEvent->size = sizeof(OceanEvent);
      sendEvent(selfEvent);
      
      
    }
  }
  
}
  
//   // use uniform distribution to assign the sharks and fishes to the sectors  
//   // use some funde to generate some reasonable values for velocities. 
//   // generate the sector id first and use it to find the xCord,yCord 
//   // theta is a random number between 0 - 359 (int) 
//   // creature id is a unique number (global)
//   // set eventType = 0 
  
//   int sharkCount ; 
//   int fishCount ;
//   int creature = 0 ;

//   OceanEvent *recvEvent = (OceanEvent *)getEvent() ;

//   if (recvEvent != NULL){
//     if (recvEvent->eventType == 0 && recvEvent->sectorCreatedBy == id){
//       for (sharkCount = 0 ; sharkCount < noOfSharks ; sharkCount ++){
// 	OceanEvent *nextEvent = new OceanEvent();
// 	nextEvent->recvTime = getLVT() + 1    ; 
// 	nextEvent->dest = (int)getSector->operator()();
// 	nextEvent->eventType = 0 ; 
// 	nextEvent->ct = SHARK ; 
// 	nextEvent->velocity = getVelocity(SHARK);
// 	nextEvent->theta = (int)getTheta->operator()();
// 	nextEvent->entryPoint = -1 ; 
// 	nextEvent->sine = sin(nextEvent->theta*M_PI/180);
// 	nextEvent->cosine = cos(nextEvent->theta*M_PI/180);
// 	nextEvent->sectorCreatedBy = nextEvent->dest ;
// 	nextEvent->creatureId = creature++ ;
// 	nextEvent->xCord = getXLocation(nextEvent->dest);
// 	nextEvent->yCord = getYLocation(nextEvent->dest);
// 	sendEvent(nextEvent);
// 	initConfig << *nextEvent ;
       
//       }
//       for (fishCount = 0 ; fishCount < noOfFishes ; fishCount ++){
// 	OceanEvent *nextEvent = new OceanEvent();
// 	nextEvent->recvTime = getLVT() + 1  ; 
// 	nextEvent->dest = (int)getSector->operator()();
// 	nextEvent->eventType = 0 ; 
// 	nextEvent->ct = FISH ; 
// 	nextEvent->velocity = getVelocity(FISH);
// 	nextEvent->theta = (int)getTheta->operator()();
// 	nextEvent->entryPoint = -1 ; 
// 	nextEvent->sine = sin(nextEvent->theta*M_PI/180);
// 	nextEvent->cosine = cos(nextEvent->theta*M_PI/180);
// 	nextEvent->sectorCreatedBy = nextEvent->dest ;
// 	nextEvent->creatureId = creature++ ;
// 	nextEvent->xCord = getXLocation(nextEvent->dest);
// 	nextEvent->yCord = getYLocation(nextEvent->dest);
// 	sendEvent(nextEvent);
// 	initConfig << *nextEvent ;
//       }
//       initConfig.flush();
//     }
//   }
// }


  
int
InitObject::getVelocity(creatureType ct){
  
  int velocity ;
  // the if loop is there so that some time in the future we can controll the
  // the speeds of fishes and sharlks for now thereis a disrtibution

  if (ct == FISH){
    velocity = (int)setVelocity->operator()();
  
  }
  else if (ct == SHARK){
    velocity = (int)setVelocity->operator()();
  
  }
  else {
    cout << "Unknown Creature Type" << endl ;
    velocity = 0 ;
  } 
   return velocity ;
}

int 
InitObject::getXLocation(int sectorId){
  int Xindex  ; 
  int Xcord ; 


  Xindex = (sectorId -1) % noOfSectorsPerRow  ;

 
  setXLoc->low(Xindex*sectorWidth);
  setXLoc->high((Xindex*sectorWidth) + sectorWidth);
  Xcord = (int)setXLoc->operator()();
  return Xcord ; 
}


int 
InitObject::getYLocation(int sectorId){

  int Yindex ;
  int Ycord ; 
  
  Yindex = (sectorId - 1) / noOfSectorsPerRow ;
  
  setYLoc->low(Yindex*sectorWidth);
  setYLoc->high(Yindex*sectorWidth + sectorWidth);
  Ycord = (int)setYLoc->operator()();
  return Ycord ;

}

#endif







