/*******************************************************************
*
*  DESCRIPTION: class CommChannel
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __COMM_CHANNEL_H
#define __COMM_CHANNEL_H

/** include files **/
//#include <string>

/** my include files **/
#include "except.h" 

/** forward declarations **/

/** declarations **/
class CommChannel
{
public:
	virtual ~CommChannel();	//Destructor
	virtual string readLine() = 0 ;
	virtual CommChannel &writeLine( const string & ) = 0 ;

protected:
	CommChannel();	//Default constructor

};	// class CommChannel


class CommunicationError: public MException
{
public:
	CommunicationError(): MException( "Channel communication error!" )
	{}
} ;

/** inline **/
inline
CommChannel::CommChannel()
{}

inline
CommChannel::~CommChannel()
{}

#endif   //__COMM_CHANNEL_H 
