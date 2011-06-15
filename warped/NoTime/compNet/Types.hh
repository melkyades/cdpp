#ifndef TYPES_HH
#define TYPES_HH

#include "BasicEvent.hh"

typedef enum {SCHED, MESSAGE} SigType;
enum DistType {UNIFORM, POISSON, EXPONENTIAL, NORMAL, BINOMIAL};

typedef struct {
  int id;
  int startNode;
  int endNode;
  int connRouter;
  DistType dist;
  int mean;
  int var;
} RouterInfo;

class NetEvent : public BasicEvent {
public:
  NetEvent() {}
  ~NetEvent() {}
  SigType type;
};

class SchedEvent : public NetEvent {
public:
  SchedEvent() { type = SCHED;}
  ~SchedEvent(){}
};

class MessEvent : public NetEvent {
public:
  MessEvent() { type = MESSAGE; noHops = 0;}
  ~MessEvent(){}
  VTime startTime;
  int srcId;
  int finalDest;
  int noHops;
};

class Link {
public:
  Link() {evnt = NULL; next = NULL;}
  ~Link() {delete evnt;}
  MessEvent *evnt;
  Link *next;
};

#endif
