//-*-c++-*-
#ifndef COMM_MGR_INTERFACE_HH
#define COMM_MGR_INTERFACE_HH
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
// $Id: CommMgrInterface.hh,v 1.8 1999/10/24 23:25:50 ramanan Exp $
//
//
//---------------------------------------------------------------------------

#ifdef COMMDEBUG
#include <fstream>
#endif

#include "warped.hh"
#include "config.hh"
#include "BasicTimeWarp.hh"
#include "KernelMsgs.hh"

#include "FossilManagerInclude.hh"

#ifdef MESSAGE_AGGREGATION
#include "MessageManager.hh"
#endif

#include "CommPhyInterface.hh"

class LogicalProcess;
class ObjectRecord;
class OfcManager;
class ChebyOfcManager;

class CommMgrInterface : public BasicTimeWarp {
  friend class FOSSIL_MANAGER;
public:

  CommMgrInterface(int numberOfLPs, CommPhyInterface * phyLib);

  ~CommMgrInterface();

  virtual void initCommManager(ObjectRecord*,   // pointer to simulation array
			       FOSSIL_MANAGER*, // pointer to the GVT Manager
			       int,             // The number of LPs
			       LogicalProcess* );   

  int getID() {  return id; };

#ifdef STATS
  void incrementReceivedApplicationMessageCount();
  void incrementReceivedPhysicalMessageCount();
  void incrementSentPhysicalMessageCount();
  void incrementStatsArray(int numberOfMsgs);
#endif

  // identifies the CommMgrInterface(overloading BasicTimeWarp function).
  virtual bool isCommManager(){
    return true;
  }

  // These methods are for the _Commucation Manager_ to receive the types
  // of events passed in ( by local objects )
  void recvEvent( BasicEvent* );
  void recvMsg( TerminateMsg * );

  void recvMsg( newGVTMsg * );
  void recvMsg( LGVTMsg * );
  void recvMsg( LowerLGVTMsg * );
  void recvMsg( GVTAckMsg * );
  void recvMsg( LPAckMsg * );
  void recvMsg( DummyMsg * );
  void recvMsg( DummyRequestMsg * );
  void recvMsg( InitMsg * );
  void recvMsg( StartMsg * );
  void recvMsg( CheckIdleMsg * );
  void recvMsg( RestoreCkptMsg * );
#ifdef MATTERNGVTMANAGER
  void recvMsg( MatternGVTMsg *);
#endif
#ifdef EXTERNALAGENT
  void recvMsg( ExtAgentMsg *);
#endif

  // this is a method the LP calls to let everyone know that he's got a
  // certain simulation object in his posession.
  void iHaveObject(int);


  virtual int recvMPI(int = 1); // wait for the number of messages specified (and
  // deliver them). If there aren't that many 
  // messages, stop receiving them.  In either case,
  // return the number of messages actually received.

  virtual BasicMsg * recvMPIMsg() = 0;
  // grab ONE message from MPI and return it..
  // return NULL if there are none...

  void terminateSimulation( TerminateMsg * );

  SequenceCounter getSequence( int );  // returns the sequence number of the
  // next message to the LP with the id
  // passed in
  void waitForStart();
  void waitForInit( int numExpected );

  // since the general model of memory in this system is that whoever
  // news memory, deletes it, the commanager gets to garbage collect
  // too
  void gcollect(VTime gtime);

  // these are to please the compiler
  void executeProcess();
  BasicEvent* getEvent();
  void simulate();
  
  VTime calculateMin();
#ifdef COMMDEBUG
  void setFile( ofstream * );
#endif
  void deliver(BasicEvent *);

  virtual void flush(){
    cerr << "CommMgrInterface::flush() not implemented" << std::endl;
    abort() ;
  }

private:

  int numLPs;
  ObjectRecord *simArray;

  // we have to define this in order to please the compiler
  void sendEvent( BasicEvent*);

  // These methods are to allow the Communation Manager to deliver messages to
  // local objects only
  void deliver(newGVTMsg *);
  void deliver(CheckIdleMsg *);
#ifdef MATTERNGVTMANAGER
  void deliver(MatternGVTMsg *);
#endif
#ifdef EXTERNALAGENT
  void deliver(ExtAgentMsg *);
#endif

  void deliver(LGVTMsg *);
  void deliver(LowerLGVTMsg *);
  void deliver(LPAckMsg *);
  void deliver(GVTAckMsg *);
  void deliver(DummyMsg *);
  void deliver(DummyRequestMsg *);

  virtual void remoteSend( BasicMsg*, int ) = 0;

protected:
  LogicalProcess *lpHandle;

#ifdef COMMDEBUG
  ofstream *lpFile;
#endif

  //commPhyLib will be newed by some external procedure. But will be deleted
  //from the destructor of this class.
  CommPhyInterface *commPhyLib;
  
  // these will be filled in with arrays so that each communication channel
  // has a unique sequence running
  SequenceCounter * sendSequenceArray;
  SequenceCounter * recvSequenceArray;

  FOSSIL_MANAGER *gVTHandle;

  // this method takes an LP Msg, figures out what kind of derived type it is,
  // and then delivers it to the LOCAL entity that is receiving it.
  void routeMsg( BasicMsg *);

public:
  BasicState* allocateState();
};

extern void printCasted( ostream &, const BasicMsg *);

void physicalCommInit(int *argc, char ***argv);
int  physicalCommGetId();
#endif



