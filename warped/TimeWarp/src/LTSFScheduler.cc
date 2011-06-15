#ifndef LTSFSCHEDULER_CC
#define LTSFSCHEDULER_CC
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
// $Id: LTSFScheduler.cc,v 1.4 1999/01/29 16:03:46 mal Exp $
//
//
//---------------------------------------------------------------------------
#include "LTSFScheduler.hh"

// The above is needed to keep this from compiling when another scheduler 
// is defined since this assumes different stuff in basic event.
void LTSFScheduler::initialize(LTSFInputQueue *qPtr, ObjectRecord *objPtr, 
			       int idParam){
 queuePtr = qPtr;
 ObjArray = objPtr;
 id = idParam;
}


void LTSFScheduler::runProcesses(){
#ifdef LPDEBUG
  int newid;
  newid = (queuePtr->get())->dest;
  *outFile << "Scheduler running: " << newid << " at " 
	   << ObjArray[newid].ptr->getLVT() << "\n";
#endif

  ObjArray[(queuePtr->get())->dest].ptr->executeSimulation();
	
}
#endif // LTSFSCHEDULER_CC
