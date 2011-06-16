// Copyright (c) 1994,1995 Ohio Board of Regents and the University of
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
//
//
// $Id: main.cc,v 1.3 1998/10/06 03:25:53 urajasek Exp $
//
//
//---------------------------------------------------------------------------

#include <fstream>
#include <iostream.h>
#include <string>

#include "config.hh"

#ifdef SEQUENTIAL
#include "SequentialLP.hh"
#else
#include "LogicalProcess.hh"
#endif

#include "RAIDTypes.hh"
#include "RAIDProcess.hh"
#include "RAIDFork.hh"
#include "RAIDDisk.hh"
#include <stdio.h>
#include <cstdlib>

#include "SimulationTime.hh"

#ifdef SEQUENTIAL
const VTime SequentialLP::SIMUNTIL = PINFINITY;
#else
const VTime LogicalProcess::SIMUNTIL = PINFINITY;
#endif

typedef void (*functionPtr)();
const DISK_TYPE SIMULATEDDISK = LIGHTNING;

#include "CommMgrInterface.hh"

int main(int argc, char *argv[]) {
  ifstream infile;
  int NUMPROCESSES;
  int NUMFORKS;
  int NUMDISKS;
  int MAXREQUESTS;
  int NUMLPS;
  int id, i;

#ifndef SEQUENTIAL
  physicalCommInit(&argc , &argv);
  id = physicalCommGetId();
#endif
  infile.open("RAID.config");

  if ( infile.good() ) {
    infile >> NUMPROCESSES >> NUMFORKS >> NUMDISKS >> MAXREQUESTS >> NUMLPS;
    //    cout <<  NUMPROCESSES << NUMFORKS << NUMDISKS << MAXREQUESTS << NUMLPS << endl; 
    infile.close();
  } else {
    cerr << "Cannot find file RAID.config" << endl;
    exit(-1);
  }

  if ((NUMPROCESSES < NUMLPS) || (NUMDISKS < NUMLPS) || (NUMFORKS < NUMLPS)) {
    cerr << "I'm sorry, but there must be at least one source, one disk, "
	 << "and one fork per LP\n"
	 << "for default assignment strategy" << endl;
    exit(-1);
  }

  cout << "\n\nStarting simulation for " << NUMPROCESSES 
       << " processes sending " << MAXREQUESTS << " REQUESTs to "
       << NUMFORKS << " forks and " << NUMDISKS << " disks on " << NUMLPS 
       << " LPs "<< endl;


#ifdef SEQUENTIAL
  id = 0 ;
#endif

  char buffer[80];

#ifdef MESSAGE_AGGREGATION
  getMessageManagerParameters(argc, argv);
#endif

  if(id < NUMLPS-1) {
    int processes = NUMPROCESSES/NUMLPS;
    int forks = NUMFORKS/NUMLPS;
    int disks = NUMDISKS/NUMLPS;
    int forkNum = 1;
    int processNum;
    int diskNum;
    RAIDProcess** process = new RAIDProcess*[processes];
    RAIDFork** fork = new RAIDFork*[forks];
    RAIDDisk** disk = new RAIDDisk*[disks];

#ifdef SEQUENTIAL
    SequentialLP lp(NUMPROCESSES+NUMFORKS+NUMDISKS,
		    processes+forks+disks,NUMLPS);
#else
    // total, local, LPs
    LogicalProcess lp(NUMPROCESSES+NUMFORKS+NUMDISKS,
		      processes+forks+disks,NUMLPS);
#endif

  cout << "\n\nStarting simulation for LP " << id << " with " << processes
       << " processes sending " << MAXREQUESTS << " requests to "
       << forks << " forks. There are " << disks << " disks on this LP." 
       << endl;

    for (i = 0; i < processes; i++) {
      processNum = i + id*(NUMPROCESSES/NUMLPS);
      sprintf(buffer,"process%d",processNum);
      process[i] = new RAIDProcess(processNum,buffer,forkNum*id+NUMPROCESSES,
				   NUMDISKS,LIGHTNING,MAXREQUESTS,
				   NUMPROCESSES+NUMFORKS);
      (process[i])->setDistribution(UNIFORM, 5+i, 100+i);
      lp.registerObject(process[i]);
      forkNum++;
      forkNum = forkNum%(NUMFORKS/NUMLPS);
    }

    for (i = 0; i < forks; i++) {
      forkNum = i + id*(NUMFORKS/NUMLPS) + NUMPROCESSES;
      sprintf(buffer,"fork%d",forkNum);
      fork[i] = new RAIDFork(forkNum,buffer,NUMDISKS,NUMPROCESSES,
				  NUMPROCESSES+NUMFORKS);
      lp.registerObject(fork[i]);

    }

    for (i = 0; i < disks; i++) {
      diskNum = i + id*(NUMDISKS/NUMLPS) + NUMPROCESSES + NUMFORKS;
      sprintf(buffer,"Disk%d",diskNum);
      disk[i] = new RAIDDisk(diskNum,buffer,SIMULATEDDISK);
      lp.registerObject(disk[i]);
    }

    lp.allRegistered();
    lp.simulate();

    for (i = 0; i < processes; i++) {
      delete process[i];
    }
    delete [] process;
    for (i = 0; i < forks; i++) {
      delete fork[i];
    }
    delete [] fork;
    for (i = 0; i < disks; i++) {
      delete disk[i];
    }
    delete [] disk;

  } else if (id == NUMLPS - 1) {

    int processes = NUMPROCESSES%NUMLPS + NUMPROCESSES/NUMLPS;
    int forks = NUMFORKS%NUMLPS + NUMFORKS/NUMLPS;
    int disks = NUMDISKS%NUMLPS + NUMDISKS/NUMLPS;
    int forkNum = 1;
    int processNum;
    int diskNum;
    RAIDProcess** process = new RAIDProcess*[processes];
    RAIDFork** fork = new RAIDFork*[forks];
    RAIDDisk** disk = new RAIDDisk*[disks];

#ifdef SEQUENTIAL
    SequentialLP lp(NUMPROCESSES+NUMFORKS+NUMDISKS,processes+forks+disks,
		    NUMLPS);
#else
    // total, local, LPs
    LogicalProcess lp(NUMPROCESSES+NUMFORKS+NUMDISKS,processes+forks+disks,
                      NUMLPS);
#endif

    cout << "\n\nStarting simulation for LP " << id << " with " << processes
	 << " processes sending " << MAXREQUESTS << " requests to "
	 << forks << " forks. There are " << disks << " disks on this LP." 
	 << endl;

    for (i = 0; i < processes; i++) {
      processNum = i + id*(NUMPROCESSES/NUMLPS);
      sprintf(buffer,"process%d",processNum);
      process[i] = new RAIDProcess(processNum,buffer,forkNum*id+NUMPROCESSES,
				   NUMDISKS,LIGHTNING,MAXREQUESTS,
				   NUMPROCESSES+NUMFORKS);
      (process[i])->setDistribution(UNIFORM, 5+i, 100+i);
      lp.registerObject(process[i]);
      forkNum++;
      forkNum = forkNum%(NUMFORKS/NUMLPS);
    }

    for (i = 0; i < forks; i++) {
      forkNum = i + id*(NUMFORKS/NUMLPS) + NUMPROCESSES;
      sprintf(buffer,"fork%d",forkNum);
      fork[i] = new RAIDFork(forkNum,buffer,NUMDISKS,NUMPROCESSES,
			     NUMPROCESSES+NUMFORKS);
      lp.registerObject(fork[i]);

    }

    for (i = 0; i < disks; i++) {
      diskNum = i + id*(NUMDISKS/NUMLPS) + NUMPROCESSES + NUMFORKS;
      sprintf(buffer,"Disk%d",diskNum);
      disk[i] = new RAIDDisk(diskNum,buffer,SIMULATEDDISK);
      lp.registerObject(disk[i]);
    }

    lp.allRegistered();
    lp.simulate();

    for (i = 0; i < processes; i++) {
      delete process[i];
    }
    delete [] process;
    for (i = 0; i < forks; i++) {
      delete fork[i];
    }
    delete [] fork;
    for (i = 0; i < disks; i++) {
      delete disk[i];
    }
    delete [] disk;

  } else {
    cerr << "No LP " << id << endl;
  }

  return 0;

}
