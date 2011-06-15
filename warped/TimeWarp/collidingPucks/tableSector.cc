#ifndef TABLESECTOR_CC
#define TABLESECTOR_CC

#include "tableSector.hh"
int eventCount = 0;
int collisionCount = 0;

tableSector::tableSector(int identifier , char *myName , int *dest){

  name = new char[strlen(myName)+1];
  strcpy(name,myName);

  state->nameOfObject = new char[strlen(myName)+1];
  strcpy(state->nameOfObject,myName);

  id = identifier ;
  for (int i = 0 ; i < 8 ; i++){
    neighbourArray[i] = dest[i] ;
  }

  ifstream inputFile("collidingPucks.config");
  if (inputFile.good()) {
    inputFile >> noOfSectorsPerRow >> sectorWidth ;
  }
  else {
    cerr << "Cannot find file collidingPucks.config" << endl;
  }

  totalNoOfSectors = (noOfSectorsPerRow * noOfSectorsPerRow) ;
  int XIndex = (id - 1) % noOfSectorsPerRow ;
  int YIndex = (id - 1) / noOfSectorsPerRow ;

  westBound = XIndex*sectorWidth;
  eastBound = (XIndex+1)*sectorWidth;
  northBound = YIndex*sectorWidth;
  southBound = (YIndex+1)*sectorWidth;
  delete [] myName;
}

tableSector::~tableSector(){
  cout << " Number of Events " << eventCount << endl;
  cout << " Number of Collisions " << collisionCount << endl;
  //  delete [] name;
}

void tableSector::executeProcess(){
  eventCount++;
  tableEvent *recvEvent = (tableEvent *)getEvent();
  
  //check for event type

  if (recvEvent != NULL){
    if (recvEvent->eventType == 0){
      // 0 is arrival
         
      tableEvent *chkPuck = isPuckInSector(recvEvent);
      if (chkPuck == NULL){
	addPuckToState(recvEvent); //overlapping sector
    
	checkForCollisions();
	projectPuck(recvEvent);
     
      }

      else {
	chkPuck->xCord = recvEvent->xCord ;
	chkPuck->yCord = recvEvent->yCord ;
     
	checkForCollisions(); 
	projectPuck(recvEvent);
       
      }
    }
    else if (recvEvent->eventType == 1) {
      // 1 is departure
   
      removePuckFromState(recvEvent);
    }

    else if (recvEvent->eventType == 2){
      // 2 is collision
      //  collisionCount++;
      //     cout <<"Collision at " << recvEvent->recvTime << endl; 
      //  cout << *recvEvent;
    }
  } 
}

tableEvent *
tableSector::isPuckInSector(tableEvent *chkPuck){
  tableEvent *tmpPuck ;
  tableEvent *found = NULL ;

  tmpPuck = ((tableSectorState *) state->current)->puckList.getHead();
  while (tmpPuck != NULL && found == NULL){
    if (tmpPuck->puckId == chkPuck->puckId && tmpPuck->sectorCreatedBy == chkPuck->sectorCreatedBy ){
      found = tmpPuck;
    }
    tmpPuck = tmpPuck->next ;
  }
  return found;
}

int
tableSector::getXIndex() {
  return  (id - 1) % noOfSectorsPerRow ;
}

int
tableSector::getYIndex() {
  return (id - 1) / noOfSectorsPerRow ;
}

void
tableSector::addPuckToState(tableEvent *newPuck){
  tableEvent *x = (tableEvent *) new char[sizeof(tableEvent)];
  new (x) tableEvent(newPuck);
  ((tableSectorState *) state->current)->puckList.insert(x) ;
  
}

void tableSector::removePuckFromState(tableEvent *rmPuck){
  
  tableEvent *found = NULL ;
  tableEvent *tmp = ((tableSectorState *) state->current)->puckList.getHead();
  
  while(tmp != NULL && found == NULL ) {
    if(tmp->puckId == rmPuck->puckId && tmp->sectorCreatedBy == rmPuck->sectorCreatedBy){
      found = tmp ;
    }
    tmp = tmp->next ;
  }
  if (found == NULL){
  }
  else {
    ((tableSectorState *) state->current)->puckList.remove(found);
    delete [] (char *)found;
  }
}

void tableSector::projectPuck(tableEvent *projPuck){
  
  if (projPuck->theta >= 0 && projPuck->theta < 90){
    calculateMinDist(projPuck, northBound ,eastBound, 0);
  } else if (projPuck->theta >=90 && projPuck->theta < 180){
    calculateMinDist(projPuck, northBound, westBound, 1);
  } else if (projPuck->theta >=180 && projPuck->theta < 270){ 
    calculateMinDist(projPuck, southBound, westBound,2);
  } else if (projPuck->theta >=270 && projPuck->theta < 360){
    calculateMinDist(projPuck, southBound, eastBound, 3);
  } 
  else {
    cout << "Invalid Angle of movement" << endl ;
  }
}


void
tableSector::calculateMinDist(tableEvent *te , int yBound , int xBound , int quadrant){
  // Finds the Closest neighbouring sector and schedules event 
  // to send the puck to its closest neighbour.
  
  double distToN ; 
  double distToS ;
  double distToE ;
  double distToW ;
  
  
  tableEvent *newEvent;
  tableEvent *exitEvent;
  int totalWidth = noOfSectorsPerRow * sectorWidth ;
  switch(quadrant){
  case 0:
    if (te->yCord - yBound < 0 ){
      distToN = tan(45*M_PI/180)*totalWidth ;
    }
    else { 
      distToN  = (te->yCord - yBound)/te->sine ;
    }
    
    if (xBound - te->xCord < 0 ){
      distToE =  tan(45*M_PI/180)*totalWidth ;
    }
    else {
      distToE = (xBound - te->xCord)/te->cosine ;
    }
    
    if (distToN <= distToE   ){
      
      if(neighbourArray[NORTH] == -10){
	newEvent = (tableEvent *) new char [sizeof(tableEvent)]; 
	new (newEvent) tableEvent(te);
	newEvent->recvTime = getLVT() + getSimTime(distToN / te->velocity) ;
	generateEdgeCollisionEvent(newEvent);
        
      }
      else {
	newEvent = (tableEvent *) new char [sizeof(tableEvent)]; 
	new (newEvent) tableEvent(te);
	newEvent->xCord = te->xCord +(int)(distToN  *  te->cosine) ;
	newEvent->yCord = yBound ; 
	newEvent->recvTime = getLVT() + getSimTime(distToN / te->velocity) ;
	newEvent->entryPoint  = NORTH  ;
	newEvent->dest = neighbourArray[NORTH];
        newEvent->size = sizeof(tableEvent);
	sendEvent(newEvent);
	
	if (yBound == (getYIndex() *sectorWidth) ||
	    xBound == (getXIndex()*sectorWidth + sectorWidth)){
	  exitEvent = (tableEvent *) new char [sizeof(tableEvent)];
	  new (exitEvent) tableEvent(newEvent);
	  exitEvent->eventType = 1;
	  exitEvent->dest = id ;
	  exitEvent->recvTime = newEvent->recvTime ;
          exitEvent->size = sizeof(tableEvent); 
	  sendEvent(exitEvent);
	  
	}    
      }
    }
    
    if (distToE <= distToN ){
      if(neighbourArray[EAST] == -10){
	newEvent = (tableEvent *) new char [sizeof(tableEvent)]; 
	new (newEvent) tableEvent(te);
	newEvent->recvTime = getLVT() + getSimTime(distToE / te->velocity) ;
	generateEdgeCollisionEvent(newEvent);
	
      }
      else {
	newEvent = (tableEvent *) new char [sizeof(tableEvent)]; 
	new (newEvent) tableEvent(te);
	newEvent->xCord = xBound ;
	newEvent->yCord = te->yCord - (int)(distToE *te->sine) ; 
	newEvent->recvTime = getLVT() + getSimTime(distToE / te->velocity) ;
	newEvent->entryPoint  = EAST  ;
	newEvent->dest = neighbourArray[EAST];
        newEvent->size = sizeof(tableEvent);
	sendEvent(newEvent);
	
	if (yBound == (getYIndex() *sectorWidth) ||
	    xBound == (getXIndex()*sectorWidth + sectorWidth)){
	  exitEvent = (tableEvent *) new char [sizeof(tableEvent)];
	  new (exitEvent) tableEvent(newEvent);
	  exitEvent->eventType = 1;
	  exitEvent->dest = id ;
	  exitEvent->recvTime = newEvent->recvTime ;
          exitEvent->size = sizeof(tableEvent);
	  sendEvent(exitEvent);
	}
      }
    }
    
    if (distToE == distToN )  {
      if(neighbourArray[NE] == -10){
	newEvent = (tableEvent *) new char [sizeof(tableEvent)]; 
	new (newEvent) tableEvent(te);
	newEvent->recvTime = getLVT() + getSimTime(distToE / te->velocity) ;
	generateEdgeCollisionEvent(newEvent);
      }
      else {
	newEvent = (tableEvent *) new char [sizeof(tableEvent)]; 
	new (newEvent) tableEvent(te);
	newEvent->xCord = xBound ;
	newEvent->yCord = yBound ; 
	newEvent->recvTime = getLVT() + getSimTime(distToE / te->velocity) ;
	newEvent->entryPoint  = NE  ;
	newEvent->dest = neighbourArray[NE];
	
	if (yBound == (getYIndex() *sectorWidth) ||
	    xBound == (getXIndex()*sectorWidth + sectorWidth)){
	  exitEvent = (tableEvent *) new char [sizeof(tableEvent)];
	  new (exitEvent) tableEvent(newEvent);
	  exitEvent->eventType = 1;
	  exitEvent->dest = id ;
	  exitEvent->recvTime = newEvent->recvTime ;
	  exitEvent->size = sizeof(tableEvent);
          sendEvent(exitEvent);
	}
      }
    }
    break;
    
  case 1:
    if (te->yCord - yBound < 0 ){
      distToN =  tan(45*M_PI/180)*totalWidth ;
    }
    else {
      distToN = (te->yCord - yBound)/te->sine ;
    }
    
    if (te->xCord - xBound < 0){
      distToW =  tan(45*M_PI/180)*totalWidth  ;
    }
    else {
      distToW = (te->xCord - xBound)/(te->cosine) ;
    }
    
    if (distToN < distToW ){
      if(neighbourArray[NORTH]==-10){
	newEvent = (tableEvent *) new char [sizeof(tableEvent)]; 
	new (newEvent) tableEvent(te);
	newEvent->recvTime = getLVT() + getSimTime(distToN / te->velocity) ;
	generateEdgeCollisionEvent(newEvent);
      }
      else { 
	newEvent = (tableEvent *) new char [sizeof(tableEvent)];
	new (newEvent) tableEvent(te);
	newEvent->xCord = te->xCord - (int)(distToN * (te->cosine)) ;
	newEvent->yCord = yBound ;
	newEvent->recvTime = getLVT() +getSimTime(distToN / te->velocity) ; 
	newEvent->entryPoint = NORTH  ;
	newEvent->dest = neighbourArray[NORTH];
	newEvent->size = sizeof(tableEvent);
	sendEvent(newEvent);
	
        if (yBound == (getYIndex() *sectorWidth) ||
	    xBound == (getXIndex()*sectorWidth + sectorWidth)){
	  exitEvent = (tableEvent *) new char [sizeof(tableEvent)];
	  new (exitEvent) tableEvent(newEvent);
	  exitEvent->eventType = 1;
	  exitEvent->dest = id ;
	  exitEvent->recvTime = newEvent->recvTime ;
	  exitEvent->size = sizeof(tableEvent);
	  sendEvent(exitEvent);
	}
      }
    }
    
    if (distToW < distToN){
      if(neighbourArray[WEST] == -10){
	newEvent = (tableEvent *) new char [sizeof(tableEvent)]; 
	new (newEvent) tableEvent(te);
	newEvent->recvTime = getLVT() + getSimTime(distToW / te->velocity) ;
	generateEdgeCollisionEvent(newEvent);
      }
      else {
	newEvent = (tableEvent *) new char [sizeof(tableEvent)];
	new (newEvent) tableEvent(te);
	newEvent->xCord = xBound ;
	newEvent->yCord = te->yCord - (int)(distToW * te->sine) ;
	newEvent->recvTime = getLVT() +getSimTime(distToW / te->velocity) ; 
	newEvent->entryPoint = WEST  ;
	newEvent->dest = neighbourArray[WEST];
	newEvent->size = sizeof(tableEvent);
	sendEvent(newEvent);
	
        if (yBound == (getYIndex() *sectorWidth) ||
	    xBound == (getXIndex()*sectorWidth + sectorWidth)){
	  exitEvent = (tableEvent *) new char [sizeof(tableEvent)];
	  new (exitEvent) tableEvent(newEvent);
	  exitEvent->eventType = 1;
	  exitEvent->dest = id ;
	  exitEvent->recvTime = newEvent->recvTime ;
	  exitEvent->size = sizeof(tableEvent);
	  sendEvent(exitEvent);
	}
      }
    } 
    
    if (distToW == distToN){
      if(neighbourArray[NW] == -10){
	newEvent = (tableEvent *) new char [sizeof(tableEvent)]; 
	new (newEvent) tableEvent(te);
	newEvent->recvTime = getLVT() + getSimTime(distToW / te->velocity) ;
	generateEdgeCollisionEvent(newEvent);

      }
      else {
	newEvent = (tableEvent *) new char [sizeof(tableEvent)];
	new (newEvent) tableEvent(te);
	newEvent->xCord = xBound ;
	newEvent->yCord = yBound ;
	newEvent->recvTime = getLVT() +getSimTime(distToW / te->velocity) ; 
	newEvent->entryPoint = NW  ;
	newEvent->dest = neighbourArray[NW];
	newEvent->size = sizeof(tableEvent);
	sendEvent(newEvent);
	
        if (yBound == (getYIndex() *sectorWidth) ||
	    xBound == (getXIndex()*sectorWidth + sectorWidth)){
	  exitEvent = (tableEvent *) new char [sizeof(tableEvent)];
	  new (exitEvent) tableEvent(newEvent);
	  exitEvent->eventType = 1;
	  exitEvent->dest = id ;
	  exitEvent->recvTime = newEvent->recvTime ;
	  exitEvent->size = sizeof(tableEvent);
	  sendEvent(exitEvent);
	}
      } 
    }  
    break ;
    
  case 2:
    if (yBound - te->yCord < 0){
      distToS =  tan(45*M_PI/180)*totalWidth ;
    }
    else {
      distToS = (yBound - te->yCord)/(te->sine) ;
    }
    
    if (te->xCord - xBound < 0){
      distToW =  tan(45*M_PI/180)*totalWidth  ;
    }
    else {
      distToW = (te->xCord - xBound)/(te->cosine) ;
    }
    
    if (distToS < distToW ){
      if(neighbourArray[SOUTH] == -10){
	newEvent = (tableEvent *) new char [sizeof(tableEvent)]; 
	new (newEvent) tableEvent(te);
	newEvent->recvTime = getLVT() + getSimTime(distToS / te->velocity) ;
	generateEdgeCollisionEvent(newEvent);
      }
      else {
	newEvent = (tableEvent *) new char [sizeof(tableEvent)];
	new (newEvent) tableEvent(te);
	newEvent->xCord = te->xCord - (int)(distToS * (te->cosine));
	newEvent->yCord = yBound ;
	newEvent->recvTime = getLVT() +getSimTime(distToS / te->velocity) ; 
	newEvent->entryPoint = SOUTH  ;
	newEvent->dest = neighbourArray[SOUTH];
	newEvent->size = sizeof(tableEvent);
	sendEvent(newEvent);

        if (yBound == (getYIndex() *sectorWidth) ||
	    xBound == (getXIndex()*sectorWidth + sectorWidth)){
	  exitEvent = (tableEvent *) new char [sizeof(tableEvent)];
	  new (exitEvent) tableEvent(newEvent);
	  exitEvent->eventType = 1;
	  exitEvent->dest = id ;
	  exitEvent->recvTime = newEvent->recvTime ;
	  exitEvent->size = sizeof(tableEvent);
	  sendEvent(exitEvent);
	}
      }
    }

    if (distToW < distToS ){
      if(neighbourArray[WEST] == -10){
	newEvent = (tableEvent *) new char [sizeof(tableEvent)]; 
	new (newEvent) tableEvent(te);
	newEvent->recvTime = getLVT() + getSimTime(distToW / te->velocity) ;
	generateEdgeCollisionEvent(newEvent);
      }
      else {
	newEvent = (tableEvent *) new char [sizeof(tableEvent)];
	new (newEvent) tableEvent(te);
	newEvent->xCord = xBound ;
	newEvent->yCord = te->yCord + (int)(distToW * (te->sine)) ;
	newEvent->recvTime = getLVT() +getSimTime(distToW / te->velocity) ; 
	newEvent->entryPoint = WEST  ;
	newEvent->dest = neighbourArray[WEST];
	newEvent->size = sizeof(tableEvent);
	sendEvent(newEvent);
      
        if (yBound == (getYIndex() *sectorWidth) ||
	    xBound == (getXIndex()*sectorWidth + sectorWidth)){
	  exitEvent = (tableEvent *) new char [sizeof(tableEvent)];
	  new (exitEvent) tableEvent(newEvent);
	  exitEvent->eventType = 1;
	  exitEvent->dest = id ;
	  exitEvent->recvTime = newEvent->recvTime ;
	  exitEvent->size = sizeof(tableEvent);
	  sendEvent(exitEvent);
	}
      }
    }
    
    if (distToW == distToS ) {
      if(neighbourArray[SW] == -10){
        newEvent = (tableEvent *) new char [sizeof(tableEvent)]; 
	new (newEvent) tableEvent(te);
	newEvent->recvTime = getLVT() + getSimTime(distToW / te->velocity) ;
	generateEdgeCollisionEvent(newEvent);
      }
      else {
	newEvent = (tableEvent *) new char [sizeof(tableEvent)];
	new (newEvent) tableEvent(te);
	newEvent->xCord = xBound ;
	newEvent->yCord = yBound ;
	newEvent->recvTime = getLVT() +getSimTime(distToW / te->velocity) ; 
	newEvent->entryPoint = SW  ;
	newEvent->dest = neighbourArray[SW];
	newEvent->size = sizeof(tableEvent);
	sendEvent(newEvent);

        if (yBound == (getYIndex() *sectorWidth) ||
	    xBound == (getXIndex()*sectorWidth + sectorWidth)){
	  exitEvent = (tableEvent *) new char [sizeof(tableEvent)];
	  new (exitEvent) tableEvent(newEvent);
	  exitEvent->eventType = 1;
	  exitEvent->dest = id ;
	  exitEvent->recvTime = newEvent->recvTime ;
	  exitEvent->size = sizeof(tableEvent);
	  sendEvent(exitEvent);
        }
      }
    }
    
    break ;
    
  case 3:
    
    if (yBound - te->yCord < 0){
      distToS =  tan(45*M_PI/180)*totalWidth ;
    }
    else {
      distToS = (yBound - te->yCord)/(te->sine) ;
    }
    
    if (xBound - te->xCord < 0){
      distToE =  tan(45*M_PI/180)*totalWidth  ;
    }
    else {
      distToE = (xBound - te->xCord)/(te->cosine);
    }
    
    if (distToS < distToE ){
      if(neighbourArray[SOUTH] == -10){
	newEvent = (tableEvent *) new char [sizeof(tableEvent)]; 
	new (newEvent) tableEvent(te);
	newEvent->recvTime = getLVT() + getSimTime(distToS / te->velocity) ;
	generateEdgeCollisionEvent(newEvent);
      }
      else {
	newEvent = (tableEvent *) new char [sizeof(tableEvent)];
	new (newEvent) tableEvent(te);
	newEvent->xCord = te->xCord - (int)(distToS * (te->cosine));
	newEvent->yCord = yBound ;
	newEvent->recvTime = getLVT() +getSimTime(distToS / te->velocity) ; 
	newEvent->entryPoint = SOUTH  ;
	newEvent->dest = neighbourArray[SOUTH];
	newEvent->size = sizeof(tableEvent);
	sendEvent(newEvent);
   
        if (yBound == (getYIndex() *sectorWidth) ||
	    xBound == (getXIndex()*sectorWidth + sectorWidth)){
	  exitEvent = (tableEvent *) new char [sizeof(tableEvent)];
	  new (exitEvent) tableEvent(newEvent);
	  exitEvent->eventType = 1;
	  exitEvent->dest = id ;
	  exitEvent->recvTime = newEvent->recvTime ;
	  exitEvent->size = sizeof(tableEvent);
	  sendEvent(exitEvent);
	}
      }
    }   
 
    if (distToE < distToS ){

      if(neighbourArray[EAST] == -10){
        newEvent = (tableEvent *) new char [sizeof(tableEvent)]; 
	new (newEvent) tableEvent(te);
	newEvent->recvTime = getLVT() + getSimTime(distToE / te->velocity) ;
	generateEdgeCollisionEvent(newEvent);

      }
      else {
	newEvent = (tableEvent *) new char [sizeof(tableEvent)];
	new (newEvent) tableEvent(te);
	newEvent->xCord = xBound ;
	newEvent->yCord = te->yCord + (int)(distToE * (te->sine)) ;
	newEvent->recvTime = getLVT() +getSimTime(distToE / te->velocity) ; 
	newEvent->entryPoint = EAST  ;
	newEvent->dest = neighbourArray[EAST]; 
	newEvent->size = sizeof(tableEvent);
	sendEvent(newEvent);

        if (yBound == (getYIndex() *sectorWidth) ||
	    xBound == (getXIndex()*sectorWidth + sectorWidth)){
	  exitEvent = (tableEvent *) new char [sizeof(tableEvent)];
	  new (exitEvent) tableEvent(newEvent);
	  exitEvent->eventType = 1;
	  exitEvent->dest = id ;
	  exitEvent->recvTime = newEvent->recvTime ;
	  exitEvent->size = sizeof(tableEvent);
	  sendEvent(exitEvent);
	}
      }
    }   
    if (distToE ==distToS){

      if(neighbourArray[SE] == -10){
	newEvent = (tableEvent *) new char [sizeof(tableEvent)]; 
	new (newEvent) tableEvent(te);
	newEvent->recvTime = getLVT() + getSimTime(distToE / te->velocity) ;
	generateEdgeCollisionEvent(newEvent);

      }
      else {
	newEvent = (tableEvent *) new char [sizeof(tableEvent)];
	new (newEvent) tableEvent(te);
	newEvent->xCord = xBound;
	newEvent->yCord = yBound ;
	newEvent->recvTime = getLVT() +getSimTime(distToE / te->velocity) ; 
	newEvent->entryPoint = SE  ;
	newEvent->dest = neighbourArray[SE];
	newEvent->size = sizeof(tableEvent);
	sendEvent(newEvent);

        if (yBound == (getYIndex() *sectorWidth) ||
	    xBound == (getXIndex()*sectorWidth + sectorWidth)){
	  exitEvent = (tableEvent *) new char [sizeof(tableEvent)];
	  new (exitEvent) tableEvent(newEvent);
	  exitEvent->eventType = 1;
	  exitEvent->dest = id ;
	  exitEvent->recvTime = newEvent->recvTime ;
	  exitEvent->size = sizeof(tableEvent);
	  sendEvent(exitEvent);
	}
      }
    }   
    
    break ;
    
  }
  //   delete [] te;
  // delete [] newEvent;
  //  delete [] exitEvent;
}

void 
tableSector::checkForCollisions(){
  //This guy takes the list of Pucks in a sector and checks for collisions.
  
  tableEvent *tmpPuck1;
  tableEvent *tmpPuck2;

  double a , b , c ;                
  double t1 , t2 ;
  int countPuck1 = 0;

  int countPuck2 = 0;
 
  tmpPuck1 = ((tableSectorState *) state->current)->puckList.getHead();
  tmpPuck2 = ((tableSectorState *) state->current)->puckList.getHead();
  tmpPuck2 = tmpPuck2->next;

  while(tmpPuck1 != NULL) {
    countPuck1++;
    while ( tmpPuck2 != NULL && tmpPuck2->eventType != 2){
      countPuck2++;
      a = sqr(tmpPuck1->velocity * cos(tmpPuck1->theta) -
	      tmpPuck2->velocity * cos(tmpPuck2->theta)) + 
	sqr(tmpPuck1->velocity * sin(tmpPuck1->theta) - 
	    tmpPuck2->velocity * sin(tmpPuck2->theta)) ;     
 
 
      b = 2 *(tmpPuck1->velocity * cos(tmpPuck1->theta) -
	      tmpPuck2->velocity * cos(tmpPuck2->theta ))*
	(tmpPuck2->xCord - tmpPuck2->xCord) +
	2*(tmpPuck1->velocity * sin(tmpPuck1->theta) - 
	   tmpPuck2->velocity * sin(tmpPuck2->theta ))*
	(tmpPuck1->yCord - tmpPuck2->yCord);


      c = sqr((long int )(tmpPuck1->xCord - tmpPuck2->xCord)) +
	sqr((long int )(tmpPuck1->yCord - tmpPuck2->yCord)) ;
   
      //out << sqr(b)-4*a*c << endl;
      t1 =   (1 * b + sqrt(abs(sqr(b)-4*a*c)))/(2*a);                 
      t2 =  (-1 * b - sqrt(abs(sqr(b)-4*a*c)))/(2*a);
      // cout << a << " ," << b << " , " << c << endl;
      // cout << t1 << " , " << t2 << endl;
 
      if (t1  > 0  && t2 > 0 ){

	if ( t1 < t2 ){
	  // cout << " I am Here " << endl;
	  tmpPuck1->recvTime = getLVT() + getSimTime(t1);
	  tmpPuck2->recvTime = getLVT() + getSimTime(t1);
	  // cout << tmpPuck1->puckId << " , " << tmpPuck2->puckId 
	  //     << " collided at " <<tmpPuck1->recvTime << endl; 
	  generateCollisionEvent(tmpPuck1,tmpPuck2 ) ;
	}

	else { 
	  //cout << " I am Here " << endl;
          tmpPuck1->recvTime = getLVT() + getSimTime(t2); 
	  tmpPuck2->recvTime = getLVT() + getSimTime(t2);
	  // cout << tmpPuck1->puckId << " , " << tmpPuck2->puckId 
          //    << " collided at " <<tmpPuck1->recvTime << endl; 
	  generateCollisionEvent(tmpPuck1,tmpPuck2 );
	}
      }
        
      else if ( t1 > 0 && t2 <= 0 ){
	//cout << " I am Here " << endl;
	tmpPuck1->recvTime = getLVT() + getSimTime(t1);
	tmpPuck2->recvTime = getLVT() + getSimTime(t1);
	// cout << tmpPuck1->puckId << " , " << tmpPuck2->puckId 
	//   << " collided at " <<tmpPuck1->recvTime << endl; 
	generateCollisionEvent(tmpPuck1,tmpPuck2 );
      }

      else if  (t2 > 0 && t1 <= 0 ){
	//cout<<" I am Here " << endl;
	tmpPuck1->recvTime = getLVT() + getSimTime(t2);
	tmpPuck2->recvTime = getLVT() + getSimTime(t2);
	//   cout << tmpPuck1->puckId << " , " << tmpPuck2->puckId 
	//<< " collided at " <<tmpPuck1->recvTime << endl; 
	generateCollisionEvent(tmpPuck1,tmpPuck2 );
      }
      else if (t1  < 0  && t2 < 0 ){

	if ( t1 < t2 ){
	  //cout << " I am Here " << endl;
	  tmpPuck1->recvTime = getLVT() + getSimTime(t1);
	  tmpPuck2->recvTime = getLVT() + getSimTime(t1);
	  // cout << tmpPuck1->puckId << " , " << tmpPuck2->puckId 
	  //   << " collided at " <<tmpPuck1->recvTime << endl; 
	  generateCollisionEvent(tmpPuck1,tmpPuck2 ) ;
	}

	else { 
	  //  cout << " I am Here " << endl;
          tmpPuck1->recvTime = getLVT() + getSimTime(t2); 
	  tmpPuck2->recvTime = getLVT() + getSimTime(t2);
	  // cout << tmpPuck1->puckId << " , " << tmpPuck2->puckId 
          //    << " collided at " <<tmpPuck1->recvTime << endl; 
	  generateCollisionEvent(tmpPuck1,tmpPuck2 );
	}
      }
      //  cout << *tmpPuck2 << endl;
      tmpPuck2 = tmpPuck2->next;
        
    }
    tmpPuck1 = tmpPuck1->next ;
  }
}

void 
tableSector::generateCollisionEvent(tableEvent* Puck1 , tableEvent* Puck2){
  //cout << " I am Here " << endl;
  Puck1->theta = 360 - Puck1->theta;
  Puck2->theta = 360 - Puck2->theta;
  tableEvent  *collisionEvent = (tableEvent *) new char[sizeof(tableEvent)];
  //cout << Puck1->puckId << " , " << Puck2->puckId;
  new (collisionEvent) tableEvent(Puck1);
  collisionEvent->recvTime = Puck1->recvTime ;
  collisionEvent->dest = id ;
  collisionEvent->eventType = 2 ;
  collisionEvent->size = sizeof(tableEvent);
  sendEvent(collisionEvent);
  
  tableEvent  *collisionEvent1= (tableEvent *) new char[sizeof(tableEvent)];
  
  new (collisionEvent1) tableEvent(Puck2);
  collisionEvent1->recvTime = Puck2->recvTime ;
  collisionEvent1->dest = id ;
  collisionEvent1->eventType = 2 ;
  collisionEvent->size = sizeof(tableEvent);
  sendEvent(collisionEvent1);
  
#ifdef SEQUENTIAL
  if(Puck1->recvTime <= SequentialLP::SIMUNTIL){
#else
    if(Puck1->recvTime <= LogicalProcess::SIMUNTIL){
#endif
      cout << Puck1->puckId << " , " << Puck2->puckId 
	   << " collided at " <<Puck1->recvTime << endl;
      collisionCount++;
    }
  }

void
tableSector::generateEdgeCollisionEvent(tableEvent* Puck){
  //This guy takes care of the case when Puck hits table edge
  //Just sends messg to itself making angle 360-theta ie move in opp dir
  
  Puck->theta = 360 - Puck->theta;
  
  tableEvent  *collisionEvent = (tableEvent *) new char[sizeof(tableEvent)];
  
  new (collisionEvent) tableEvent(Puck);
  collisionEvent->recvTime = Puck->recvTime  ;
  collisionEvent->dest = id ;
  collisionEvent->eventType = 2 ;
  collisionEvent->size = sizeof(tableEvent);
  sendEvent(collisionEvent);
  
#ifdef SEQUENTIAL
  if(Puck->recvTime <= SequentialLP::SIMUNTIL){
#else
    if(Puck->recvTime <= LogicalProcess::SIMUNTIL){
#endif
      cout << Puck->puckId << " collided with an edge at "  << Puck->recvTime << endl;
      collisionCount++;
      delete [] (char *)Puck;
    }
    // cout << *collisionEvent;
  }
#endif

