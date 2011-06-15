#ifndef COMM_PHY_INTERFACE_HH
#define COMM_PHY_INTERFACE_HH
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
//
// $Id: CommPhyInterface.hh,v 1.4 1998/10/06 20:20:24 mal Exp $
//
//---------------------------------------------------------------------------

#include "config.hh"
#include "bool.hh"

class CommPhyInterface {
public:
  virtual ~CommPhyInterface() {}
  virtual void physicalInit(int *argc, char ***argv) = 0;
  virtual int  physicalGetId() const = 0;
  
  // commSend just sends the buffer of length size to the id in the group
  virtual void physicalSend(char *buffer, int size, int id) = 0;

  
  // commProbeRecv probes for a message and if there, stores in a char * buffer
  // and returns that address. If no message, returns NULL;
  virtual char * physicalProbeRecv() = 0;

  
  // commProbeRecvBuffer probes for a message and if there, receives the
  // message if size <= messagesize waiting. sets sizeStatus to true if
  // size <= messagesize otherwise, sizeStatus to false. If there is a message
  // it returns true, otherwise, false;
  virtual bool physicalProbeRecvBuffer(char *buffer, int size, bool& sizeStatus) = 0;
  
  virtual void physicalFinalize() = 0;

protected:
  CommPhyInterface() {}

private:
};


#endif //ifndef COMM_PHY_INTERFACE_HH
