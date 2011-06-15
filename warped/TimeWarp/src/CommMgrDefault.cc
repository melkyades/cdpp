//-*-c++-*-
#ifndef COMM_MGR_DEFAULT_CC
#define COMM_MGR_DEFAULT_CC
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
// $Id: CommMgrDefault.cc,v 1.3 1998/10/17 19:19:26 mal Exp $
//
//---------------------------------------------------------------------------

#include "CommMgrDefault.hh"
#include "LogicalProcess.hh"
#include "LogicalProcessStats.hh"

CommMgrDefault::CommMgrDefault(int numberOfLPs, CommPhyInterface *phyLib) : CommMgrInterface(numberOfLPs, phyLib) {
}

CommMgrDefault::~CommMgrDefault(){
}

BasicMsg *
CommMgrDefault::recvMPIMsg(){

  BasicMsg *newMsg = NULL;
  newMsg = (BasicMsg *)commPhyLib->physicalProbeRecv();
  if ( newMsg != NULL ){
#ifdef COMMDEBUG
    *lpFile << "CommMgrDefault received : (" << newMsg << ") ";
    printCasted(*lpFile, newMsg);
#endif

#ifdef STATS
    lpHandle->lpStats.incrementReceivedApplicationMessageCount();
    lpHandle->lpStats.incrementReceivedPhysicalMessageCount();
#endif
  }
  return newMsg;
}

void
CommMgrDefault::remoteSend(BasicMsg *msg, int size){
#ifdef COMMDEBUG
  *lpFile << "sending : ";
  printCasted(*lpFile, msg);
#endif

#ifdef STATS
  lpHandle->lpStats.incrementSentApplicationMessageCount();
#endif

  commPhyLib->physicalSend((char*)msg, size, msg->destLP);

#ifdef STATS
  lpHandle->lpStats.incrementSentPhysicalMessageCount();
#endif
}

#endif // #ifndef COMM_MGR_DEFAULT_CC
