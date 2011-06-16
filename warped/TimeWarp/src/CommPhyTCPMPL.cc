#if !defined(COMM_PHY_TCPMPL_CC) && defined(TCPLIB)
#define COMM_PHY_TCPMPL_CC
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
// $Id: CommPhyTCPMPL.cc,v 1.5 1998/11/06 20:36:03 gsharma Exp $
//
//---------------------------------------------------------------------------

#include "CommPhyTCPMPL.hh"
#include "TCPMPL.hh"
#include <cstdlib>
#include <iostream.h>

void
CommPhyTCPMPL::physicalInit(int *argc, char ***argv){

  MPL.initTCPMPL(*argc, *argv);
}

int
CommPhyTCPMPL::physicalGetId() const {
  return MPL.getId();
}

void
CommPhyTCPMPL::physicalSend(char *buffer, int size, int id){
  MPL.sendData(buffer, size, id);
}

char *
CommPhyTCPMPL::physicalProbeRecv(){
  int source, length;
  char *msg;
  
  source = length = -1;
  if ( MPL.probeForData(&source, &length) ) {
    if ( source < 0 || length < 0 ){
      cerr << "CommPhyTCPMPL::physicalProbeRecv probeForData returned invalid source: " << source << " and length:" << length << endl;
    }
    msg = new char[length];
    if ( msg == NULL ){
      cerr << "CommPhyTCPMPL::physicalProbeRecv new returned NULL" << endl;
      abort();
    }
    MPL.recvDataFrom(msg, length, source);
    return msg;
  } else {
    return NULL;
  }
}

bool
CommPhyTCPMPL::physicalProbeRecvBuffer(char *buffer, int size, bool& sizeStatus){
  int source, length;

  source = length = -1;
  if ( MPL.probeForData(&source, &length) ) {
    if ( source < 0 || length < 0 ){
      cerr << "CommPhyTCPMPL::physicalProbeRecvBuffer probeForData returned invalid source:" << source << " and length:" << length << endl;
    }
    if ( length > size ){
      sizeStatus = false;
    } else {
      sizeStatus = true;
      MPL.recvDataFrom(buffer, length, source);
    }
    return true;
  } else {
    return false;
  }
}


void
CommPhyTCPMPL::physicalFinalize(){
}

int
CommPhyTCPMPL::physicalGetSize() const{
  return(MPL.getTCPMPLSize());
}


#endif // #if !defined(COMM_PHY_TCPMPL_CC) && defined(TCPLIB)

