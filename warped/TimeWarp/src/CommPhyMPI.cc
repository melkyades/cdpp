#if !defined(COMM_PHY_MPI_CC) && defined(MPI)
#define COMM_PHY_MPI_CC

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
// $Id: CommPhyMPI.cc,v 1.7 1998/12/31 22:33:47 gsharma Exp $
//
//---------------------------------------------------------------------------

#include "CommPhyMPI.hh"
#include <malloc.h>
#include <cstdlib>
#include <iostream.h>

extern "C" {
#include "mpi.h"
}

#define MPI_DATA_TAG  100
#define BUFFER_SIZE   65535

void
CommPhyMPI::physicalInit(int *argc, char ***argv){
  mpi_buffer = new char[BUFFER_SIZE];
  MPI_Init(argc, argv);
  MPI_Buffer_attach(mpi_buffer, BUFFER_SIZE);
}

int
CommPhyMPI::physicalGetId() const {
  int id;

  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  return id;
}


void
CommPhyMPI::physicalSend(char *buffer, int size, int id){
  MPI_Bsend(buffer, size, MPI_BYTE, id, MPI_DATA_TAG, MPI_COMM_WORLD);
}

char *
CommPhyMPI::physicalProbeRecv() {
  char *msg = NULL;
  MPI_Status status;
  int flag = 0, size= 0;

  MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
  if ( flag ){
    MPI_Get_count(&status, MPI_BYTE, &size);
    msg = new char[size];
    if ( msg == NULL ){
      cerr << "CommPhyMPI::physicalProbeRecv new returned NULL" << endl;
      abort();
    }
    MPI_Recv(msg, size, MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    return msg;
  } else {
    return NULL;
  }
}

bool
CommPhyMPI::physicalProbeRecvBuffer(char *buffer, int size, bool& sizeStatus){
  MPI_Status status;
  int flag = 0, msgSize = 0;

  MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
  if ( flag ) {
    MPI_Get_count(&status, MPI_BYTE, &msgSize);
    if ( msgSize > size ){
      sizeStatus = false;
    } else {
      sizeStatus = true;
      MPI_Recv(buffer, msgSize, MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    }
    return true;
  } else {
    return false;
  }
  
}

void
CommPhyMPI::physicalFinalize(){
  MPI_Finalize();
  delete [] mpi_buffer;
}

int
CommPhyMPI:: physicalGetSize() const {
  int size ;
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  return size ;
}
#endif // #if !defined(COMM_PHY_MPI_CC) && defined(MPI)
