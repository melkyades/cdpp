#ifndef SOURCE_HH
#define SOURCE_HH

#include "Types.hh"
#include "SimulationObj.hh"

#ifdef __GNUG__
#include <MLCG.h>
#include <Random.h>
#include <Normal.h>
#include <Poisson.h>
#include <Binomial.h>
#include <Uniform.h>
#include <NegExp.h>
#else
#include "../rnd/MLCG.h"
#include "../rnd/Random.h"
#include "../rnd/Normal.h"
#include "../rnd/Poisson.h"
#include "../rnd/Binomial.h"
#include "../rnd/Uniform.h"
#include "../rnd/NegExp.h"
#endif

class Source : public SimulationObj {
private:
  int maxEvents;      // total number of events in queue to generate
  int dest;           // Destination object id

  Random *numGen;     // Based on the distribution this guy generates nos.
  MLCG *gen;          // Random number generator

  int interArrivalTime(); // Time between two items in the queue

public:
  Source(int myId, int myDest, int totalEvents = 100);
  ~Source();

  void setDistribution(DistType distr, double mean, double variance = 0.0); 
  void initialize();
  void executeProcess();
  BasicState* allocateState();
};

#endif
