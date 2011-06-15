//-*-c++-*-
#ifndef MATTERN_GVT_MANAGER_HH
#define MATTERN_GVT_MANAGER_HH
#include "warped.hh"
#include "config.hh"

#ifdef MATTERNGVTMANAGER
// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.

// BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY 
// FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT 
// PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, 
// EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE 
// PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME 
// THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION. 

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
// $Id: 
//
//---------------------------------------------------------------------------
#include "BasicTimeWarp.hh"
#include "KernelMsgs.hh"
#include "CommMgrInterface.hh"
#include "OneShot.hh"

class ObjectRecord;
class CommMgrInterface;

const int UNCOLORED = -1;
const int WHITE = 0;
const int RED = 1;

class MatternGVTManager : public BasicTimeWarp {
  friend class LogicalProcess;
  friend class OfcManager;
public:

  
  MatternGVTManager(){
    gVT = ZERO;
    lGVT = ZERO;
    definedGVT = ZERO;
    lastReported = ZERO;
    lastIdle = false;
    lpIdle = false;
    tokenPending = false;
    gVTTokenPending = false;
    oldgVT = ZERO;
    noOfGVTToSkip = 0;
    gvtPeriod = 0;
    memoryUseage = 0;
    maxMemoryUseage = 0;
#ifdef EXTERNALAGENT
    tokenAlreadySent = false;
    tokenReceived = false;
    numberOfEvents = 0;
    tokenHasBeenReceived = false;
#endif
    
    ranBetweenTokens = false ;
    gcollectCalled = 0;
  };

  ~MatternGVTManager(){
    delete [] simArray;
  };

#ifdef EXTERNALAGENT  
  // flags to indicate token dispatch or receive
  bool tokenAlreadySent;
  bool tokenReceived;
  long int numberOfEvents;
  bool tokenHasBeenReceived;
  void readyToPassItOn();
#endif
  void initGVTManager(ObjectRecord*, CommMgrInterface*,int, int, int, int); 
  // the array of simObjects
  // the address of the commManager
  // the id of this lp
  // the total number of objects
  // the number of local guys
  // the number of LPs
  
  void msgRecvPeek( EventMsg *);
  void recvMsg(newGVTMsg *);
  void recvMsg(MatternGVTMsg *); // received a MatternGVTMsg token 
#ifdef EXTERNALAGENT  
  void recvMsg(ExtAgentMsg *);
#endif
  void recvMsg(CheckIdleMsg *);
  void calculateLGVT();    // calculate the local estimate of GVT
                           // also figures out if the LP is idle or not...

  void calculateGVT();    // calculate the actual value of gVT.  This
				// function should only be called in the
				// function that is responsible for the
				// calculation of the "real" gVT.  It is
				// assumed that LP 0 is responsible for gVT 
				// calculation

  VTime getGVT();         // return GVT to the caller
  void setGVT(VTime);     // set a new internal value for gVT
  VTime getLGVT();        // return local estimate to the caller
  void setLGVT(VTime);    // set the local estimate of GVT


   void sendEvent(newGVTMsg *);

  // these are here to make the compiler happy...
  void simulate();
  BasicEvent* getEvent();
  void sendEvent(BasicEvent *);
  void recvEvent(BasicEvent *);
  
  void sendEvent(MatternGVTMsg *);
  void passItOn(MatternGVTMsg *);
#ifdef EXTERNALAGENT  
  void sendEvent(ExtAgentMsg *);
  void passItOn(ExtAgentMsg *);
  void startExternalAgent();
#endif
  void sendEvent(TerminateMsg *);
  void sendEvent(CheckIdleMsg *);
  VTime calculateMin();
  void executeProcess();
  ObjectRecord* getArrayHandle(){ return simArray;}

  ofstream *lpFile;
  void setFile( ofstream * );
#ifdef LPDEBUG
#define DEBUGSTREAM *lpFile
#else
#define DEBUGSTREAM cerr
#endif
#ifdef STATS
  ofstream * statFile;
  void setStatFile( ofstream * lpStatFile) {
    statFile = lpStatFile;
  }
#endif
  // the LP sets this flag when all of the local simulation objects have
  // reported that they're idle
  OneShot<bool> lpIdle;

  
  void setRanBetweenTokens(bool) ;
  bool giveRanBetweenTokens() ;

  void terminateSimulation(char *msg);

  BasicState* allocateState();
  
  int memoryUseage;
  int maxMemoryUseage;
  int gvtPeriod;
  int gvtCycle;
  VTime avgGVTUpdate;
  VTime minGVTUpdate;
  VTime maxGVTUpdate;
  VTime gvtDiffSum;

  int gcollectCalled;
protected:
  virtual VTime gcollect();

private:
  bool colorAlreadyChanged;
  bool checkIdle();
  void cancelToken(CheckIdleMsg *);
  void passItOn(CheckIdleMsg *);
  void startToken();
  void startGVTToken();

  void calcMin();
  void deliver( BasicEvent*); 

  ObjectRecord* simArray;  // This is a pointer to the array of 
                           // simulation objects in the LP
  CommMgrInterface* commHandle; // our handle to our communication 
                           // manager

  int definedGVT;                 // this is for the "head" GVT guy only -
                                  // it is who set gVT last time

  VTime gVT;                       // global virtual time
  VTime lGVT;                      // our local estimate of gVT

  VTime lastReported;              // the last lGVT value sent to the head
                                  // gVT Manager
  bool lastIdle;                  // our last idle

  int numObjects;                 // the number of objects on our LP
  int totalObjects;               // the total number of simulaiton objects
  int numLPs;                     // the number of logical processes

  VTime* lastTimeSent;  // array of the last times of event messages that
                        // I sent to each LP (and are unacknowleged)

  int currentToken;
  bool tokenPending;
  bool gVTTokenPending;
  bool resetGVT;
  VTime oldgVT;
  int noOfGVTToSkip;
  SequenceCounter gVTTokenNum;
  
  bool ranBetweenTokens ;

};
#endif
#endif
