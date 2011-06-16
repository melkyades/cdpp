/*******************************************************************
*
*  DESCRIPTION: class NetworkLoader
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian
*	   Alejandro Troccoli (v3)
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*	  mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 30/08/2000 (v3)
*
*******************************************************************/


#ifndef __NETLOAD_H
#define __NETLOAD_H

/** include files **/
#include "loader.h"

/** forward declarations **/
class CommChannel ;

/** declarations **/
class NetworkLoader : public SimLoader
{
public:
	NetworkLoader(int argc, char** argv);	//Default constructor
	virtual ~NetworkLoader();	//Destructor
	
	SimLoader &loadData();
	virtual SimLoader &openComm();
	virtual SimLoader &closeComm();
	
protected:
	SimLoader &writeResults();
	
private:
	CommChannel *commChannel ;
	const std::string delimitator ;
	
};	// class NetworkLoader

#endif   //__NETLOAD_H 
