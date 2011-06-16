#include <iostream.h>
#include <fstream>
#include <string>

#include "Node.hh"

#ifndef NO_BUFF
#include "Router.hh"
#else
#include "BuffLessRouter.hh"
#endif

#include "StatCollector.hh"
#include "Types.hh"
#include "ConfigNet.hh"
#ifndef SEQUENTIAL
#include "CommMgrInterface.hh"
#endif

#ifndef SEQUENTIAL
#include "LogicalProcess.hh"
#else
#include "SequentialLP.hh"
#endif

#ifdef SEQUENTIAL
const VTime SequentialLP::SIMUNTIL = PINFINITY;
#else
//set SIMUNTIL's in Logical Process classes
const VTime LogicalProcess::SIMUNTIL = PINFINITY;
#endif

int 
main(int argc, char **argv) {
  // Read the config File inot this object
  ConfigNet *parseConfig = new ConfigNet();
  int NUMLPS = parseConfig->numLP;
  int numNodes, numRouters, offset, i, j;
  int totObjects = parseConfig->noNodes + parseConfig->noRouters + 1;

  int id = 0;

#ifndef SEQUENTIAL
  physicalCommInit(&argc, &argv);
  id = physicalCommGetId();
#endif

  if (id == 0) {
    if (NUMLPS == 1) {
      numNodes = parseConfig->noNodes;
      numRouters = parseConfig->noRouters;
    } else {
      numNodes = parseConfig->noNodes/NUMLPS;
      numRouters = parseConfig->noRouters/NUMLPS;
    }

#ifdef SEQUENTIAL
    SequentialLP lp(totObjects, numNodes+numRouters+1, NUMLPS);
#else
    LogicalProcess lp(totObjects, numNodes+numRouters+1, NUMLPS);
#endif

    cout << "Starting simulation for LP 0" << " with ";
    cout << (numNodes+numRouters+1) << " objects \n";
    
#ifndef NO_BUFF
    Router **routers = new Router*[numRouters];
    for (i = 0; i < numRouters; i++) {
      j = parseConfig->routerTable[i].id - 1;
      routers[i] = new Router(j,  parseConfig, totObjects-1);
      lp.registerObject(routers[i]);
    }
#else 
    BuffLessRouter **routers = new BuffLessRouter*[numRouters];
    for (i = 0; i < numRouters; i++) {
      j = parseConfig->routerTable[i].id - 1;
      routers[i] = new BuffLessRouter(j,  parseConfig, totObjects-1);
      lp.registerObject(routers[i]);
    }
#endif

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

  } 
#ifndef SEQUENTIAL
  else if (id < NUMLPS-1) {
    numNodes = parseConfig->noNodes/NUMLPS;
    numRouters = parseConfig->noRouters/NUMLPS;

    LogicalProcess lp(totObjects, numNodes+numRouters, NUMLPS);
    cout << "Starting simulation for LP " << id << " with ";
    cout << (numNodes+numRouters) << " objects \n";
    
    offset = id * numRouters;

#ifndef NO_BUFF    
    Router **routers = new Router*[numRouters];
    for (i = 0; i < numRouters; i++) {
      j = parseConfig->routerTable[i+offset].id - 1;
      routers[i] = new Router(j,  parseConfig, totObjects-1);
      lp.registerObject(routers[i]);
    }
#else 
    BuffLessRouter **routers = new BuffLessRouter*[numRouters];
    for (i = 0; i < numRouters; i++) {
      j = parseConfig->routerTable[i+offset].id - 1;
      routers[i] = new BuffLessRouter(j,  parseConfig, totObjects-1);
      lp.registerObject(routers[i]);
    }
#endif

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

#ifndef NO_BUFF
    Router **routers = new Router*[numRouters];
    for (i = 0; i < numRouters; i++) {
      j = parseConfig->routerTable[i+offset].id - 1;
      routers[i] = new Router(j,  parseConfig, totObjects-1);
      lp.registerObject(routers[i]);
    }
#else
    BuffLessRouter **routers = new BuffLessRouter*[numRouters];
    for (i = 0; i < numRouters; i++) {
      j = parseConfig->routerTable[i+offset].id - 1;
      routers[i] = new BuffLessRouter(j,  parseConfig, totObjects-1);
      lp.registerObject(routers[i]);
    }
#endif

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
#endif
  delete parseConfig;
  return 0;
}
