#ifndef TYPES_HH
#define TYPES_HH

#include "BasicEvent.hh"

enum DistType {UNIFORM, POISSON, EXPONENTIAL, NORMAL, BINOMIAL};


class Client : public BasicEvent {
public:
  VTime entryTime;
  Client() {};
  ~Client() {};
};

class IdleEvent : public BasicEvent {
public:
  IdleEvent() {};
  ~IdleEvent() {};
  VTime idleTime;
};

class StatEvent : public BasicEvent {
public:
  StatEvent() {};
  ~StatEvent() {};
  VTime entryTime;  // The time at which the client entered the system
  VTime waitTime;   // Time at which the client is at the server
  VTime exitTime;   // Time at which the client leaves the system
};


typedef struct LINK {
  int entryTime;
  struct LINK *next;
} Link;

#endif

