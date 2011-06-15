
#ifndef TABLESECTORSTATE_HH
#define TABLESECTORSTATE_HH

#include <new.h>

#include "BasicState.hh"
#include "SortedListOfEvents.hh"
#include "tableEvent.hh"
#include "BasicEvent.hh"
extern int tableEventCompare(tableEvent *a , tableEvent *b);
class tableSectorState : public BasicState {
public:

tableSectorState();
~tableSectorState();

 tableSectorState(const tableSectorState&);
 tableSectorState& operator=(tableSectorState&);

SortedListOfEvents<tableEvent> puckList ;

  int getSize() const {
    return sizeof(tableSectorState);
  }

  void copyState(BasicState *rhs) {
    operator=(*((tableSectorState *) rhs));
  }
};
#endif

