//-*-c++-*-
#ifndef BASICTIMEWARP_CC
#define BASICTIMEWARP_CC
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
// $Id: BasicTimeWarp.cc,v 1.6 1999/10/24 23:25:45 ramanan Exp $
//
//
//---------------------------------------------------------------------------

#include "BasicTimeWarp.hh"

LTSFInputQueue BasicTimeWarp::inputQ;

BasicTimeWarp::BasicTimeWarp()  {
  iRanLastTime = false;
  localId = 0 ;
#ifdef MATTERNGVTMANAGER
  color                = 0; // initialized to white
  currentColorMessages = 0;
  redMessages          = 0;
  whiteMessages        = 0;
  tMin                 = ZERO;
#endif
  name                 = NULL;
}

void
BasicTimeWarp::openFile() {
  outFile.open(name);
}

void
BasicTimeWarp::setFile(ofstream *) {
  cerr << name << ": Error! BasicTimeWarp::setFile called!" << endl;
  abort();
}

void
BasicTimeWarp::sendNegEvent(BasicEvent*) {
  cerr << name << ": Error! BasicTimeWarp::sendNegEvent called!" << endl;
  abort();
}

void
BasicTimeWarp::setCommHandle(ObjectRecord *tempHandle) {
  commHandle = tempHandle;
  outputQ.simArray = tempHandle;
}

void
BasicTimeWarp::setLPHandle(LogicalProcess* ) {
  cerr << name << ": Error! BasicTimeWarp::setLPHandle called!" 
       << " for object with id " << id << endl;
  abort();
}


LogicalProcess*
BasicTimeWarp::getLPHandle() const {
  cerr << name << ": Error! BasicTimeWarp::getLPHandle() called!"
       << " for object with id " << id << endl;
  abort();
  
  //this is to keep CC from reporting error
  return NULL;
}

VTime
BasicTimeWarp::getLVT() const {
  cerr << name << ": Error! BasicTimeWarp::getLVT called!" << endl;
  abort();
  return ZERO;
}

void
BasicTimeWarp::executeSimulation() {
  cerr << name << ": Error! BasicTimeWarp::executeSimulation called!" 
       << " for object with id " << id << endl;
  abort();
}

int
BasicTimeWarp::inputGcollect(VTime gtime, BasicEvent* inputQptr) {
  cerr << name << ": Error! BasicTimeWarp::inputGcollect called with ptr " 
       << inputQptr << " at time "  << gtime << endl;
  abort();
  return 0;
}

void
BasicTimeWarp::outputGcollect(VTime gtime, 
			      Container<BasicEvent>*& outputQptr) {
  cerr << name << ": Error! BasicTimeWarp::outputGcollect called with ptr " 
       << outputQptr << " at time " << gtime << endl;
  abort();
}

void
BasicTimeWarp::outputGcollect(VTime gtime){
  cerr << name << ": Error! BasicTimeWarp::outputGcollect called at time" 
       << gtime << endl;
  abort();
}

VTime
BasicTimeWarp::getGVT() {
  cerr << name << ": Error! BasicTimeWarp::getGVT called!" << endl;
  abort();
  return ZERO;
}

bool
BasicTimeWarp::withinTimeWindow(){
  cerr << "called withinTimeWindow in BasicTimeWarp!" << endl;
  abort();
  return false;
}

void
BasicTimeWarp::simulate() {
  cerr << name << ": Error! BasicTimeWarp::simulate called!" << endl;
  abort();
}

void
BasicTimeWarp::saveState() {
  cerr << name << ": Error! BasicTimeWarp::saveState called!" << endl;
  abort();
}

void
BasicTimeWarp::coastForward(VTime) {
  cerr << name << ": Error! BasicTimeWarp::coastForward called!" << endl;
  abort();
}

void
BasicTimeWarp::cancelMessages(const VTime) {
  cerr << name << ": Error! BasicTimeWarp::cancelMessages called!" << endl;
  abort();
}
  
VTime
BasicTimeWarp::stateGcollect(VTime gtime){
  cerr << name << ": Error! BasicTimeWarp::stateGcollect called at time" 
       << gtime << endl;
  abort();
  return ZERO;
}
  
VTime
BasicTimeWarp::stateGcollect(VTime gtime, BasicEvent*& inputPtr,
			     Container<BasicEvent>*& outputptr) {
  cerr << name << ": Error! BasicTimeWarp::stateGcollect called at time" 
       << gtime << " with Pointers " << inputPtr << "and" 
       << outputptr << endl;
  abort();
  return ZERO;
}

void
BasicTimeWarp::stateClear(){
  cerr << name << ": Error! BasicTimeWarp::stateClear called!" << endl;
  abort();
}

void
BasicTimeWarp::clearInitState(){
  cerr << name << ": Error! BasicTimeWarp::clearInitState called!" 
       << endl;
  abort();
}

#ifdef STATEDEBUG

void
BasicTimeWarp::setStateFile(ofstream *) {
  cerr << name << ": Error! BasicTimeWarp::setStateFile called!" << endl;
  abort();
}

#endif

void
BasicTimeWarp::setSchedulerHandle(LTSFScheduler* myScheduler) { 
  schedulerHandle = myScheduler;
}

#if defined(LAZYCANCELLATION) || defined(LAZYAGGR_CANCELLATION)

void
BasicTimeWarp::openLCFile(char*) {
  cerr << name << ": Error! BasicTimeWarp::openLCStatsFile called!." << endl;
  exit(-18);
}

void
BasicTimeWarp::openDCStatsFile(char*) {
  cerr << name << ": Error! BasicTimeWarp::openDCStatsFile called!." << endl;
  exit(-19);
}

// currently used for GVT calculations
VTime
BasicTimeWarp::getLazyQMinTime(){
  cout << "getLazyQMinTime called here" << endl;
  return PINFINITY;
}
  
int
BasicTimeWarp::getLazyQSize(){
  cout << "getLazyQSize called here" << endl;
  return -1;
}

#endif // end of LAZYCANCELLATION or LAZYAGGR_CANCELLATION define

void
BasicTimeWarp::rollbackFileQueues(VTime) {
  cerr << name 
       << ": Error! BasicTimeWarp::rollbackFileQueues called" 
       << endl;
  
}


ostream& operator<< (ostream& os, const BasicTimeWarp& btw) {
  BasicEvent *proc;

  os << "id(" << btw.id << ")  lvt["
     << btw.getLVT() << "] ts<" ;
  proc = btw.inputQ.get();
  if (proc == NULL) {
    os << "NULL";
  } else {
    os << proc->recvTime;
  }
  os <<  btw.name;
  return os;
}


void
BasicTimeWarp::deAllocateState(BasicState *) {
  cerr << name
       << ": Error! BasicTimeWarp::allocateState() called" << endl;
  cerr << "Maybe new style of state saving is not fully implemented yet"
       << endl;
  abort();
}

BasicState*
BasicTimeWarp::getCurrentState() const{
  cerr << name
       << ": Error! BasicTimeWarp::getCurrentState() called" << endl;
  cerr << "Maybe new style of state saving is not fully implemented yet"
       << endl;
  abort();
  
  return NULL;
}

void
BasicTimeWarp::timeWarpInit() {
  cerr << name
       << ": Error! BasicTimeWarp::timeWarpInit() called" << endl;
  cerr << "Maybe new style of state saving is not fully implemented yet"
       << endl;
  abort();
}
  
#endif




