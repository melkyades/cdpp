//-*-c++-*-
#ifndef DYNAMICCANCELLATION_HH
#define DYNAMICCANCELLATION_HH
// Copyright (c) 1994,1995 Ohio Board of Regents and the University of
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
// $Id: DynamicCancellation.hh,v 1.5 1999/10/24 23:25:53 ramanan Exp $
//
//
//---------------------------------------------------------------------------

#include <fstream>
#include "warped.hh"
#include "config.hh"

#if defined(LAZYCANCELLATION) || defined(LAZYAGGR_CANCELLATION)

#include "DefaultVTime.hh"
#include "TimeWarp.hh"

// For lazy cancellation
#include "OutputQueue.hh"

#define FILTER_DEPTH 16
#define PERMANENTSETVAL 32 // Used to set the cancellation mode permanently

#define AGGRESSIVE_TO_LAZY 0.5
#define LAZY_TO_AGGRESSIVE 0.2
#define INDEX hitRatio
#define THIRD_THRESHOLD 0.1

class BasicEvent;

class LazyAggrTimeWarp : public TimeWarp {

enum CancellationMode {AGGRESSIVE, LAZY};

public:
 LazyAggrTimeWarp();
 ~LazyAggrTimeWarp();
 void executeSimulation();
 bool suppressMessageTest(BasicEvent *toSend);
 void checkForLazyAggrHit(BasicEvent *toSend);
 void cancelMessages(const VTime rollbackTime);
 bool lazyCancel(BasicEvent *toSend);
 void setCancellationMode();
  
 // Use this method to fix the cancellation mode permanently to whichever 
 // mode you want. Call this immediately after the constructor is called
 // for the simulation object.
 void fixCancellationMode(CancellationMode modeToBeFixedTo) {
     currentMode = modeToBeFixedTo;
 }
 void determineMode();

#if defined(LAZYAGGR_CANCELLATION) && defined(STATS)
 ofstream dcStatsFile;
 void openDCStatsFile(char* dcStatsFilename) {
    dcStatsFile.open(dcStatsFilename);
 }
#endif

#ifdef PERMANENTLYSET
 const int measurementCycle ;
#else
 const int measurementCycle ;
#endif

private:

 float hitRatio;        // the Index of Performance
 int hitCount;          // total number of "hits" in the measurement cycle
 int missCount;         // total number of "misses" in the measurement cycle
 int noOfComparisions;  // total number of comparisions in one cycle
 bool lazyHit;          // if true then it is a lazy hit
 bool lazyAggrHit;      // if true then it is a lazy aggressive hit
 CancellationMode currentMode; // Cancellation strategy currently being used
 int *comparisonResults ;

#ifdef PERMANENTLYSET
 // This flag determines when the cancellation strategy can be permanently set.
 bool modePermanent;    
#endif

#ifdef PERMANENTLY_AGGR
 // Aggressive cancellation is used throughout the simulation when this 
 // flag is true.
 bool permanentlyAggressive;
#endif

 OutputQueue lazyAggrQ; 

};
#endif
#endif



