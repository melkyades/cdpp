#ifndef SIMULATION_OBJECT_HH
#define SIMULATION_OBJECT_HH

#include "config.hh"

#ifdef SEQUENTIAL
#include "SequentialObj.hh"
#elif defined(LAZYAGGR_CANCELLATION)
#include "DynamicCancellation.hh"
#else
#include "TimeWarp.hh"
#endif


class SimulationObj : public
#ifdef SEQUENTIAL
  SequentialObj
#elif defined(LAZYAGGR_CANCELLATION)
  LazyAggrTimeWarp
#else
  TimeWarp
#endif
{
public:
SimulationObj() {};
~SimulationObj() {};
};

#endif
