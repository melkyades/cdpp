// Copyright (c) 1994,1995 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.
//
// $Id: main.cc,v 1.2 1998/05/07 22:43:44 gsharma Exp $
//
// Change Log
//---------------------------------------------------------------------------
// $Log: main.cc,v $
// Revision 1.2  1998/05/07 22:43:44  gsharma
// updated the police model to comply with the new state changes.
//
// Revision 1.1  1998/03/07 21:07:24  dmadhava
// Moved the files in the old "warped" directory down by a level. The new
// hierarchy is
//
//       warped
//         |
//         +----- TimeWarp (The files in warped are now here)
//         |
//         +----- Sequential
//         |
//         +----- NoTime
//
// Revision 1.2  1997/07/03 07:19:02  skarthik
// Added the changed definition of SIMUNTIL.
//
// Revision 1.1.1.1  1995/08/14 20:22:55  dmartin
// The warped system.
//
// Revision 1.1  1995/07/27 20:23:26  ramanan
// added police simulation application directory
//
//Revision 1.2  1995/04/27  21:53:39  cyoung
//Major Revision
//
//Revision 1.1  1995/04/25  18:10:12  ramanan
//adding version 1 of queuing model application layer
//
// Revision 1.5  1995/03/17  14:49:16  dmartin
// Preliminary parallel version - works with 20 tokens and 2 machines.
//
// Revision 1.4  1995/02/23  22:09:50  dmartin
// Colored tokens introduced.
//
// Revision 1.3  1995/02/21  20:52:20  dmartin
// Stuff - colored tokens
//
// Revision 1.2  1995/02/20  15:53:58  dmartin
// Changed lots of stuff.
//
// Revision 1.1  1995/02/08  17:48:52  dmartin
// Merged Dale's stuff with Ramanan's.
//
// Revision 1.1.1.1  1995/01/25  21:30:42  dmartin
// The warped system
//
//---------------------------------------------------------------------------
static const char* main_rcsid = "$Id: main.cc,v 1.2 1998/05/07 22:43:44 gsharma Exp $";

#include <iostream.h>

#define MAXTOKENS 200

#include "QueuingTypes.hh"
#include "LogicalProcess.hh"
#include "PoliceSourceObject.hh"
#include "PoliceForkObject.hh"
#include "PoliceSinkObject.hh"
#include "PoliceServerObject.hh"
#include "SourceObject.hh"
#include "ForkObject.hh"
#include "ServerObject.hh"
#include "DelayObject.hh"
#include "SinkObject.hh"

extern "C" {
#include "mpi.h"
}
typedef void (*functionPtr)();
const VTime LogicalProcess::SIMUNTIL = PINFINITY;

main() {
  
  int obj_id[3] ;

 
    LogicalProcess lp(8,8,1);  // total, local, LPs    

    PoliceSourceObject source0(0,"station1",20);
     //source-id, source-name, numberOfCars
    source0.setDistribution(UNIFORM, 5, 100);
    source0.dest = 1;
    lp.registerObject(&source0);
 
    PoliceForkObject fork("CallsReceiver") ;                                            
    obj_id[0] = 2 ;
    obj_id[1] = 3 ;
    obj_id[2] = 4 ;
    fork.setForkDistribution(UNIFORM,3, obj_id);
    fork.id = 1;
    lp.registerObject(&fork);

    PoliceServerObject server0("Operator0");
    server0.setServerDistribution(UNIFORM,0,10);
    server0.id = 2 ;
    server0.dest = 5;
    lp.registerObject(&server0);

 
    PoliceServerObject server1("Operator1");
    server1.setServerDistribution(UNIFORM,0,10);
    server1.id = 3 ;
    server1.dest = 6;
    lp.registerObject(&server1);

    PoliceServerObject server2("Operator2");
    server2.setServerDistribution(UNIFORM,0,10);
    server2.id = 4 ;
    server2.dest = 7;
    lp.registerObject(&server2);

    PoliceSinkObject sink0("PoliceReceiver0");
    sink0.id = 5;
    lp.registerObject(&sink0);

    PoliceSinkObject sink1("PoliceReceiver1");
    sink1.id = 6;
    lp.registerObject(&sink1);

    PoliceSinkObject sink2("PoliceReceiver2");
    sink2.id = 7;
    lp.registerObject(&sink2);


    lp.allRegistered();
    lp.simulate();

 
}



