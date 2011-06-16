#include "ConfigNet.hh"
#include <string>

ConfigNet::ConfigNet() {
  configFile.open("Net.config");

  if (configFile.good()) {
    configFile >> noNodes;
    readRouterInfo();
    readTrafficInfo();
    readServTimeInfo();
    readDelayInfo();
    configFile >> numLP;
  } else {
    cerr << "Couldn't open the config File \n";
  }
  configFile.close();
}

ConfigNet::~ConfigNet() {
  delete [] node;
  delete [] nodeMean;
  delete [] nodeVar;
  delete [] nodeDist;
  delete [] routerTable;
}

void
ConfigNet::readRouterInfo() {
  int i, node, router, j;
  char limiter;
  int totNodes;

  configFile >> noRouters;
  routerTable = new RouterInfo[noRouters];

  // Initialize the router information
  for (i = 0; i < noRouters; i++) {
    routerTable[i].startNode = -1;
    routerTable[i].endNode = -1;
    routerTable[i].connRouter = 0;
  }
  totNodes = noNodes + noRouters;

  // Read in the router id's
  for (configFile >> limiter; limiter != '('; configFile >> limiter);
  configFile >> router;
  routerTable[0].id = router;
  for (i = 1; i < noRouters; i++) {
    configFile >> limiter;
    if (limiter == ',') {
      configFile >> router;
      routerTable[i].id = router;
    } else if (limiter != ')') {
      // Syntax error
      cout << "Syntax Error in line \n";
      abort();
    }
  }
  for (configFile >> limiter; limiter != ')'; configFile >> limiter);

  // Read the router connections
  while ((limiter = configFile.get()) != 'x') {
    configFile.unget();
    configFile >> router;
    for (i = 0; i < noRouters; i++) {
      if (routerTable[i].id == router) {
	break;
      }
    }
    if (i >= noRouters) {
      cerr << "Router with illegal id(" << router << ") found\n";
      abort();
    }

    for (configFile >> limiter; limiter != '('; configFile >> limiter);
    
    // The nodes that are connected to this router
    //a..b a = startnode and b = endNode
    configFile >> node;
    routerTable[i].startNode = node;
    if ((limiter = configFile.get()) == '.') {
      configFile >> limiter;
      configFile >> node;
      routerTable[i].endNode = node;
      configFile >> limiter;
    }
    
    // The routers that are connected to this router
    while (limiter != ')') {
      configFile >> node;
      for (j = 0; j < noRouters; j++) {
	if (routerTable[j].id == node) {
	  break;
	}
      }
      if (j < noRouters) {
	routerTable[i].connRouter = routerTable[i].connRouter | (1 << j);
      } else {
	cerr << "Router with illegal id(" << node << ") found\n";
	abort();
      }
      configFile >> limiter;
    }
  }
}

DistType
ConfigNet::findDistr(char *distrib) {
  DistType retVal;

  if (!strcasecmp(distrib, "uniform")) {
    retVal = UNIFORM;
  } else if (!strcasecmp(distrib, "poisson")) {
    retVal = POISSON;
  } else if (!strcasecmp(distrib, "exponential")) {
    retVal = EXPONENTIAL;
  } else if (!strcasecmp(distrib, "normal")) {
    retVal = NORMAL;
  } else if (!strcasecmp(distrib, "binomial")) {
    retVal = BINOMIAL;
  }
  return retVal;
}

void
ConfigNet::readTrafficInfo() {
  int iter = noNodes, syntax, lVar, lMean, i, j;
  char distr[20];
  DistType typ;
  char separ;
  nodeDist = new DistType[noNodes];
  nodeMean = new int[noNodes];
  nodeVar = new int[noNodes];
  node = new int[noNodes];

  while (iter > 0) {
    iter--;
    i = 0; syntax = 0;
    
    configFile >> node[i];
    while ((separ = configFile.get()) == ' ');
    
    // a, b, c 
    while (separ == ',') {
      iter--; 
      configFile >> node[++i];
      while ((separ = configFile.get()) == ' ');
    } 

    // a..b
    if (separ == '.') {
      configFile.get();
      configFile >> node[++i];
      iter = iter - (node[i] - node[i-1]);
      while ((separ = configFile.get()) == ' ');
      syntax = 1;
    }
    
    if (iter < 0) {
      cerr << "Config file ERROR : More servers than specified \n";
      abort();
    }
    
    configFile.unget();
    // Read the distribution, mean and SD of these servers
    configFile >> distr >> lMean >> lVar;
    typ = findDistr(distr);
    if (syntax != 1) {
      for (j = 0; j <= i; j++) {
	if (node[j] < noNodes) {
	  nodeDist[node[j]-1] = typ;
	  nodeMean[node[j]-1] = lMean;
	  nodeVar[node[j]-1] = lVar;
	} else {
	  cerr << "Config File ERROR : Node not " << node[j] << " found \n";
	  abort();
	}
      }
    } else {
      for (j = node[0]; j <= node[1]; j++) {
	if (j <= noNodes) {
	  nodeDist[j-1] = typ;
	  nodeMean[j-1] = lMean;
	  nodeVar[j-1] = lVar;
	} else {
	  cerr << "Config File ERROR : Node not " << j << " found \n";
	  abort();
	}
      }
    }
  }
  configFile >> totMess;
  delete [] node;
}

void
ConfigNet::readServTimeInfo() {
  int iter = noRouters, syntax, i, lMean, lVar, j;
  char separ, distr[20];
  DistType typ;
  int routerId;
  node = new int[noRouters];

  while (iter > 0) {
    iter--;
    i = 0; syntax = 0;
    
    configFile >> node[i];
    while ((separ = configFile.get()) == ' ');
    
    // a, b, c 
    while (separ == ',') {
      iter--; 
      configFile >> node[++i];
      while ((separ = configFile.get()) == ' ');
    } 
    
    // a..b
    if (separ == '.') {
      configFile.get();
      configFile >> node[++i];
      iter = iter - (node[i] - node[i-1]);
      while ((separ = configFile.get()) == ' ');
      syntax = 1;
    }
    
    if (iter < 0) {
      cerr << "Config file ERROR : More servers than specified \n";
      abort();
    }
    
    configFile.unget();
    // Read the distribution, mean and var of these servers
    configFile >> distr >> lMean >> lVar;
    typ = findDistr(distr);
    if (syntax != 1) {
      for (j = 0; j <= i; j++) {
	int k;
	for (k = 0; (routerTable[k].id != node[j]-1 && k < noRouters); k++);
	if (k < noRouters) {
	  routerTable[k].dist = typ;
	  routerTable[k].mean = lMean;
	  routerTable[k].var = lVar;
	} else {
	  cerr << "Config File ERROR : Node not " << node[j] << " found \n";
	  abort();
	}
      }
    } else {
      for (j = node[0]; j <= node[1]; j++) {
	int k;
	for (k = 0; (routerTable[k].id != j && k < noRouters); k++);
	if (k < noRouters) {
	  routerTable[k].dist = typ;
	  routerTable[k].mean = lMean;
	  routerTable[k].var = lVar;
	} else {
	  cerr << "Config File ERROR : Node not " << j << " found \n";
	  abort();
	}
      }
    }
  }
  configFile >> bufferSize;
}

void
ConfigNet::readDelayInfo() {
  configFile >> LANDelay;
  configFile >> WANDelay;
}

int
ConfigNet::getRouter(int nodeId) {
  for (int j = 0; j < noRouters; j++) {
    if ((nodeId == routerTable[j].startNode) ||
	(nodeId > routerTable[j].startNode &&
	 nodeId <= routerTable[j].endNode)) {
      return (routerTable[j].id - 1);
    }
  }
  return -1;
}
