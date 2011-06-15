#ifndef TABLESECTORSTATE_CC
#define TABLESECTORSTATE_CC	

#include "tableSectorState.hh"

tableSectorState::tableSectorState(){
  puckList.setFunc(tableEventCompare);
}

tableSectorState::~tableSectorState(){
  /*  tableEvent *tmp = NULL;
  tableEvent *tmp1 = NULL;
  tmp = puckList.getHead();
  while(tmp != NULL){
    tmp1= tmp->next;
    delete [] (char *)tmp;
    tmp = tmp1;
    } */
};

tableSectorState::tableSectorState(const tableSectorState& tableState){
 
  //  cout << "Copy Constructor" << endl;
  puckList = tableState.puckList ;

}

tableSectorState&
tableSectorState::operator=(tableSectorState& tableState){
 
  tableEvent *tmp;

  BasicState::operator=((BasicState &) tableState);
  
  puckList.setFunc(tableEventCompare);

  tmp = tableState.puckList.getHead();

  while(tmp != NULL){
    tableEvent *x = (tableEvent *) new char[sizeof(tableEvent)];
    new (x) tableEvent(tmp);
    puckList.insert(x);
    tmp = tmp->next;

  }
  return *this;
}
#endif;
