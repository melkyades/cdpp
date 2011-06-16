#ifndef CONFIGNET_HH
#define CONFIGNET_HH

#include <fstream>
#include "Types.hh"

class ConfigNet {
public:
  int noNodes, *node, *nodeMean, *nodeVar;
  DistType *nodeDist;
  int noRouters;
  RouterInfo *routerTable;
  ifstream configFile;
  int LANDelay, WANDelay;
  int numLP, totMess, bufferSize;

  int getRouter(int );
  void readRouterInfo();
  void readDelayInfo();
  void readTrafficInfo();
  void readServTimeInfo();
  DistType findDistr(char *);
  ConfigNet();
  ~ConfigNet();
};

#endif
