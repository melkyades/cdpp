//-*-c++-*-
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
// $Id: MemGen.cc,v 1.3 1999/09/13 17:26:18 dmartin Exp $
//
//
//---------------------------------------------------------------------------


#include<iostream.h>
#include<fstream.h>
#include <cstdlib>

int numProcs, seq, numLPs, cacheSpeed, mainSpeed, numRequests;
float cacheHit;

#define NUMPER 6

void writeObjects( ostream &os, int lpCounter ){
  int procCounter;
  int procPerLP = numProcs / numLPs;
  for( procCounter = 0; procCounter < procPerLP; procCounter++ ){
    int baseId = ( lpCounter * procPerLP * NUMPER ) + (NUMPER * procCounter);
    os << "    MemSourceObject source" << baseId << "(" << "\"source"
	<< baseId << "\", " << numRequests << ");" << endl;
    os << "    source" << baseId << ".id = " << baseId << ";" << endl;
    os << "    source"<< baseId <<
      ".setDistribution(FIXED,"<<baseId+1<<");"<<endl;
    os << "    source"<< baseId << ".dest = " << baseId + 1 << ";" << endl;
//     os << "    source"<< baseId << ".name = " << "\"source" << baseId <<
//       "\";" << endl;
    os << "    lp.registerObject(&source"<<baseId<<");"<<endl;
    os << endl;

    int myId = baseId + 1;
    os << "    JoinObject join" << myId << "(" << "\"join" << myId << "\");"
       << endl;
    os << "    join" << myId << ".id = " << myId << ";" << endl;
    os << "    join" << myId << ".dest = " << myId + 1 << ";" << endl;
//    os << "    join" << myId << ".name = " << "\"join"<<myId<< "\";" << endl;
    os << "    lp.registerObject(&join"<<myId<<");"<<endl;
    os << endl;
    
    myId++;
    os << "    QueueObject queue" << myId  << "(" << "\"queue" << myId 
       << "\");" << endl;
    os << "    queue" << myId << ".id = " << myId << ";" << endl;
    os << "    queue"<< myId << ".serverID = " << myId + 1 << ";" << endl;
//    os << "    queue"<< myId << ".name = " << "\"queue" << myId <<
//      "\";" << endl;
    os << "    lp.registerObject(&queue"<<myId<<");"<<endl;
    os << endl;

    myId++;
    os << "    ForkObject fork" << myId << "(" << "\"fork" << myId 
       << "\");" << endl;
    os << "    fork" << myId << ".id = " << myId << ";" << endl;
    os << "    int outputs" << myId << "[2] = { " << myId + 1 << "," << 
      numProcs * NUMPER << " };" << endl;
    os << "    fork"<< myId <<
      ".setForkDistribution(SPECIAL, 2, outputs"<< myId <<"," << cacheHit << ");"<<endl;
//    os << "    fork"<< myId << ".dest = " << myId + 1 << ";" << endl;
//    os << "    fork"<< myId << ".name = " << "\"fork" << myId <<
//      "\";" << endl;
    os << "    lp.registerObject(&fork"<<myId<<");"<<endl;
    os << endl;

    myId++;
    os << "    ServerObject server" << myId << "(" << "\"server" << myId 
       << "\");" << endl;
    os << "    server" << myId << ".id = " << myId << ";" << endl;
    os << "    server"<< myId <<
      ".setServerDistribution(FIXED, " << cacheSpeed << ");"<<endl;
    os << "    server"<< myId << ".dest = " << myId + 1 << ";" << endl;
//    os << "    server"<< myId << ".name = " << "\"server" << myId <<
//      "\";" << endl;
    os << "    lp.registerObject(&server"<<myId<<");"<<endl;
    os << endl;
    
    myId++;
    os << "    JoinObject join" << myId << "(" << "\"join" << myId << "\");"
       << endl;
    os << "    join" << myId << ".id = " << myId << ";" << endl;
    os << "    join" << myId << ".dest = " << baseId << ";" << endl;
//    os << "    join" << myId << ".name = " << "\"join"<<myId<< "\";" << endl;
    os << "    lp.registerObject(&join"<<myId<<");"<<endl;
    os << endl;    
  }
}

void writeMem( ostream &os ){
  int baseId = numProcs * NUMPER;
  int myId = baseId;

  os << "    JoinObject join" << myId << "(" << "\"join" << myId << "\");"
     << endl;
  os << "    join" << myId << ".id = " << myId << ";" << endl;
  os << "    join" << myId << ".dest = " << myId + 1 << ";" << endl;
//  os << "    join" << myId << ".name = " << "\"join"<<myId<< "\";" << endl;
  os << "    lp.registerObject(&join"<<myId<<");"<<endl;
  os << endl;

  myId++;
  os << "    QueueObject queue" << myId  << "(" << "\"queue" << myId 
     << "\");" << endl;
  os << "    queue" << myId << ".id = " << myId << ";" << endl;
  os << "    queue"<< myId << ".serverID = " << myId + 1 << ";" << endl;
//  os << "    queue"<< myId << ".name = " << "\"queue" << myId <<
//    "\";" << endl;
  os << "    lp.registerObject(&queue"<<myId<<");"<<endl;
  os << endl;
  
  myId++;
  os << "    ServerObject server" << myId << "(" << "\"server" << myId 
       << "\");" << endl;
  os << "    server" << myId << ".id = " << myId << ";" << endl;
  os << "    server"<< myId <<
      ".setServerDistribution(FIXED, " << mainSpeed << ");"<<endl;
  os << "    server"<< myId << ".dest = " << myId + 1 << ";" << endl;
//  os << "    server"<< myId << ".name = " << "\"server" << myId <<
//    "\";" << endl;
  os << "    lp.registerObject(&server"<<myId<<");"<<endl;
  os << endl;

  myId++;
  os << "    MemRouterObject memRouter" << myId << "(" << "\"memRouter" 
     << myId << "\");" << endl;
  os << "    memRouter" << myId << ".id = " << myId << ";" << endl;
//  os << "    memRouter"<< myId << ".dest = " << myId + 1 << ";" << endl;
//  os << "    memRouter"<< myId << ".name = " << "\"memRouter" << myId <<
//      "\";" << endl;
  os << "    lp.registerObject(&memRouter"<<myId<<");"<<endl;
  os << endl;

}

void writeLPs( ostream &os ){
  int lpCounter = 0;
  for( lpCounter = 0; lpCounter < numLPs; lpCounter++ ){
    if( lpCounter != 0 ) os << "  else";
    os << "  if(id == " << lpCounter << ") {" << endl;
    if( lpCounter != numLPs - 1 && numLPs != 1 ){
      // the normal case only has the number of objects per LP
	if(seq == 0) {		// Not sequential
	    os << "    LogicalProcess lp(" << numProcs * NUMPER + 4 <<
		"," << (numProcs * NUMPER) / numLPs << "," << numLPs << ");";
	    os << " // total, local, LPs" << endl;
	} else {
	    os << "    SequentialLP lp(" << numProcs * NUMPER + 4 <<
		"," << (numProcs * NUMPER) / numLPs << "," << numLPs << ");";
	    os << " //  total, local, LPs" << endl;
	}
      writeObjects(os, lpCounter );
    }
    else {
      // this guy has additional objects representing main memory
	if(seq == 0) {		// Not Sequential
	    os << "    LogicalProcess lp(" << numProcs * NUMPER + 4 
	       << "," << (numProcs * NUMPER) / numLPs + 4 << "," << numLPs
	       << ");";
	    os << " // total, local, LPs" << endl;
	} else {
	    os << "    SequentialLP lp(" << numProcs * NUMPER + 4 
	       << "," << (numProcs * NUMPER) / numLPs + 4 << "," << numLPs
	       << ");";
	    os << " // total, local, LPs" << endl;
	}
      writeObjects(os, lpCounter );
      writeMem(os);
    }
    os << "lp.allRegistered();" << endl;
    os << "lp.simulate();" << endl;
    os << "  }" << endl;
  }
}

void writeProgram( ostream &os ){
  os << "// generated by machine" << endl << endl;
  os << "#include \"SidStuff.hh\" " << endl;
  if(seq == 0){			// Not Sequential
      os << "#include \"LogicalProcess.hh\"" << endl;
      os << "#include \"CommMgrInterface.hh\"" << endl;
  }
  else				// Sequential
      os << "#include \"SequentialLP.hh\"" << endl;
  os << "#include \"SimulationTime.hh\" " << endl<< endl;

  if(seq == 0)
    {
      os << "const VTime LogicalProcess::SIMUNTIL = PINFINITY;" 
	 << endl << endl;
    }
  else
    {
      os << "const VTime SequentialLP::SIMUNTIL = PINFINITY;" 
	 << endl << endl;
    }
  if(seq == 0) {
  os << "int main(int argc, char *argv[]) {" << endl;
  }else{
    os << "main() {" << endl;
  }
  os << "  int id=0;" << endl;
  if(seq == 0) {
    os << " physicalCommInit( &argc, &argv );" << endl;
    os << " id = physicalCommGetId();" << endl;
  }
  
  os << "#ifdef MESSAGE_AGGREGATION" << endl;
  os << "  getMessageManagerParameters(argc, argv);" << endl;
  os << "#endif" << endl;
  
  writeLPs(os);
  os << "  return 0;" << endl;
  os << "} // main" << endl;
}

main(){
  ostream *os;
  ofstream outFile;
  char fileName[255];

  cout << "How many processors? : ";
  cin >> numProcs;
  cout << "Sequential or Parallel? (1/0) : ";
  cin >> seq;
  if(seq == 0) {
    cout << "How many LPs ? : ";
    cin  >> numLPs;	
  }
  else{ 
      numLPs = 1;
  }
  cout << "Speed of cache? (integer value, e.g. 10): ";
  cin >> cacheSpeed;
  cout << "Cache hit ratio? (e.g. .85) : ";
  cin >> cacheHit;
  cout << "Speed of main memory? (integer value, e.g. 100): ";
  cin >> mainSpeed;
  cout << "number of requests per processor? : ";
  cin >> numRequests;
  cout << "Name of file to generate? (Hit return to write to screen) : ";
  cin >> fileName;
  if( fileName[0] == '\n' ){
    os = &cout;
  }
  else{
    outFile.open( fileName );
    if( outFile ){
      os = &outFile;
    }
    else{
      cerr << "Couldn't open " << fileName << endl;
      exit( -1 );
    }
  }

  writeProgram( *os );
}
  

