#if !defined(COMM_PHY_MPI_NONBLOCK_CC) && defined(MPINONBLOCK)
#define COMM_PHY_MPI_NONBLOCK_CC

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
// $Id: CommPhyMPINonBlock.cc,v 1.3 1998/10/06 20:20:25 mal Exp $
//
//---------------------------------------------------------------------------

#include "CommPhyMPINonBlock.hh"
#include <iostream.h>

extern "C" {
#include "mpi.h"
}

#define MPI_DATA_TAG  100
#define BUFFER_SIZE   65535
#define MESSAGE_SIZE  512

void
CommPhyMPINonBlock::physicalInit(int *argc, char ***argv){
  char *buffer;

  buffer = new char[BUFFER_SIZE];
  MPI_Init(argc, argv);
  MPI_Buffer_attach(buffer, BUFFER_SIZE);
}

int
CommPhyMPINonBlock::physicalGetId() const {
  int id;

  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  return id;
}


void
CommPhyMPINonBlock::physicalSend(char *buffer, int size, int id){
  static MPI_Request request;
  static MPI_Status  status;
  static char msgBuffer[MESSAGE_SIZE];
  static int sent = 1;
  
  //MPI_Bsend(buffer, size, MPI_BYTE, id, MPI_DATA_TAG, MPI_COMM_WORLD);

  while ( sent == 0 ){
    MPI_Test(&request, &sent, &status);
  }
  memcpy(msgBuffer, buffer, size);
  MPI_Isend(msgBuffer, size, MPI_BYTE, id, MPI_DATA_TAG, MPI_COMM_WORLD, &request);
  sent = 0;
}

char *
CommPhyMPINonBlock::physicalProbeRecv() {
  static char *buffer = NULL;
  static MPI_Request request;
  static MPI_Status  status;
  int flag = 0;
  char *newMsg = NULL;

  if ( buffer != NULL ){
    MPI_Test(&request, &flag, &status);
    if ( flag == 1 ){
      newMsg = buffer;
      buffer = NULL;
    }
  } else {
    buffer = new char[MESSAGE_SIZE];
    MPI_Irecv(buffer, MESSAGE_SIZE, MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
  }
  return newMsg;
}

bool
CommPhyMPINonBlock::physicalProbeRecvBuffer(char *, int, bool&){
  cerr << "Should not be called" << endl;
  abort();
  return false;
}

void
CommPhyMPINonBlock::physicalFinalize(){
  MPI_Finalize();
}

#endif // #if !defined(COMM_PHY_MPI_NONBLOCK_CC) && defined(MPI)

