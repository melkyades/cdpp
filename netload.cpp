/*******************************************************************
*
*  DESCRIPTION: class NetworkLoad
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

/** include files **/
#include <strstream.h>
#include <function.h>      // Warning!!! must bne included before string (for operator !=)
#include <string>
#include "netload.h"       // bass class
#include "bsdchann.h"      // class BSDChannel

#ifdef MPI
#include "mpi.h"			//MPI_Routines
#include "LogicalProcess.hh"
#endif

/** public functions **/

/*******************************************************************
* Function Name: NetworkLoader
* Description: Constructor
********************************************************************/
NetworkLoader::NetworkLoader(int argc, char** argv)
: SimLoader( argc, argv), 
delimitator( "." )
{
	commChannel = new BSDChannel("simulator");	 // get the port
	assert( commChannel );
}

/*******************************************************************
* Function Name: ~NetworkLoader
* Description: Destructor
********************************************************************/
NetworkLoader::~NetworkLoader()
{
	assert( commChannel );
	delete commChannel ;

	delete models ; models = NULL ;
	delete events ; events = NULL ;

}

/*******************************************************************
* Function Name: openCommChanel
* Opens the communications channel. Gets the machine ID
********************************************************************/
SimLoader &NetworkLoader::openComm(){

	//Init MPI.If not start up machine, this will update
	//the command line arguments

#ifdef MPI	
	physicalCommInit(&argc , &argv);

	machineID = physicalCommGetId();

	//cout << "MPI ID: " << machineID << endl;
#else
	machineID = 0;
#endif


	return *this;
}

/*******************************************************************
* Function Name: closeCommChanel
* Opens the communications channel. Gets the machine ID
********************************************************************/
SimLoader &NetworkLoader::closeComm(){

#ifdef MPI
	MPI_Finalize();
#endif

	return *this;
}

/*******************************************************************
* Function Name: loadData
* Description: 
********************************************************************/
SimLoader &NetworkLoader::loadData()
{
	string line;

	strstream *outModels = new strstream ;   
	while( ( line = commChannel->readLine() ) != NetworkLoader::delimitator )
		*outModels << line << endl;

	models = outModels ;

	strstream *outEvents = new strstream;
	while( (line = commChannel->readLine()) != NetworkLoader::delimitator )
		*outEvents << line << endl;

	events = outEvents ;

	stopTime( VTime( commChannel->readLine() ) );

	return *this ;
}


/*******************************************************************
* Function Name: writeResults
* Description: 
********************************************************************/
SimLoader &NetworkLoader::writeResults()
{
	return *this ;
}
