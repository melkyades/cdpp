#include <iostream.h>
#include <fstream>
#include <string>

#include "Source.hh"
#include "Server.hh"
#include "StatCollector.hh"
#include "Types.hh"
#include "Queue.hh"
#include "ParseInput.hh"

#ifndef SEQUENTIAL
#include "LogicalProcess.hh"
#else
#include "SequentialLP.hh"
#endif

#include "CommMgrInterface.hh"

#ifdef SEQUENTIAL
const VTime SequentialLP::SIMUNTIL = PINFINITY;
#else
//set SIMUNTIL's in Logical Process classes
const VTime LogicalProcess::SIMUNTIL = PINFINITY;
#endif

int 
main(int argc, char **argv) {
  // Read the config File and store all the info in this object
  ParseInput confObj;

  int NUMLPS = confObj.numLP;
  int noServers = confObj.noServers;
  int servs;

  int id = 0;

#ifndef SEQUENTIAL
  physicalCommInit(&argc, &argv);
  id = physicalCommGetId();
#endif

#ifdef SEQUENTIAL
  id = 0;
  NUMLPS = 1;
#endif

  if (id == 0) {
    // This is lp 0
    // Currently we will always have the source and queue object 
    // on this LP
    // Find the number of servers on this LP

    int statObject = 0;
    if (NUMLPS == 1) {
      servs = noServers; 
      statObject = 1;
    } else {
      servs = (noServers + 3)/NUMLPS - 2; 
    }

#ifdef SEQUENTIAL
    SequentialLP lp(noServers+3, servs+2+statObject, NUMLPS);
#else
    LogicalProcess lp(noServers+3, servs+2+statObject, NUMLPS);
#endif

    cout << "Starting simulation for LP 0" << " with ";
    if (NUMLPS == 1) {
      cout << (noServers+3);
    } else {
      cout << (servs+2);
    }
    cout<< " objects " << endl;

    Source *qGen = new Source(0, 1, confObj.customers);
    lp.registerObject(qGen);

    Queue *qObj = new Queue(1, 2, 2+noServers);
    lp.registerObject(qObj);
    
    Server **servers = new Server*[servs];
    for (int i = 0; i < servs; i++) {
      servers[i] = new Server(2+i, 1, 2+noServers);
      lp.registerObject(servers[i]);
    }

    StatCollector *stats;
    if (NUMLPS == 1) {
      stats = new StatCollector(2+noServers, noServers);
      lp.registerObject(stats);
    }

    lp.allRegistered();

    qGen->setDistribution(confObj.srcDist, confObj.srcMean, confObj.srcSD);
    for (int i = 0; i < servs; i++) {
      servers[i]->setDistribution(confObj.serverDist[i], confObj.serverMean[i], confObj.serverSD[i]);
    }
    lp.simulate();
    
    delete qGen;
    delete qObj;

    for (int i = 0; i < servs; i++) {
      delete servers[i];
    }
    delete [] servers;
    if (NUMLPS == 1) {
      delete stats;
    }

  }
#ifndef SEQUENTIAL
  else if (id < NUMLPS-1) {
    int offset;

    if (id == 1) {
      servs = (noServers + 3)/NUMLPS + (noServers + 3)%NUMLPS;
      offset = (noServers + 3)/NUMLPS;
    } else {
      servs = (noServers + 3)/NUMLPS;
      offset = id * ((noServers + 3)/NUMLPS) + (noServers + 3)%NUMLPS;
    }
    LogicalProcess lp(noServers + 3, servs, NUMLPS);
    cout << "\n\nStarting simulation for LP" << id << " with " << servs
	 << " objects " << endl;

    Server **servers = new Server*[servs];
    for (int i = 0; i < servs; i++) {
      servers[i] = new Server(offset+i, 1, 2+noServers);
      lp.registerObject(servers[i]);
    }
    lp.allRegistered();
    for (int i = 0; i < servs; i++) {
      servers[i]->setDistribution(confObj.serverDist[offset-2+i], confObj.serverMean[offset-2+i], confObj.serverSD[offset-2+i]);
    }
    lp.simulate();

    for (int i = 0; i < servs; i++) {
      delete servers[i];
    }
    delete [] servers;

  }else {
    // when id == NUMLPS- 1 (last LP)
    // The stat collector object will be on this LP

    // Find the number of servers in this LP (1 - Stat Collector object)
    servs = (noServers+3)/NUMLPS - 1;
    int offset = 0;

    if (NUMLPS != 2) {
      offset = id * ((noServers + 3) / NUMLPS) + (noServers + 3)%NUMLPS;
    }
    else {
      offset = id * ((noServers + 3) / NUMLPS);
      servs += (noServers + 3) % NUMLPS;
    }
    
    LogicalProcess lp(noServers+3, servs+1, NUMLPS);
    cout << "\n\nStarting simulation for LP" << id << " with " << (servs+1)
	 << " objects " << endl;


    
    Server **servers = new Server*[servs];
    for (int i = 0; i < servs; i++) {
      servers[i] = new Server(offset+i, 1, 2+noServers);
      lp.registerObject(servers[i]);
    }

    StatCollector *stats = new StatCollector(2+noServers, noServers);
    lp.registerObject(stats);

    lp.allRegistered();
    for (int i = 0; i < servs; i++) {
      servers[i]->setDistribution(confObj.serverDist[offset-2+i], confObj.serverMean[offset-2+i], confObj.serverSD[offset-2+i]);
    }
    lp.simulate();

    for (int i = 0; i < servs; i++) {
      delete servers[i];
    }
    delete [] servers;

    delete stats;
  }
#endif

  return 0;
}

