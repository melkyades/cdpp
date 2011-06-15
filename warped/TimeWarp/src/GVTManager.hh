//-*-c++-*-
#ifndef GVT_MANAGER_HH
#define GVT_MANAGER_HH
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
// $Id: GVTManager.hh,v 1.7 1999/10/24 23:25:56 ramanan Exp $
//
//---------------------------------------------------------------------------
#include "warped.hh"
#include "config.hh"

#ifndef MATTERNGVTMANAGER

#include "BasicTimeWarp.hh"
#include "AckRecord.hh"
#include "KernelMsgs.hh"
#include "CommMgrInterface.hh"
#include "OneShot.hh"

int AckRecordCompare(const AckRecord*, const AckRecord*);

class ObjectRecord;
class CommMgrInterface;

class GVTManager : public BasicTimeWarp {
  friend class LogicalProcess;

public:
  GVTManager();
  virtual ~GVTManager();
  virtual void initGVTManager(ObjectRecord*, CommMgrInterface*,int, int, int, int); 
  // the array of simObjects
  // the address of the commManager
  // the id of this lp
  // the total number of objects
  // the number of local guys
  // the number of LPs
  
  virtual void recvMsg(LGVTMsg *);
  virtual void recvMsg(LowerLGVTMsg *);
  virtual void recvMsg(LPAckMsg *);
  virtual void recvMsg(RestoreCkptMsg * ){};
  void recvMsg(newGVTMsg *);
  void recvMsg(GVTAckMsg *);
  void recvMsg(CheckIdleMsg *);
  void recvMsg(DummyMsg *);
  void recvMsg(DummyRequestMsg *);

  virtual void calculateLGVT(); // calculate the local estimate of GVT
                                // also figures out if the LP is idle or not...

  virtual void calculateGVT();   // calculate the actual value of gVT.  This
                                 // function should only be called in the
                                 // function that is responsible for the
                                 // calculation of the "real" gVT.  It is
                                 // asumed that LP 0 is responisble for gVT 
                                 // calculation

  VTime getGVT();         // return GVT to the caller
  void setGVT(VTime);     // set a new internal value for gVT
  VTime getLGVT();        // return local estimate to the caller
  void setLGVT(VTime);    // set the local estimate of GVT
  
  virtual void sendEvent(LGVTMsg *);
  virtual void sendEvent(DummyMsg *);
  virtual void sendEvent(DummyRequestMsg *);
  void sendEvent(newGVTMsg *);
  void sendEvent(CheckIdleMsg *);
  void sendEvent(TerminateMsg *);

  // All of this stuff is to avoid message acknowledgement
  virtual void msgSendPeek( EventMsg *);
  virtual void msgRecvPeek( EventMsg *);

  // these are here to make the compiler happy...
  void simulate();
  BasicEvent* getEvent();
  void sendEvent(BasicEvent *);
  void recvEvent(BasicEvent *);
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
  int gvtPeriod;
  
protected:
  virtual VTime gcollect();

private:
  bool checkIdle();
  void cancelToken(CheckIdleMsg *);
  virtual void passItOn(CheckIdleMsg *);
  void startToken();
  void calcMin();
  void deliver( BasicEvent*); 


  ObjectRecord* simArray;  // This is a pointer to the array of 
                           // simulation objects in the LP
  CommMgrInterface* commHandle; // our handle to our communication 
                           // manager

  VTime* lGVTArray;        // array of gVT estimates for all LPs
                           // only valid for LP 0
  bool* idleArray;

  int definedGVT;          // this is for the "head" GVT guy only -
                           // it is who set gVT last time

  VTime gVT;               // global virtual time
  VTime lGVT;              // our local estimate of gVT

  VTime lastReported;      // the last lGVT value sent to the head
                           // gVT Manager
  bool lastIdle;           // our last idle

  int numObjects;          // the number of objects on our LP
  int totalObjects;        // the total number of simulaiton objects
  int numLPs;              // the number of logical processes

  VTime* lastTimeSent;     // array of the last times of event messages that
                           // I sent to each LP (and are unacknowleged)

  VTime* lastTimeReceived; // array of the last
  VTime lowestUnacknowledged;
  SortedList<AckRecord> needAcked;
  int currentToken;
  bool tokenPending;
  bool *dummyPending;
  int noOfGVTToSkip;

  bool ranBetweenTokens ;
};

#endif // not MATTERNGVTMANAGER
#endif
