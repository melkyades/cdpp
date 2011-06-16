/*******************************************************************
*
*  DESCRIPTION: class BSDChannel
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __BSD_CHANNEL_H
#define __BSD_CHANNEL_H

/** include files **/
#include <string>
#include "commchan.h"      // base class CommChannel

/** forward declarations **/

/** declarations **/
class BSDChannel: public CommChannel
{
public:
	BSDChannel( const std::string &serviceName );  

	BSDChannel( int portNumber );  

	~BSDChannel();	 			//Destructor

	std::string readLine() ;

	CommChannel &writeLine( const std::string & ) ;

private:
	BSDChannel &open( int portNumber ) ;

	int connection ;

};	// class BSDChannel

#endif   //__BSD_CHANNEL_H 
