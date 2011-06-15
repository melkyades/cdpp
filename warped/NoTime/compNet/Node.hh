#ifndef NODE_HH
#define NODE_HH

#include "Types.hh"
#include "SimulationObj.hh"
#include "ConfigNet.hh"

#ifdef __GNUG__
#include <MLCG.h>
#include <Random.h>
#include <Normal.h>
#include <Poisson.h>
#include <Binomial.h>
#include <Uniform.h>
#include <NegExp.h>
#else
#include "../rnd/Random.h"
#include "../rnd/MLCG.h"
#include "../rnd/Normal.h"
#include "../rnd/Poisson.h"
#include "../rnd/Binomial.h"
#include "../rnd/Uniform.h"
#include "../rnd/NegExp.h"
#endif

class Node : public SimulationObj {
private:
  int routerId, statDest; // The destination ids of router and SC
  ConfigNet *netConf;
  Random *numGen, *destGen;

  void genMessage(MessEvent *); // Form a message
  int propDelay(); 
  int getDest();
  int interMessDelay(); // Required for generting message
public:

  Node(int, ConfigNet*, int, int);
  ~Node();

  void initialize();
  void setDistr();
  BasicState *allocateState();
  void executeProcess();
};

#endif
