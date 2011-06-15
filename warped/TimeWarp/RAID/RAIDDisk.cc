//-*-c++-*-
#ifndef RAIDDISK_CC
#define RAIDDISK_CC
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
//
//
// $Id: RAIDDisk.cc,v 1.1 1998/03/07 21:03:41 dmadhava Exp $
//
// 
//
//---------------------------------------------------------------------------

#include <iostream.h>
#include <math.h>
#include "RAIDDisk.hh"
#include "RAIDDiskState.hh"

RAIDDisk::RAIDDisk(int myid,const char* myname, DISK_TYPE theDisk) {
  id = myid;
  name = new char[strlen(myname)+1];
  state->nameOfObject = new char[strlen(myname)+1];
  strcpy(name, myname);
  strcpy(state->nameOfObject,myname);
  
  switch (theDisk) {
  case FUJITSU:
    REVOLUTIONTIME = 11.1;
    MINSEEKTIME = 2;
    AVGSEEKTIME = 11;
    MAXSEEKTIME = 22;
    NUMCYLINDERS = 1944;
    TRACKSPERCYL = 20;
    SECTORSPERTRACK = 88;
    break;
  case FUTUREDISK:
    REVOLUTIONTIME = 9.1;
    MINSEEKTIME = 1.8;
    AVGSEEKTIME = 10;
    MAXSEEKTIME = 20;
    NUMCYLINDERS = 2500;
    TRACKSPERCYL = 20;
    SECTORSPERTRACK = 132;
    break;
  case LIGHTNING:
    REVOLUTIONTIME = 13.9;
    MINSEEKTIME = 2;
    AVGSEEKTIME = 12.5;
    MAXSEEKTIME = 25;
    NUMCYLINDERS = 949;
    TRACKSPERCYL = 14;
    SECTORSPERTRACK = 48;
    break;
  }
}

void
RAIDDisk::executeProcess() {
  
  RAIDRequest *recvEvent;
  
  recvEvent = (RAIDRequest*) getEvent();
       
  if (recvEvent != NULL) {
    RAIDRequest *newEvent;
    newEvent = (RAIDRequest *) new char[sizeof(RAIDRequest)];
    new (newEvent) RAIDRequest();

    *newEvent = *recvEvent; 

    int seekDist;
    double seekTime,a,b,c;
    int nextCylinder = newEvent->startStripe/(SECTORSPERTRACK*TRACKSPERCYL);
    newEvent->startCylinder = nextCylinder;
    newEvent->startSector = newEvent->startStripe%SECTORSPERTRACK;

    seekDist = nextCylinder - ((RAIDDiskState *) state->current)->currentCylinder;
    if ( seekDist < 0) {
      seekDist = abs(seekDist);
    }
    if ( seekDist != 0) {
      a = (-10*MINSEEKTIME + 15*AVGSEEKTIME - 5*MAXSEEKTIME)/
	(3*sqrt(NUMCYLINDERS));
      b = (7*MINSEEKTIME - 15*AVGSEEKTIME + 8*MAXSEEKTIME)/(3*NUMCYLINDERS);
      c = MINSEEKTIME;
      if (a > 0 && b > 0) {
	seekTime = a*sqrt(seekDist -1) + b*(seekDist -1) + c;
      }
      else {
	cerr << "Disk Model parameters are not correct for model" << endl;
	seekTime = 0;
      }
    } 
    else {
      seekTime = 0;
    }

    // Since we may have several events arriving at the same time, we
    // process some events later, but we can determine when since we
    // know how long they will be delayed.
    // If we are working in the future then continue to do so, but if
    // there has been a long enough delay between our last processed
    // message and the current one then use the current time.
    if (((RAIDDiskState *) state->current)->myTime < state->current->lVT) {
      ((RAIDDiskState *) state->current)->myTime = state->current->lVT;
    }
    
    
#ifdef OBJECTDEBUG
    VTime myTimeSave = ((RAIDDiskState *) state->current)->myTime;
    int saveCurrentCylinder = ((RAIDDiskState *) state->current)->currentCylinder;
#endif
   
    ((RAIDDiskState *) state->current)->myTime += int(seekTime + 0.5*REVOLUTIONTIME);
    ((RAIDDiskState *) state->current)->currentCylinder = nextCylinder;
    newEvent->recvTime = ((RAIDDiskState *) state->current)->myTime;
    
#ifdef OBJECTDEBUG
    outFile << "[" << name << "]" << " seeking stripe [ "
	    << newEvent->startStripe << " ] seeking cylinder " 
	    << nextCylinder << " from cylinder "
	    << saveCurrentCylinder << " at " 
	    << myTimeSave << endl << " finished seek at " 
	    << newEvent->recvTime << endl;
    outFile << "Receieved " << *newEvent << "\n";
#endif

    // send event back to source
    newEvent->dest = newEvent->sourceId ;
    newEvent->size = sizeof(*newEvent);

    ((RAIDDiskState *) state->current)->currentCylinder = nextCylinder;
    
    sendEvent(newEvent);
  }
}

BasicState*
RAIDDisk::allocateState() {
  return new RAIDDiskState();
}

#endif
