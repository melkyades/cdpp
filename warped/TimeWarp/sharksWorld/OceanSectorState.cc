#ifndef OCEANSECTORSTATE_CC
#define OCEANSECTORSTATE_CC

#include "OceanSectorState.hh" 


OceanSectorState::OceanSectorState(){
  fishList.setFunc(OceanEventCompare);
  sharkList.setFunc(OceanEventCompare);
} 
  
OceanSectorState::~OceanSectorState(){
  
  // remove all elements
  OceanEvent *tmp = NULL;
  OceanEvent *tmp1 = NULL;
  tmp = fishList.getHead();
  while(tmp != NULL){
    tmp1= tmp->next;
    delete tmp;
    tmp = tmp1;
  }
  tmp = sharkList.getHead();
  while(tmp != NULL){
    tmp1= tmp->next;
    delete tmp;
    tmp = tmp1;
  }

   
};

// the copy constructor

OceanSectorState::OceanSectorState(const OceanSectorState& oceanState){
  cout << "Copy Constructor" << endl;
  fishList = oceanState.fishList ;
  sharkList = oceanState.sharkList ;
}

OceanSectorState&
OceanSectorState::operator=(OceanSectorState& oceanState){
  //  cout << " Equating States " << endl ;
  
  // oceanState.fishList.print();
  //oceanState.sharkList.print();
  
  OceanEvent *tmp ;

  fishList.setFunc(OceanEventCompare);
  sharkList.setFunc(OceanEventCompare);

  
  tmp = oceanState.fishList.getHead();
  while(tmp != NULL){
    OceanEvent *x = (OceanEvent *) new char[sizeof(OceanEvent)];
    new (x) OceanEvent(tmp);
    fishList.insert(x);
    //cout << *tmp << endl;
    //cout << tmp << tmp->next << endl;
    tmp = tmp->next ;
  }
  
  tmp = oceanState.sharkList.getHead();
  while (tmp != NULL){
    OceanEvent *x = (OceanEvent *)new char[sizeof(OceanEvent)];
    new (x) OceanEvent(tmp);
    sharkList.insert(x);
    tmp = tmp->next ;
  }
  return *this;
}
#endif
