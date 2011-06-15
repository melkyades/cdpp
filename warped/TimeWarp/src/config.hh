//-*-c++-*-
// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.
//
// BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
// FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT
// WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER
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
// $Id: config.hh,v 1.26 1999/10/24 23:26:28 ramanan Exp $
//
//
//---------------------------------------------------------------------------

//----------------------------------------------------------------------------
//The following section allows the configuration of the warped kernel.  In
//each section, several choices are available.  For each category, two
//symbols must be defined.  The first symbol must be defined to be the
//name of the class being instantiated.  The second symbol is the class
//name, all in uppercase.  This is to allow the appropriate header files
//to be included.
//----------------------------------------------------------------------------


// The different schedulers we support.  Supported schedulers:
// we only support the LTSFScheduler. Other schedulers (such as the Round 
// Robin schedulers have been discontinued. 

// Uncomment the following define for turning on "message aggregation"
// optimization
// #define MESSAGE_AGGREGATION

// if STATS is defined, then this flag gives the user the option of either
// viewing the stats results in a file or on the screen
// Setting FILE_OUTPUT to zero => output printed to stdout
// Setting FILE_OUTPUT to one  => output printed to file

//#define STATS 
#ifdef STATS
#define FILE_OUTPUT 0
#ifdef MESSAGE_AGGREGATION
// define MAX_AGGREGATE_SIZE to see the different aggregates sizes that are
// accumulated during the simulation
#define MAX_AGGREGATE_SIZE 500
#endif
#endif

#ifdef MESSAGE_AGGREGATION
//Uncomment the aggregation strategy that you want
//The default strategy is SAAW, also called MEAN_OF_FACTORS
//WARNING Two strategies should not be enabled
#define MEAN_OF_FACTORS
//#define FIXED_PESSIMISM
//#define FIXED_MSG_COUNT
//#define PROBE_SEND_RECEIVE_MESSAGES
//#define ADAPTIVE_AGGREGATION
#endif

// The different state managers we support.
// Supported State Managers: StateManager, InfreqStateManager, LinStateManager,
// NashStateManager, CostFuncStateManager

#define STATE_MANAGER StateManager
#define STATEMANAGER

// The different fossil collection managers we support.
// Supported fossil managers: GVTManager and MatternGVTManager

#define FOSSIL_MANAGER GVTManager
#define GVTMANAGER

// The following defines control infrequent polling of the Message
// Communication Subsystem.
// For using infrequent define INFREQ_POLLING and then define ONE of
// INFREQ_STATIC or INFREQ_FXD_INCREMENT or INFREQ_VAR_INCREMENT.
// To get traces define INFREQ_TRACE and/or MCS_TRACE

// #define INFREQ_POLLING
// #define INFREQ_STATIC
// #define INFREQ_FXD_INCREMENT
// #define INFREQ_VAR_INCREMENT
// #define INFREQ_TRACE
// #define MCS_TRACE


// The different Memory Manager Supported are
// GlobalMemoryManager BuddyMemoryManager SegregatedMemAlloc BrentMemAlloc
// #define MEMORY_MANAGER BrentMemAlloc
// #define BRENTMEMALLOC

// Uncomment the following define for turning on "one anti message"
// optimization
// #define ONE_ANTI_MESSAGE

// The default cancellation strategy is Aggressive Cancellation
// Uncomment the following define for switching to Lazy Cancellation
// if you wish to dynamically switch between the two cancellation
// strategies, then uncomment the LAZYAGGR_CANCELLATION define.

// #define LAZYCANCELLATION
// #define LAZYAGGR_CANCELLATION

// This is for the external control optimization
// #define EXTERNALAGENT
// #define MAX_EVENTS_TO_WAIT 3000



// The different physical communication mechanisms:
// These following ICM Strategies work only with
// TCPMPL (TCP Message passing library)
// They do not work with MPICH.
// #define CLUMPS_ICM
// #define CLUMPS_ICM_IRCM
// #define CLUMPS_ICM_ISCM
// #define CLUMPS_ICM_CCM
// #define CLUMPS_ICM_AGGR_ISCM

#ifdef CLUMPS_ICM
#ifdef CLUMPS_ICM_IRCM
#define COMMPHYSICAL CommPhyClumpsIRCM
#elif defined(CLUMPS_ICM_ISCM)
#define COMMPHYSICAL CommPhyClumpsISCM
#elif defined(CLUMPS_ICM_AGGR_ISCM)
#define COMMPHYSICAL CommPhyClumpsAggrISCM
#else
#define COMMPHYSICAL CommPhyClumpsCCM
#endif 
#endif


//#define CLUMPS_DPC
//#define CLUMPS_DPC_REG_RETR
//#define CLUMPS_DPC_INFREQ_RETR

#ifdef CLUMPS_DPC
#ifdef CLUMPS_DPC_REG_RETR
#define COMMPHYSICAL CommPhyClumpsDPC
#elif defined(CLUMPS_DPC_INFREQ_RETR)
#define COMMPHYSICAL CommPhyClumpsDPC
#endif
#endif


#if !defined(CLUMPS_ICM) && !defined(CLUMPS_DPC)

#ifdef MPI
#define COMMPHYSICAL CommPhyMPI
#elif MPINONBLOCK
#define COMMPHYSICAL CommPhyMPINonBlock
#elif TCPLIB
#define COMMPHYSICAL CommPhyTCPMPL
#else
#define COMMPHYSICAL CommPhyDefault
#endif

#endif /* #ifndef CLUMPS_ICM */


// The different optimizations at the communication manager level
#ifdef MESSAGE_AGGREGATION
#define COMMMANAGER CommMgrMsgAgg
#else
#define COMMMANAGER CommMgrDefault
#endif


// Diplays the simulation configuration i.e. which optimizations/algorithms
// are being used for that simulation run. The configuration is displayed
// at the begining of the simulation

// #define DISPLAY_SIM_CONFIG


#ifndef ASSERT
#ifdef DEVELOPER_ASSERTIONS
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif
#endif

//----------------------------------------------------------------------------

// This part is related to the Cancellation strategies. The default is
// Aggressive cancellation
#ifdef LAZYCANCELLATION
// This moves the messages to the lazyCancelQ during lazy cancellation
#define CANCEL_MESSAGES(rollbackTime) moveMessagesToLazyQueue(rollbackTime)
#define FILL_EVENT_INFO(eventPtr) fillEventInfo(eventPtr) 
#define LAZY_CANCEL_TEST(eventPtr)  suppressLazyMessages(eventPtr)

#elif defined(LAZYAGGR_CANCELLATION)

#if defined(PERMANENTLY_AGGR) && defined(PERMANENTLYSET)
#error The PERMANENTLY_AGGR and PERMANENTLYSET flags are mutually exclusive!
#endif

// Here, cancelMessages() of LazyAggrTimeWarp is called
#define CANCEL_MESSAGES(rollbackTime) cancelMessages(rollbackTime)
#define FILL_EVENT_INFO(eventPtr) fillEventInfo(eventPtr)
#define LAZY_CANCEL_TEST(eventPtr)  suppressMessageTest(eventPtr)
#else
// This dispatches the messages rightaway when it receives a straggler
#define CANCEL_MESSAGES(rollbackTime) cancelMessagesAggressively(rollbackTime)
#define FILL_EVENT_INFO(eventPtr) fillEventInfo(eventPtr)
#define LAZY_CANCEL_TEST(eventPtr)  false

#endif

