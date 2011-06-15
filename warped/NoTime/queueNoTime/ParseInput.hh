#ifndef PARSE_INPUT_HH
#define PARSE_INPUT_HH

#include <fstream.h>
#include "Types.hh"

class ParseInput {
public:
  ParseInput();
  ~ParseInput();
  void readServers(); // reads the server Configuration
  void readSource(); // reads the source Configuration
  
  DistType findDistr(char *);
  DistType srcDist, *serverDist;
  int srcMean, srcSD, customers;
  int noServers, *serverMean, *serverSD;
  int numLP;

private:
  ifstream configFile;
  char distr[20];
};

#endif
