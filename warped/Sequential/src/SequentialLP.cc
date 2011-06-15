//-*-c++-*-
#ifndef SEQUENTIAL_LP_CC
#define SEQUENTIAL_LP_CC
// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.
//
// BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY 
// FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT 
// PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, 
// EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE 
// PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME 
// THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION. 
//
// IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING 
// WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR 
// REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR 
// DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL 
// DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM 
// (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED 
// INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF 
// THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH HOLDER 
// OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 
//
//
// $Id: SequentialLP.cc,v 1.4 1999/10/28 04:55:46 mal Exp $
//
//---------------------------------------------------------------------------
#include "SequentialLP.hh"
#include "ConfigurationManager.hh"

ConfigurationManager configurationMgr;
ConfigurationManager* configurationManager;

SequentialLP::SequentialLP(int totalNum, int myNum, int lpNum){
  int i;
  totalObjects  = totalNum;
  numObjects    = myNum;
  numLPs        = lpNum;
  simulationFinished = false;
  initialized   = false;
  numRegistered = 0;
  id            = 0;
 
  simArray = new SequentialObjectRecord[totalObjects];
  
#ifdef LPDEBUG
  char filename[20];
  sprintf(filename,"LP%d",id);
  lpFile.open(filename);
  if(lpFile == 0){
    cerr << "LP " << id << " couldn't open logging file!" << endl;
    exit(-1);
  }
  else {
    cout << "opened file " << filename << " for debug " << endl;
  }
#endif
  for (i = 0; i < totalObjects; i++) {
    simArray[i].ptr = NULL;
    simArray[i].lpNum = MAXINT;
  }
  
  srand48(time(NULL));
  // pass the GVTManager and CommManager the handle to the simulation objects
  
#ifdef STATS
  eventCount = 0;
#endif

  //The configurationManagers functions are called here to
  //configure different components of the kernel. Configuration
  //could be a seperate step. but, this is just a beginning
  //point towards a full fledged configuration manager
  configurationManager = &configurationMgr;
  BaseSequential::inputQ.setQImplementation(configurationManager->getQImplementation());
}

SequentialLP::~SequentialLP(){
  delete [] simArray ;
  
#ifdef LPDEBUG
  lpFile.close();
#endif
  
#ifdef STATS
  commitStatsFile.close();
#endif
  
#ifdef STATEDEBUG
  outFile2.close();
#endif

}

void 
SequentialLP::simulate(VTime) {
#ifdef STATS
  stopwatch.start();
#endif
  int i;
  if(initialized == false) {
    cerr << "allRegistered not called in LP " << id << "\n";
  }

  cout << "Initializing simulation objects...\n";
  
  for( i = 0; i < numObjects ; i++ ){
    simArray[i].ptr->timeWarpInit();
  }

  for( i = 0; i < numObjects ; i++ ){
    simArray[i].ptr->initialize();
  }

  cout << "Starting simulation...\n";
  
  simulationFinished = false;
  
  //  BasicEvent *nextEvent = BaseSequential::inputQ.gotoHead();
  //has been changed to peekEvent Interface

  BasicEvent *nextEvent = BaseSequential::inputQ.peekEvent();

  while (nextEvent != NULL)  {
    simArray[nextEvent->dest].ptr->simulate();  
    //    nextEvent = BaseSequential::inputQ.removeAlreadyProcessedEvents();

    nextEvent =  BaseSequential::inputQ.peekEvent();
    BaseSequential::inputQ.garbageCollect(NULL);

#ifdef STATS
    eventCount++;
#endif
  }
  
  for(i = 0; i < totalObjects; i++) {
    simArray[i].ptr->finalize();
  }

  cout << "Simulation complete.\n";
  
#ifdef STATS
  stopwatch.stop();
  cout << eventCount << " events processed in " << stopwatch.elapsed()/1000000000 <<
    " seconds = " << (double) (eventCount/stopwatch.elapsed() * 1000000000) <<
    " events/second>" << endl;
#endif
}

void 
SequentialLP::registerObject(BaseSequential* handle) {
  numRegistered++;
  
  if(simArray[handle->id].ptr == NULL)  {
    simArray[handle->id].ptr   = handle;
    simArray[handle->id].lpNum = id;
    handle->commHandle         = simArray;
  }
  else {
    cerr << "Object " << handle->id << " trying to register twice!\n";
    exit( -1 );
  }
  
#ifdef LPDEBUG
  handle->setFile(&lpFile);
#endif
}

void
SequentialLP::allRegistered() {
  if(numRegistered == numObjects)  {
    initialized = true;
    cout << "All objects registered Sequential\n";
  }
  else {
    cerr << "LP " << id << " incorrect number of objects registered!\n";
    cerr << "Expected " << numObjects << " objects, and " << numRegistered
	 << " registered!\n";
    exit( -1 );
  }
}

int
SequentialLP::getNumObjects() {
  return numObjects;
}

int
SequentialLP::getLPid(){
  return id ;
}

#endif
