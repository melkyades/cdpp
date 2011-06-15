#ifndef ROUTER_HH
#define ROUTER_HH

#include "SimulationObj.hh"
#include "Types.hh"
#include "RouteState.hh"
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

class Router : public SimulationObj {
private:
  int index;
  Random *numGen;
  MLCG *gen;
  ConfigNet *netConf;
  int statDest;

  int maxQLen;
  int propDelay(int delayType);
  int servDelay();
  int getRouterId(int );
public:
  void finalize();
  void setDistr();
  Router(int , ConfigNet*, int );
  ~Router();
  BasicState *allocateState();
  void executeProcess();
};

#endif
