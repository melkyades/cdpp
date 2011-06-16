#ifndef _INPUT_Q_INTERFACE_HH_
#define _INPUT_Q_INTERFACE_HH_

// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.

// BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY 
// FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT 
// PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, 
// EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE 
// PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME 
// THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION. 

// IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING 
// WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR 
// REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR 
// DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL 
// DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM 
// (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED 
// INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF 
// THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH HOLDER 
// OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 

// Authors: Philip A. Wilsey            phil.wilsey@uc.edu
//          Malolan Chetlur             mal@ececs.uc.edu
//---------------------------------------------------------------------------

//This is the class interface for the various Q implementations in the
//Simulation kernel
//The specific Q implementations are hidden from the other components of the
// simulation kernel by the aggregation of Q implemtations in this class 
//interface. 
//All Q implementations have to be inherited from  the abstract class called
//InputQ

#include "ConfigurationManager.hh"
#include "InputQ.hh"
#include "DefaultVTime.hh"

class BasicEvent;

extern ConfigurationManager* configurationManager;

class InputQInterface {
private:
  InputQ* inputQ;

public:

  InputQInterface() {
    //    setQImplementation(configurationManager->getQImplementation());
  }

  ~InputQInterface() {};

  //Funtion to attach the particular Q implementation in the kernel
  //Will be used by the kernel configuration tool or by the
  //constructor to set the appropriate Q implementation
  void setQImplementation(InputQ* qImplementation) {
    inputQ = qImplementation;
  }

  //Insert an Event into the Input Event std::list
  void insert(BasicEvent* event) {
    inputQ->insert(event);
  }

  //returns the event to be processed
  BasicEvent* getEvent() {
    return inputQ->getEvent();
  }

  //Peeks the event to be executed
  //Does not modify the state of the Queue
  BasicEvent* peekEvent() {
    return inputQ->peekEvent();
  }

  //This function removes the old events in the event std::list that are no more
  //needed by the simulation kernel
  void garbageCollect(VTime* gTime) {
    inputQ->garbageCollect(gTime);
  }

};

#endif
