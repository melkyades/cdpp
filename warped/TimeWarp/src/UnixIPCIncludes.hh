#ifndef UNIX_IPC_INCLUDES_HH
#define UNIX_IPC_INCLUDES_HH

#include "config.hh"

#ifdef CLUMPS_DPC

#ifdef SUNOS
union semun {
  int val;
  struct semid_ds *buf;
  ushort *array ;
};
#endif


#endif

#endif
