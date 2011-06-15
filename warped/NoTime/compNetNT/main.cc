#include <iostream.h>
#include <fstream.h>
#include <string.h>

#include "Node.hh"
#include "Router.hh"
#include "StatCollector.hh"
#include "Types.hh"
#include "ConfigNet.hh"

#include "LogicalProcess.hh"

#ifdef MPI
extern "C" {
#include "mpi.h"
}
#endif

//set SIMUNTIL's in Logical Process classes
const VTime LogicalProcess::SIMUNTIL = PINFINITY;

int 
main(int argc, char **argv) {
  // Read the config File inot this object
  ConfigNet *parseConfig = new ConfigNet();
  int NUMLPS = parseConfig->numLP;
  int numNodes, numRouters, offset, i, j;
  int totObjects = parseConfig->noNodes + parseConfig->noRouters + 1;

  int id = 0;
#ifdef MPI
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &id);
#endif

  if (id == 0) {
    if (NUMLPS == 1) {
      numNodes = parseConfig->noNodes;
      numRouters = parseConfig->noRouters;
    } else {
      numNodes = parseConfig->noNodes/NUMLPS;
      numRouters = parseConfig->noRouters/NUMLPS;
    }
    LogicalProcess lp(totObjects, numNodes+numRouters+1, NUMLPS);
    cout << "Starting simulation for LP 0" << " with ";
    cout << (numNodes+numRouters+1) << " objects \n";
    
    Router **routers = new Router*[numRouters];
    for (i = 0; i < numRouters; i++) {
      j = parseConfig->routerTable[i].id - 1;
      routers[i] = new Router(j,  parseConfig, totObjects-1);
      lp.registerObject(routers[i]);
    }

    Node **nodes = new Node*[numNodes];
    for (i = 0; i < numNodes; i++) {
      j = parseConfig->getRouter(i+1);
      nodes[i] = new Node(i, parseConfig, j, totObjects-1);
      lp.registerObject(nodes[i]);
    }
    
    StatCollector *statObj = new StatCollector(totObjects-1);
    lp.registerObject(statObj);

    lp.allRegistered();
    lp.simulate();

    for (i = 0; i < numRouters; i++) {
      delete routers[i];
    }
    delete [] routers;

    for (i = 0; i < numNodes; i++) {
      delete nodes[i];
    }
    delete [] nodes;
    
    delete statObj;

  } else if (id < NUMLPS-1) {
    numNodes = parseConfig->noNodes/NUMLPS;
    numRouters = parseConfig->noRouters/NUMLPS;

    LogicalProcess lp(totObjects, numNodes+numRouters, NUMLPS);
    cout << "Starting simulation for LP " << id << " with ";
    cout << (numNodes+numRouters) << " objects \n";
    
    offset = id * numRouters;
    Router **routers = new Router*[numRouters];
    for (i = 0; i < numRouters; i++) {
      j = parseConfig->routerTable[i+offset].id - 1;
      routers[i] = new Router(j,  parseConfig, totObjects-1);
      lp.registerObject(routers[i]);
    }

    offset = id * numNodes;
    Node **nodes = new Node*[numNodes];
    for (i = 0; i < numNodes; i++) {
      j = parseConfig->getRouter(i+offset+1);
      nodes[i] = new Node(i+offset, parseConfig, j, totObjects-1);
      lp.registerObject(nodes[i]);
    }
    
    lp.allRegistered();
    lp.simulate();

    for (i = 0; i < numRouters; i++) {
      delete routers[i];
    }
    delete [] routers;

    for (i = 0; i < numNodes; i++) {
      delete nodes[i];
    }
    delete [] nodes;

  } else {   // The last LP
    numNodes = parseConfig->noNodes/NUMLPS + parseConfig->noNodes%NUMLPS;
    numRouters = parseConfig->noRouters/NUMLPS + parseConfig->noRouters%NUMLPS;

    LogicalProcess lp(totObjects, numNodes+numRouters, NUMLPS);
    cout << "Starting simulation for LP " << id << " with ";
    cout << (numNodes+numRouters) << " objects \n";
    
    offset = id * (parseConfig->noRouters/NUMLPS);
    Router **routers = new Router*[numRouters];
    for (i = 0; i < numRouters; i++) {
      j = parseConfig->routerTable[i+offset].id - 1;
      routers[i] = new Router(j,  parseConfig, totObjects-1);
      lp.registerObject(routers[i]);
    }

    offset = id * (parseConfig->noNodes/NUMLPS);
    Node **nodes = new Node*[numNodes];
    for (i = 0; i < numNodes; i++) {
      j = parseConfig->getRouter(i+offset+1);
      nodes[i] = new Node(i+offset, parseConfig, j, totObjects-1);
      lp.registerObject(nodes[i]);
    }
    
    lp.allRegistered();
    lp.simulate();
    for (i = 0; i < numRouters; i++) {
      delete routers[i];
    }
    delete [] routers;

    for (i = 0; i < numNodes; i++) {
      delete nodes[i];
    }
    delete [] nodes;
  }
  delete parseConfig;
}
