#ifndef OCEANSECTOR_CC
#define OCEANSECTOR_CC

#include "OceanSector.hh"
int eventCount = 0;

OceanSector::OceanSector(int identifier , char *myName , int *dest){
  
  name = new char[strlen(myName)+1];
  strcpy(name,myName);

  state->nameOfObject = new char[strlen(myName)+1];
  strcpy(state->nameOfObject,myName);

  id = identifier ; 
  for (int i = 0 ; i < 8 ; i++){
    neighbourArray[i] = dest[i] ;
  }
  ifstream inputFile("sharksWorld.config");
 
  inputFile >> noOfSectorsPerRow >> sectorWidth >> attackRange ; 
  
  totalNoOfSectors = (noOfSectorsPerRow * noOfSectorsPerRow) ;

  //cout <<noOfSectorsPerRow << " " << sectorWidth << " " << attackRange << endl ;
  if(sectorWidth < attackRange ){
    cout << "attack range less than sector width" << endl ;
  }
  inputFile.close();

  // initialize the boundaries and sharks range for this sector 
  if (getXIndex() == 0 ){
    innerWestBound = (getXIndex()*sectorWidth) + attackRange ; 
    innerEastBound = (getXIndex()*sectorWidth) + sectorWidth - attackRange ; 
    outerWestBound =(noOfSectorsPerRow*sectorWidth)  - attackRange ; 
    outerEastBound = (getXIndex()*sectorWidth) + sectorWidth + attackRange ; 
  }
  else if (getXIndex() == noOfSectorsPerRow - 1 ){
    innerWestBound = (getXIndex()*sectorWidth) + attackRange ; 
    innerEastBound = (getXIndex()*sectorWidth) + sectorWidth - attackRange ; 
    outerWestBound = (getXIndex()*sectorWidth) - attackRange ; 
    outerEastBound = attackRange ;
  }
  else {
    innerWestBound = (getXIndex()*sectorWidth) + attackRange ; 
    innerEastBound = (getXIndex()*sectorWidth) + sectorWidth - attackRange ; 
    outerWestBound = (getXIndex()*sectorWidth) - attackRange ; 
    outerEastBound = (getXIndex()*sectorWidth) + sectorWidth + attackRange ; 
  }

  if (getYIndex() == 0){
    innerNorthBound = (getYIndex()*sectorWidth) + attackRange ;
    innerSouthBound = (getYIndex()*sectorWidth) + sectorWidth - attackRange ;
    outerNorthBound = noOfSectorsPerRow * sectorWidth - attackRange ; 
    outerSouthBound = (getYIndex()*sectorWidth) + sectorWidth + attackRange ;
  }
  else if (getYIndex() == noOfSectorsPerRow - 1){
    innerNorthBound = (getYIndex()*sectorWidth) + attackRange ;
    innerSouthBound = (getYIndex()*sectorWidth) + sectorWidth - attackRange ;
    outerNorthBound = (getYIndex()*sectorWidth) - attackRange ;
    outerSouthBound = attackRange ; 
  }
  else {
  innerNorthBound = (getYIndex()*sectorWidth) + attackRange ;
  innerSouthBound = (getYIndex()*sectorWidth) + sectorWidth - attackRange ;
  outerNorthBound = (getYIndex()*sectorWidth) - attackRange ;
  outerSouthBound = (getYIndex()*sectorWidth) + sectorWidth + attackRange ;
  }


  //cout << name << " :" << innerNorthBound << " " << innerSouthBound << " " << innerEastBound << " " << innerWestBound << endl; 
  //cout << outerNorthBound << " " << outerSouthBound << " " << outerEastBound << " " << outerWestBound << endl; 

}

OceanSector::~OceanSector(){
  cout << " Number of Events = " << eventCount << endl;
}

void
OceanSector::executeProcess(){
  //Keeping Count of the number of Events.
  eventCount++;
  OceanEvent *recvEvent = (OceanEvent *)getEvent() ;
  //cout << name << " " << getLVT() << ";" << endl; 
  
  if (recvEvent != NULL) {
    if (recvEvent->eventType == 0){
      // implies a arrival event 
      
      if (recvEvent->ct == SHARK ){
	OceanEvent *chkShark = isSharkInSector(recvEvent);
	if (chkShark == NULL){
	  addSharkToState(recvEvent);
	  // if the initial guy is in an overlapping secotr then transmit ARRIVAL
	  // event to neighbour 
	  addCreatureToNeighbour(recvEvent);
          checkForAttacks();
	  projectShark(recvEvent);
	  
	}
	else {
	  chkShark->xCord = recvEvent->xCord ;
	  chkShark->yCord = recvEvent->yCord ;
          checkForAttacks();
	  projectShark(recvEvent);
	  
	}
      }
      else if (recvEvent->ct == FISH){
	OceanEvent *testFish = isFishInSector(recvEvent);
	if (testFish == NULL   ){
	  // this check is there 'coz the arrival message may come after 
	   // the fish is dead
	   addFishToState(recvEvent);
	   // if the initial guy is in an overlapping secotr then
	   // transmit ARRIVAL
	   // event to neighbour 
	   addCreatureToNeighbour(recvEvent);
           checkForAttacks();
	   projectFish(recvEvent);
	   
	 }
	else {
	  testFish->xCord = recvEvent->xCord ;
	  testFish->yCord = recvEvent->yCord ;
	  checkForAttacks();
          projectFish(recvEvent);
	  
	}
       }
       else {
	 cout << "Unknown Creature Encountered" << endl ;
	 exit(-1);
       }
     }
     else if (recvEvent->eventType == 1){
       // imples that a sharks/fish os going out of range /sector 
      
        if (recvEvent->ct == SHARK ){
	 removeSharkFromState(recvEvent);

       }
       else if (recvEvent->ct == FISH){
	 removeFishFromState(recvEvent);
       }
       else {
	 cout << "Unknown Creature Encountered" << endl ;
	 exit(-1);
       }
     }
     else if (recvEvent->eventType == 2){
       // a kill event i.e basically check removes the fish involved if
       // the messages is from itself and cancels all messages sent for this
       // fish including exit-messaes
     
       if (recvEvent->ct == SHARK ){
	 cout <<" Trying to Kill a SHARK eh !!! " << endl ;
       }
       else if (recvEvent->ct == FISH){
	 projectDeadFish(recvEvent);
	 //         removeFishFromState(recvEvent);
       }
       else {
	 cout << "This is an Unkown Event Type " << name <<  endl ;
       }
       
     }
  }
}


bool 
OceanSector::isFishDead(OceanEvent *chkFish){
   OceanEvent *tmpFish ;
   bool found = false ;
   // return false if this fish is dead and in state
   tmpFish = ((OceanSectorState *)state->current)->fishList.getHead();
   while (tmpFish != NULL){
     if (tmpFish->creatureId == chkFish->creatureId &&
	 tmpFish->sectorCreatedBy == chkFish->sectorCreatedBy &&
	 tmpFish->eventType == 2){
       // fish is dead 
       found  = true ;
     }
     tmpFish = tmpFish->next ;
   }
   return found ;
 }

OceanEvent * 
OceanSector::isFishInSector(OceanEvent *chkFish){
  OceanEvent *tmpFish = NULL ;
  OceanEvent *found = NULL ;
  // return false if this fish is dead and in state
  tmpFish = ((OceanSectorState *)state->current)->fishList.getHead();
  while (tmpFish != NULL && found == NULL ){
    if (tmpFish->creatureId == chkFish->creatureId  &&
	tmpFish->sectorCreatedBy == chkFish->sectorCreatedBy ){
      found  = tmpFish;
    }
    tmpFish = tmpFish->next ;
  }
  return found ;
}


OceanEvent *
OceanSector::isSharkInSector(OceanEvent *chkShark){
  
  OceanEvent *tmpShark ;
  OceanEvent *found = NULL ;
  // return false if this fish is dead and in state
  tmpShark = ((OceanSectorState *)state->current)->sharkList.getHead();
  while (tmpShark != NULL && found == NULL ){
    if (tmpShark->creatureId == chkShark->creatureId &&
	tmpShark->sectorCreatedBy == chkShark->sectorCreatedBy ){
      found  = tmpShark;
    }
    tmpShark = tmpShark->next ;
  }
  return found ;
}




int 
OceanSector::getXIndex(){
  return  (id - 1) % noOfSectorsPerRow ;
}

int 
OceanSector::getYIndex(){
  return (id - 1) / noOfSectorsPerRow ;
}

void 
OceanSector::addSharkToState(OceanEvent *newShark){
  OceanEvent *x =(OceanEvent *) new char[sizeof(OceanEvent)];
  new (x) OceanEvent(newShark);
  ((OceanSectorState *)state->current)->sharkList.insert(x);
 }

void
OceanSector::addFishToState(OceanEvent *newFish){

  OceanEvent *x = (OceanEvent *) new char[sizeof(OceanEvent)];
  new (x) OceanEvent(newFish);
  
  ((OceanSectorState *)state->current)->fishList.insert(x);
  
 }

void
OceanSector::removeSharkFromState(OceanEvent *rmShark){
  
  OceanEvent *found = NULL  ; 
  OceanEvent *tmp = ((OceanSectorState *)state->current)->sharkList.getHead();
  
  while (tmp != NULL && found == NULL ) {
    if (tmp->ct == rmShark->ct &&
	tmp->creatureId == rmShark->creatureId  &&
        tmp->sectorCreatedBy == rmShark->sectorCreatedBy ){
      found = tmp ; 
    }
    tmp = tmp->next ;
  }
  if (found == NULL){
    //cout << " The shark to be removed was not found in state "  << name << endl ; 
  }
  else {
    ((OceanSectorState *)state->current)->sharkList.remove(found);
    //cout << "AFTER" << endl ;
  }
}

OceanEvent *
OceanSector::removeFishFromState(OceanEvent *rmFish){
  
  OceanEvent *found = NULL  ; 
  OceanEvent *tmp = ((OceanSectorState *)state->current)->fishList.getHead();
  
  while (tmp != NULL) {
    if (tmp->ct == rmFish->ct &&
	tmp->creatureId == rmFish->creatureId  &&
	tmp->sectorCreatedBy == rmFish->sectorCreatedBy ){
      found = tmp ; 
    }
    tmp = tmp->next ;
  }
  if (found == NULL){
    //cout << " The fish  to be removed was not found in state "  << name << endl ; 
  }
  else {
    ((OceanSectorState *)state->current)->fishList.remove(found);
  }
  return found ; 
}



void
OceanSector::projectFish(OceanEvent *projFish){

   
  if (projFish->theta >=0  && projFish->theta < 90){
    if ((projFish->xCord > innerNorthBound || projFish->yCord < innerEastBound)&& !( projFish->xCord < innerNorthBound && projFish->yCord > innerEastBound)){
      calculateMinDist(projFish, innerNorthBound , innerEastBound , 0 );
    }
    else {
      calculateMinDist(projFish, outerNorthBound , outerEastBound , 0 );
    }
  }
  else if (projFish->theta >=90 && projFish->theta < 180 ){
    if ((projFish->xCord < innerNorthBound || projFish->yCord > innerWestBound) &&! (projFish->xCord < innerNorthBound && projFish->yCord < innerWestBound)) {
      calculateMinDist(projFish, innerNorthBound , innerWestBound , 1 );
    }
    else {
      calculateMinDist(projFish, outerNorthBound , outerWestBound , 1 );
    }
  }
  else if (projFish->theta >=180 && projFish->theta < 270 ){
    if ((projFish->xCord < innerSouthBound || projFish->yCord > innerWestBound) && !(projFish->xCord > innerSouthBound && projFish->yCord < innerWestBound)) {
      calculateMinDist(projFish, innerSouthBound , innerWestBound , 2 );
    }
    else {
      calculateMinDist(projFish, outerSouthBound , outerWestBound , 2 );
    }
  }
  else if (projFish->theta >=270 && projFish->theta < 360 ){
    if ((projFish->xCord < innerSouthBound || projFish->yCord < innerEastBound)&&!(projFish->xCord > innerSouthBound && projFish->yCord > innerEastBound)) {
      calculateMinDist(projFish, innerSouthBound , innerEastBound , 3);
    }
    else {
       calculateMinDist(projFish, outerSouthBound , outerEastBound , 3);
    }
  }
  else {
      cout << "Invalid Angle of movement" << endl ;
  }

}

void 
OceanSector::calculateMinDist(OceanEvent *oe , int yBound , int xBound ,
			      int quadrant){
  // depending on the quatdrant send appropriate event to progess the 
  //shark/fish 
  double distToN ;
  double distToS ;
  double distToE ;
  double distToW ;
  


  OceanEvent *newEvent = NULL, *exitEvent ;
  int totalWidth = noOfSectorsPerRow * sectorWidth ;
  switch(quadrant){
  
  case 0:
   
    if (oe->yCord - yBound < 0 ){
      //      distToN = (totalWidth + (oe->yCord - yBound ))/ oe->sine ;
      distToN = tan(45*M_PI/180)*totalWidth ;
    }
    else {
      distToN  = (oe->yCord - yBound)/oe->sine ;
    }

    if (xBound - oe->xCord < 0 ){
      //distToE = (totalWidth + (xBound - oe->xCord ))/oe->cosine ;
      
      distToE =  tan(45*M_PI/180)*totalWidth ;
    }
    else {
      distToE = (xBound - oe->xCord)/oe->cosine ;
    }

   
    if (distToN <= distToE   ){
      newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (newEvent) OceanEvent(oe);
      newEvent->xCord = oe->xCord +(int)(distToN  *  oe->cosine) ; 
      if ( newEvent->xCord > totalWidth ){
	newEvent->xCord = totalWidth - newEvent->xCord ;
      }
      newEvent->yCord = yBound ; 
      newEvent->recvTime = getLVT() + getSimTime(distToN / oe->velocity) ; 
      newEvent->entryPoint  = NORTH  ; 
      newEvent->dest = neighbourArray[NORTH];
      sendEvent(newEvent);
      //cout<< *newEvent << endl ;
    }
    if (distToE <= distToN ){
      newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (newEvent) OceanEvent(oe);
      newEvent->xCord = xBound ; 
      newEvent->yCord = oe->yCord - (int)(distToE *oe->sine) ; 
      if ( newEvent->yCord < 0 ){
	newEvent->yCord = totalWidth + newEvent->yCord ;
      }
      newEvent->recvTime = getLVT() +getSimTime(distToE / oe->velocity );
      newEvent->entryPoint  = EAST  ;
      newEvent->dest = neighbourArray[EAST];

      sendEvent(newEvent);
      //cout<< *newEvent << endl ;
    }
    if (distToE == distToN )  {
      newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (newEvent) OceanEvent(oe);
      newEvent->xCord = xBound ; 
      newEvent->yCord = yBound ; 
      newEvent->recvTime = getLVT() + getSimTime(distToE/oe->velocity) ;
      newEvent->entryPoint = NE ;
      newEvent->dest = neighbourArray[NE];

      sendEvent(newEvent);
      //cout<< *newEvent << endl ;
    }
  
    if (oe->ct == FISH ){
      // i.e the fish exit this sector when it crosses this boundary
      if (yBound == outerNorthBound || xBound == outerEastBound ){
	exitEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
	new (exitEvent) OceanEvent(newEvent);
	exitEvent->eventType = 1 ;
	exitEvent->dest = id ;
	// warped cannnot have zero delay messages
	exitEvent->recvTime = newEvent->recvTime  ;
	sendEvent(exitEvent);
	//cout<< *exitEvent << endl ;
      }
    }
    else if (oe->ct == SHARK) {
      // i.e shark exits if it crosses the boundary of the sector 
      if (yBound == (getYIndex() *sectorWidth) ||
	  xBound == (getXIndex()*sectorWidth + sectorWidth)){
	exitEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
	new (exitEvent) OceanEvent(newEvent);
	exitEvent->eventType =1 ;
	exitEvent->dest = id ; 
	exitEvent->recvTime = newEvent->recvTime ;

	sendEvent(exitEvent);
	//cout<< *exitEvent << endl ;
      }
    }
    else {
      cout<< "Unknown Creature Encountered !! The LochNess eh " << endl;
    }
   
    break;
  
  case 1:
   
    if (oe->yCord - yBound < 0 ){
      //distToN = (totalWidth + (oe->yCord - yBound ))/ oe->sine ;
      
      distToN =  tan(45*M_PI/180)*totalWidth ;
    }
    else {
      distToN  = (oe->yCord - yBound)/oe->sine ;
    }

    if (oe->xCord - xBound < 0 ){
      //distToW = (totalWidth +  (oe->xCord - xBound ))/( oe->cosine) ;
      
      distToW =  tan(45*M_PI/180)*totalWidth  ;
    }
    else{ 

      distToW = (oe->xCord - xBound )/( oe->cosine) ;
      
    }

    //cout<< "While in 1 Dn: " << distToN << " Dw "  << distToW << endl;
    if (distToN < distToW ){
      newEvent = (OceanEvent *) new char[sizeof(OceanEvent)]; 
      new (newEvent) OceanEvent(oe);
      newEvent->xCord = oe->xCord - (int)(distToN *( oe->cosine)) ; 
      if (newEvent->xCord < 0){
	newEvent->xCord = totalWidth + newEvent->xCord ; 
      }
      newEvent->yCord = yBound ; 
      newEvent->recvTime = getLVT() +getSimTime(distToN / oe->velocity) ; 
      newEvent->entryPoint = NORTH  ; 
      newEvent->dest = neighbourArray[NORTH];

      sendEvent(newEvent);
      //cout<< *newEvent << endl ;
    }
    if (distToW < distToN ){
      newEvent = (OceanEvent *) new  char[sizeof(OceanEvent)];
      new (newEvent) OceanEvent(oe);
      newEvent->xCord = xBound ; 
      newEvent->yCord = oe->yCord - (int)(distToW * oe->sine) ; 
      if (newEvent->yCord < 0){
	newEvent->yCord = totalWidth + newEvent->yCord ;
      }
      newEvent->recvTime = getLVT() + getSimTime(distToW / oe->velocity) ;
      newEvent->entryPoint = WEST  ;
      newEvent->dest = neighbourArray[WEST];

      sendEvent(newEvent);

      //cout<< *newEvent << endl ;
    }
    if (distToW == distToN){
      newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (newEvent) OceanEvent(oe);
      newEvent->xCord = xBound ; 
      newEvent->yCord = yBound ; 
      newEvent->recvTime = getLVT() + getSimTime(distToW/oe->velocity) ;
      newEvent->entryPoint = NW ;
      newEvent->dest = neighbourArray[NW];

      sendEvent(newEvent);
      //cout<< *newEvent << endl ; 
    }
    
    if (oe->ct == FISH ){
      // i.e the fish exit this sector when it crosses this boundary
      if (yBound == outerNorthBound || xBound == outerWestBound ){
        exitEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
	new (exitEvent) OceanEvent(newEvent);
	exitEvent->eventType = 1 ;
	exitEvent->dest = id ;
	exitEvent->recvTime = newEvent->recvTime ;
	sendEvent(exitEvent);
	//cout<< *exitEvent << endl ; 
      }
    }
    else if (oe->ct == SHARK) {
      // i.e shark exits if it crosses the boundary of the sector 
      if (yBound == (getYIndex() *sectorWidth) ||
	  xBound == (getXIndex()*sectorWidth)){
	exitEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
	new (exitEvent) OceanEvent(newEvent);
	exitEvent->eventType = 1 ;
	exitEvent->dest = id ; 
	exitEvent->recvTime = newEvent->recvTime ;

	sendEvent(exitEvent);
	//cout<< *exitEvent << endl ;
      }
    }
    else {
      cout << "Unknown Creature Encountered !! The LochNess eh " << endl;
    }
   
    break;
 
  case 2:
   
    if (yBound - oe->yCord < 0 ){
      //distToS = (totalWidth + ( yBound - oe->yCord ))/( oe->sine) ;
      
      distToS =  tan(45*M_PI/180)*totalWidth ;
    }
    else {
      distToS = ( yBound - oe->yCord )/( oe->sine) ; 
    }
    
    if (oe->xCord - xBound < 0){
      //distToW = (totalWidth + (oe->xCord - xBound ))/( oe->cosine) ; 
     
      distToW =  tan(45*M_PI/180)*totalWidth  ;
    }
    else {
        distToW =  (oe->xCord - xBound )/( oe->cosine) ; 
    }
    //cout<< "While in 2 Ds:" << distToS <<" Dw " << distToW << endl; 
    if (distToS < distToW ){
      newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (newEvent) OceanEvent(oe);
      newEvent->xCord = oe->xCord - (int)(distToS * ( oe->cosine ));
      if (newEvent->xCord < 0){
	newEvent->xCord = totalWidth - newEvent->xCord ;
      }
      newEvent->yCord = yBound ;  
      newEvent->recvTime = getLVT() + getSimTime(distToS / oe->velocity) ; 
      newEvent->entryPoint = SOUTH  ; 
      newEvent->dest = neighbourArray[SOUTH];

      sendEvent(newEvent);
      //cout<< *newEvent << endl ;
    }
    if (distToW < distToS ){
      newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (newEvent) OceanEvent(oe);
      newEvent->xCord = xBound ; 
      newEvent->yCord = oe->yCord + (int)(distToW *( oe->sine)) ;
      if (newEvent->yCord > totalWidth  ){
	newEvent->yCord = totalWidth - newEvent->yCord ;
      }
      newEvent->recvTime = getLVT() + getSimTime(distToW / oe->velocity) ;
      newEvent->entryPoint = WEST  ;
      newEvent->dest = neighbourArray[WEST];

      sendEvent(newEvent);
      //cout<< *newEvent << endl ;
    }
    if (distToW == distToS ) {
      newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (newEvent) OceanEvent(oe);
      newEvent->xCord = xBound ; 
      newEvent->yCord = yBound ; 
      newEvent->recvTime = getLVT() + getSimTime(distToW/oe->velocity) ;
      newEvent->entryPoint = SW ;
      newEvent->dest = neighbourArray[SW];

      sendEvent(newEvent);
      //cout<< *newEvent << endl ;
    }
    if (oe->ct == FISH ){
      // i.e the fish exit this sector when it crosses this boundary
      if (yBound == outerSouthBound || xBound == outerWestBound ){
        exitEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
	new (exitEvent) OceanEvent(newEvent);
	exitEvent->eventType = 1 ;
	exitEvent->dest = id ;
	exitEvent->recvTime = newEvent->recvTime ;
	sendEvent(exitEvent);
	//cout<< *exitEvent << endl ;
      }
    }
    else if (oe->ct == SHARK) {
      // i.e shark exits if it crosses the boundary of the sector 
      if (yBound == (getYIndex() *sectorWidth + sectorWidth) ||
	  xBound == (getXIndex()*sectorWidth )){
	exitEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
	new (exitEvent) OceanEvent(newEvent);
	exitEvent->eventType =1 ;
	exitEvent->dest = id ;
	exitEvent->recvTime = newEvent->recvTime ;

	sendEvent(exitEvent);
	//cout<< *exitEvent << endl; 
      }
    }
    else
      {
	cout << "Unknown Creature Encountered !! The LochNess eh " << endl;
      }
  
    break;
  
  case 3:
  
    if ( yBound - oe->yCord < 0 ){
      //distToS = (totalWidth + (yBound - oe->yCord ))/( oe->sine) ; 
      
      distToS =  tan(45*M_PI/180)*totalWidth ;
    }
    else {
      distToS = (yBound - oe->yCord )/( oe->sine) ; 
    }
    if (xBound - oe->xCord < 0 ){
      //distToE = (totalWidth + (xBound - oe->xCord  ))/( oe->cosine) ; 
     
      distToE =  tan(45*M_PI/180)*totalWidth  ;
    }
    else {
      distToE = (xBound - oe->xCord  )/( oe->cosine) ; 
    }

    //cout<< "While in 3 Ds:" << distToS << " De " << distToE << endl;
    if (distToS < distToE ){
      newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (newEvent) OceanEvent(oe);
      newEvent->xCord = oe->xCord + (int)(distToS * ( oe->cosine ));
      if (newEvent->xCord > totalWidth ){
	newEvent->xCord =  newEvent->xCord - totalWidth  ;
      }
      newEvent->yCord = yBound ;  
      newEvent->recvTime = getLVT() + getSimTime(distToS / oe->velocity) ; 
      newEvent->entryPoint = SOUTH  ; 
      newEvent->dest = neighbourArray[SOUTH];
      sendEvent(newEvent);
      
      //      cout<< "THIS SHOT O" << *newEvent << endl ;

    }
    if (distToE < distToS ){
      newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (newEvent) OceanEvent(oe); 
      newEvent->xCord = xBound ; 
      newEvent->yCord = oe->yCord + (int)(distToE* ( oe->sine)) ; 
      if (newEvent->yCord > totalWidth ){
	newEvent->yCord = newEvent->yCord - totalWidth;
      }
      newEvent->recvTime = getLVT() + getSimTime(distToE / oe->velocity) ;
      newEvent->entryPoint = EAST  ;
      newEvent->dest = neighbourArray[EAST];

      sendEvent(newEvent);

      //      cout<< "THIS SHOT O " << *newEvent << endl ;
      }
    if (distToE == distToS ){
      newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (newEvent) OceanEvent(oe);
      newEvent->xCord = xBound ; 
      newEvent->yCord = yBound ; 
      newEvent->recvTime = getLVT() + getSimTime(distToE/oe->velocity) ;
      newEvent->entryPoint = SE ;
      newEvent->dest = neighbourArray[SE];

      sendEvent(newEvent);

      //      cout<< " THIS SHOT O " << *newEvent << endl ;
    }
    if (oe->ct == FISH ){
      // i.e the fish exit this sector when it crosses this boundary
      if (yBound == outerSouthBound || xBound == outerEastBound ){
	exitEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
	//cout<< " THIS SHOT O " << *newEvent << endl ;
	new (exitEvent) OceanEvent(newEvent);
	exitEvent->eventType = 1 ;
	exitEvent->dest = id ;
	exitEvent->recvTime = newEvent->recvTime ;

	sendEvent(exitEvent);
	//cout<< *exitEvent << endl ;
      }
    }
    else if (oe->ct == SHARK) {
      // i.e shark exits if it crosses the boundary of the sector 
      if (yBound == (getYIndex() *sectorWidth + sectorWidth) ||
	  xBound == (getXIndex()*sectorWidth + sectorWidth)){
        exitEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
	//cout<< " THIS SHOT O " << *newEvent << endl ;
	new (exitEvent) OceanEvent(newEvent);
	exitEvent->eventType = 1 ;
	exitEvent->dest = id ; 
	exitEvent->recvTime = newEvent->recvTime ;
	sendEvent(exitEvent);
	//cout << *exitEvent << endl ;
      }
    }
    else {
      cout << "Unknown Creature Encountered !! The LochNess eh " << endl;
    }
  
    break;
  
  default:
  cout << "Unknown Quadrant" << endl ;
  
  };

  //cout << "THE ORIGINAL " << *oe << endl ;
  //cout << "THE PROJECTED " << *newEvent << endl ;
  //cout << "THE EXIT " << *exitEvent << endl ;
}  
  
    
void
OceanSector::projectShark(OceanEvent *projShark){
  
  if (projShark->theta >=0  && projShark->theta < 90){
    calculateMinDist(projShark, getYIndex() *sectorWidth , (getXIndex()*sectorWidth + sectorWidth), 0 );
  }
  else if (projShark->theta >=90 && projShark->theta < 180 ){
    calculateMinDist(projShark, getYIndex()*sectorWidth , getXIndex()*sectorWidth , 1 );
  }
  else if (projShark->theta >=180 && projShark->theta < 270 ){
      calculateMinDist(projShark, (getYIndex()*sectorWidth + sectorWidth), getXIndex()*sectorWidth , 2 );
    }
    else if (projShark->theta >=270 && projShark->theta < 360 ){
      calculateMinDist(projShark, (getYIndex()*sectorWidth + sectorWidth),(getXIndex()*sectorWidth + sectorWidth), 3);
    }
    else {
      cout << "Invalid Angle of movement" << endl ;
    }
  

}


void 
OceanSector::checkForAttacks(){
  
  OceanEvent *tmpShark ; 
  OceanEvent *tmpFish ;
  
  double  a , b , c ;
  double t1 , t2 ;
  int countFish =0;
  int countShark = 0 ;
  //cout << " checking for attacks " << endl;
  // take a shark and check with every fish in state to cal the following
  //equation 
  
  tmpShark = ((OceanSectorState *)state->current)->sharkList.getHead();
  tmpFish = ((OceanSectorState *)state->current)->fishList.getHead();

  while (tmpShark != NULL){
    countShark++ ;
    while (tmpFish != NULL && tmpFish->eventType != 2){
      countFish++ ;
      a = sqr(tmpShark->velocity * cos(tmpShark->theta) - 
	      tmpFish->velocity * cos(tmpFish->theta)) + 
	sqr(tmpShark->velocity * sin(tmpShark->theta) - 
	    tmpFish->velocity * sin(tmpFish->theta)) ;
     
      b = 2 *(tmpShark->velocity * cos(tmpShark->theta) -
	      tmpFish->velocity * cos(tmpFish->theta ))*
	(tmpShark->xCord - tmpFish->xCord) +
	2*(tmpShark->velocity * sin(tmpShark->theta) - 
	   tmpFish->velocity * sin(tmpFish->theta ))*
	(tmpShark->yCord - tmpFish->yCord);

      c = sqr((long int )(tmpShark->xCord - tmpFish->xCord)) +
	sqr((long int) (tmpShark->yCord - tmpFish->yCord) ) - sqr((long int)attackRange);
      

      //cout << "a:" << a << " b:" << b << " c:" << c << endl;

      t1 = (-1 * b + sqrt(abs(sqr(b) - 4 * a * c)))/(2 * a) ; 
      t2 =  (-1 * b - sqrt(abs(sqr(b) - 4 * a * c)))/(2 * a) ; 

      //   cout << " Soln1: " << t1 << " Soln2: " <<  t2 << endl ;
      
      if (t1  > 0  && t2 > 0 ){ 
	// cout << " Soln1: " << t1 << " Soln2: " <<  t2 << endl ;
	if ( t1 < t2 ){
	
	  tmpFish->recvTime = getLVT() + getSimTime(t1);
	  generateKillEvent(tmpFish ) ;
	}
	else { 
	
	  tmpFish->recvTime = getLVT() + getSimTime(t2);
	  generateKillEvent(tmpFish );
	}
      }
      else if ( t1 > 0 && t2 <= 0 ){
	
	
	tmpFish->recvTime = getLVT() + getSimTime(t1);
	generateKillEvent(tmpFish  ); 
      }
      else if  (t2 > 0 && t1 <= 0 ){
	
	
	tmpFish->recvTime = getLVT() + getSimTime(t2);
	generateKillEvent(tmpFish);
      }

      tmpFish = tmpFish->next ;
    }
    
    tmpShark = tmpShark->next ;
  }
  // if (countShark != 0 ){
//     int noFish = countFish/countShark ; 
//     cout <<" sharks:" << countShark << " fish:" << noFish  << endl ;
//   }
}
void
OceanSector::generateKillEvent(OceanEvent* killFish ){
  // send a kill event to self
  // generate a cancel fish event to  sectors  which view this fish 
  // basically i am not going to project this fish again i send a cancel event   //to possible destination 
  // kill event type == 2 
  int i ; 
  //  cout << "SHARK KILLED FISH " << endl ;
  //cout << " sending self kill" <<  *killFish << endl;
  OceanEvent *killEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
  new (killEvent) OceanEvent(killFish);
  killEvent->recvTime = killFish->recvTime;
  killEvent->dest = id ;
  killEvent->eventType = 2 ;
  sendEvent(killEvent) ; 
  //cout << *killEvent << endl ;
  
  // now send message to inform neghbours 
  //cout << "to the neighbours " << endl ;
  if (killFish->theta >=0  && killFish->theta < 90){
    i = 0 ; 
    while( i <= 2){
      OceanEvent *killEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (killEvent) OceanEvent(killFish);
      killEvent->recvTime = killFish->recvTime;
      killEvent->dest = neighbourArray[i]; // NORTH , NE , EAST
      killEvent->eventType = 2 ;
      sendEvent(killEvent);
      //cout << *killEvent << endl ;
      i ++ ; 
    }
  }
  else if (killFish->theta >=90 && killFish->theta < 180 ){
    i = 6 ;
    while ( i <=7 ){
     
      OceanEvent *killEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (killEvent) OceanEvent(killFish);
      killEvent->recvTime = killFish->recvTime;
      killEvent->dest = neighbourArray[i];//WEST , N
      killEvent->eventType = 2 ;
      sendEvent(killEvent);
      //cout << *killEvent << endl ;
      i ++ ;
    }
    // and this for NORTH 
    OceanEvent *killEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
    new (killEvent) OceanEvent(killFish);
    killEvent->dest = neighbourArray[0];
    killEvent->recvTime = killFish->recvTime; 
    killEvent->eventType = 2 ;
    sendEvent(killEvent);
    //cout << *killEvent << endl ;
  }
  else if (killFish->theta >=180 && killFish->theta < 270 ){
    i = 4 ;
    while ( i <=6 ){
     
      OceanEvent *killEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (killEvent) OceanEvent(killFish);
      killEvent->recvTime = killFish->recvTime;
      killEvent->dest = neighbourArray[i];//WEST , SW SOUTH 
      killEvent->eventType = 2 ;
      sendEvent(killEvent);
      i ++ ;
      //cout << *killEvent << endl;
    }
    
  }
  else if (killFish->theta >=270 && killFish->theta < 360 ){
    i = 2 ; 
    while ( i <= 4){
     
      OceanEvent *killEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (killEvent) OceanEvent(killFish);
      killEvent->recvTime = killFish->recvTime;
      killEvent->dest = neighbourArray[i];//EAST , SE , SOUTH
      killEvent->eventType = 2;
      sendEvent(killEvent);
      i ++ ; 
      //cout << *killEvent << endl ;
    }
  }
}


void
OceanSector::projectDeadFish(OceanEvent *deadFish) {
  // on receiveing a kill event this guy is called 
  // check if the the sender is this sector 
  // if so check if the fish is still instate 
  // if so then mark as dead 
  // else - hope the fish moved out so do nothing 
 
  OceanEvent *tmpFish = isFishInSector(deadFish);

  if (tmpFish != NULL && tmpFish->eventType != 2 ){  // mark this fish as dead 
    tmpFish->eventType = 2 ; // dead ..... 
    
    if (deadFish->sender != id ){
      // only if the this is happening in a different sector 
      //than the original sector 'coz that is done by th generate kill event
       OceanEvent *copyDeadFish = (OceanEvent *) new char[sizeof(OceanEvent)];
        new (copyDeadFish) OceanEvent(deadFish) ;
        copyDeadFish->recvTime = deadFish->recvTime + 100 ; // why 10 ? 'coz 10 unit == i sec see getSimTime() 
      generateKillEvent(copyDeadFish);

      }
    else {
#ifdef SEQUENTIAL
        if(deadFish->recvTime <= SequentialLP::SIMUNTIL){
#else
        if(deadFish->recvTime <= LogicalProcess::SIMUNTIL){
#endif
           cout << deadFish->creatureId << " Fish Dead"  << " at "
                << deadFish->recvTime << endl ;
	}  
    }
  }
  else if ( tmpFish == NULL ){
    // the fish was not found so it might come later so put it in the queue 
    // with the dead flag
    deadFish->eventType = 2 ; 
    addFishToState(deadFish);
  }

}


void
OceanSector::addCreatureToNeighbour(OceanEvent *oe){
  
  if (oe->ct == FISH ){
    // we need to project only fishes atleast thats what I think 
    // check if the fish has been created in an overlapping region
    if(oe->xCord <= innerWestBound){
      // send message to the secto West of this one 
      OceanEvent *newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (newEvent) OceanEvent(oe);
      newEvent->dest = neighbourArray[WEST];
      newEvent->recvTime = getLVT() + 10 ;
      sendEvent(newEvent);
      if (oe->yCord <= innerNorthBound ){
	OceanEvent *newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
	new (newEvent) OceanEvent(oe);
	newEvent->dest = neighbourArray[NW];
	newEvent->recvTime = getLVT() + 10 ;
	sendEvent(newEvent);
      }
      if (oe->yCord >= innerSouthBound ){
	OceanEvent *newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
	new (newEvent) OceanEvent(oe);
	newEvent->dest = neighbourArray[SW];
	newEvent->recvTime = getLVT() + 10 ;
	sendEvent(newEvent);
      }
      
    }
    if (oe->xCord >= innerEastBound ){
      OceanEvent *newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (newEvent) OceanEvent(oe);
      newEvent->dest = neighbourArray[EAST];
      newEvent->recvTime = getLVT() + 10 ;
      sendEvent(newEvent);
      if (oe->yCord <= innerNorthBound ){
	OceanEvent *newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
	new (newEvent) OceanEvent(oe);
	newEvent->dest = neighbourArray[NE];
	newEvent->recvTime = getLVT() + 10 ;
	sendEvent(newEvent);
      }
      if (oe->yCord >= innerSouthBound ){
	OceanEvent *newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
	new (newEvent) OceanEvent(oe);
	newEvent->dest = neighbourArray[SE];
	newEvent->recvTime = getLVT() + 10 ;
	sendEvent(newEvent);
      }
    }
    if (oe->yCord <= innerNorthBound ){
      OceanEvent *newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (newEvent) OceanEvent(oe);
      newEvent->dest = neighbourArray[NORTH];
      newEvent->recvTime = getLVT() + 10 ; 
      sendEvent(newEvent);
    }
    if (oe->yCord >= innerSouthBound ){
      OceanEvent *newEvent = (OceanEvent *) new char[sizeof(OceanEvent)];
      new (newEvent) OceanEvent(oe);
      newEvent->dest = neighbourArray[SOUTH];
      newEvent->recvTime = getLVT() + 10 ;
      sendEvent(newEvent);
    }
  }
}

void 
OceanSector::finalize(){
#ifdef STATS
  strstream filename ; 
  filename << name << ".final" << ends ;
  ofstream finalState(filename.str());
  finalState << " The time is " << getLVT() << endl;
 
  OceanEvent * tmp = ((OceanSectorState *)state->current)->fishList.getHead();
  while (tmp != NULL){
    finalState << tmp->ct << " " << "  (" << tmp->xCord  << "," 
	       << tmp->yCord << ")  " << tmp->creatureId << " " ;
    finalState << tmp->eventType << " " 
	       << tmp->theta << endl;
    tmp = tmp->next ;
  }
  
  tmp = ((OceanSectorState *)state->current)->sharkList.getHead();
  while (tmp != NULL){
    finalState << tmp->ct << " " << "(" << tmp->xCord  << "," 
	       << tmp->yCord << ")" << tmp->creatureId << " "  ;

    finalState << tmp->eventType << " "  ;
    finalState << tmp->theta << endl ;
    tmp = tmp->next ;
  }
#endif
}
#endif 






















