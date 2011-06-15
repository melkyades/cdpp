#include "ParseInput.hh"
#include <fstream.h>

ParseInput::ParseInput() {
  configFile.open("/home/nthondug/research/warped/NoTime/queue/Queue.config");

  if (configFile.good()) {
    readSource();
    readServers();
    configFile >> numLP;
  } else {
    cerr << "Couldn't open the config File \n";
  }
  configFile.close();
}

DistType
ParseInput::findDistr(char *distrib) {
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
ParseInput::readSource() {
  // We can have only one source
  configFile >> distr >> srcMean >> srcSD >> customers;
  srcDist = findDistr(distr);
}

void
ParseInput::readServers() {
  configFile >> noServers;
  char separ;
  int i, j, lMean, lSD, syntax, iter, *server; 
  DistType typ;

  if (noServers > 0) {
    serverDist = new DistType[noServers];
    serverMean = new int[noServers];
    serverSD = new int[noServers];
    iter = noServers;
    server = new int[iter];

    while (iter > 0) {
      iter--;
      i = 0; syntax = 0;

      configFile >> server[i];
      while ((separ = configFile.get()) == ' ');

      // a, b, c 
      while (separ == ',') {
	iter--; 
	configFile >> server[++i];
	while ((separ = configFile.get()) == ' ');
      } 

      // a..b
      if (separ == '.') {
	configFile.get();
	configFile >> server[++i];
	iter = iter - (server[i] - server[i-1]);
	while ((separ = configFile.get()) == ' ');
	syntax = 1;
      }
      
      if (iter < 0) {
	cerr << "Config file ERROR : More servers than specified \n";
	abort();
      }

      configFile.unget();
      // Read the distribution, mean and SD of these servers
      configFile >> distr >> lMean >> lSD;
      typ = findDistr(distr);
      if (syntax != 1) {
	for (j = 0; j <= i; j++) {
	  if (server[j] <= noServers) {
	    serverDist[server[j]-1] = typ;
	    serverMean[server[j]-1] = lMean;
	    serverSD[server[j]-1] = lSD;
	  } else {
	    cerr << "Config File ERROR : Server not " << server[j] << " found \n";
	    abort();
	  }
	}
      } else {
	for (j = server[0]; j <= server[1]; j++) {
	  if (j <= noServers) {
	    serverDist[j-1] = typ;
	    serverMean[j-1] = lMean;
	    serverSD[j-1] = lSD;
	  } else {
	    cerr << "Config File ERROR : Server not " << j << " found \n";
	    abort();
	  }
	}
      }
    }
    delete [] server;
  } else {
    cerr << "No of servers must be atleast 1" << endl;
    abort();
  }
}

ParseInput::~ParseInput() {
  delete [] serverDist;
  delete [] serverMean;
  delete [] serverSD;
}
