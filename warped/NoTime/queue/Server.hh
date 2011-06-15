
#ifndef SERVER_HH
#define SERVER_HH

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
#include "../rnd/Random.h"
#include "../rnd/MLCG.h"
#include "../rnd/Normal.h"
#include "../rnd/Poisson.h"
#include "../rnd/Binomial.h"
#include "../rnd/Uniform.h"
#include "../rnd/NegExp.h"
#endif

class Server : public SimulationObj {
private:
  int qDest;
  int statDest;

  Random *numGen; // Generates a number based on the distribution

  int serviceTime();
public:
  Server(int, int, int);
  ~Server();

  void initialize();
  void setDistribution(DistType distr, double avg, double var = 0.0);
  void executeProcess();
  BasicState* allocateState();
};

#endif
