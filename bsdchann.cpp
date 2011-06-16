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

/** include files **/
#include <netdb.h>         // getservbyname( ... )
#include <sys/types.h>     // 
#include <sys/socket.h>    // socket( ... )
#include <netinet/in.h>    // htonl( ... )
#include <arpa/inet.h>
#include <cstdio>         // close( int ) 
#include <cstring>
#include <unistd.h>        // fork() 
#include "bsdchann.h"      // base header

using namespace std;

/** public data **/

/** private data **/

/** public functions **/

/*******************************************************************
* Function Name: BSDChannel
* Description: constructor
********************************************************************/
BSDChannel::BSDChannel( const string &serviceName ) 
	: connection( -1 )
{
	servent *entry = getservbyname( serviceName.c_str(), "tcp" ) ;

	if( !entry )
	{
		CommunicationError e ;
		e.addLocation( MEXCEPTION_LOCATION() ) ;
		e.addText( serviceName + "/tcp not found in the services file (/etc/services)" ) ;
		throw e ;
	}

	this->open( ntohs( entry->s_port ) ) ;
}


/*******************************************************************
* Function Name: BSDChannel
* Description: constructor by portNumber
********************************************************************/
BSDChannel::BSDChannel( int portNumber ) 
	: connection( -1 )
{
	this->open( portNumber ) ;
}

/*******************************************************************
* Function Name: ~BSDChannel
* Description: destructor
********************************************************************/
BSDChannel::~BSDChannel()
{
	if( this->connection > 0 )
		close( connection ) ;
}


/*******************************************************************
* Function Name: open
* Description: open the communication 
********************************************************************/
BSDChannel &BSDChannel::open( int portNumber )
{
	int sockFD = socket( AF_INET, SOCK_STREAM, 0 ) ;

	if( sockFD < 0 )
	{
		CommunicationError e ;
		e.addLocation( MEXCEPTION_LOCATION() ) ;
		e.addText( "Can't open the stream socket!" ) ;
		throw e ;
	}

	sockaddr_in serverAddress, clientAddress ;

	bzero( reinterpret_cast<char*>( &serverAddress ), sizeof( serverAddress ) ) ;

	serverAddress.sin_family = AF_INET ;
	serverAddress.sin_addr.s_addr = htonl( INADDR_ANY ) ;
	serverAddress.sin_port = htons( portNumber ) ;

	if( bind( sockFD, reinterpret_cast< sockaddr* >( &serverAddress ), sizeof( serverAddress ) ) < 0 )
	{
		CommunicationError e ;
		e.addLocation( MEXCEPTION_LOCATION() ) ;
		e.addText( "Can't bind local address!" ) ;
		throw e ;
	}

	listen( sockFD, 5 ) ;

	while( true )
	{
		socklen_t cliLength = sizeof( clientAddress );

		connection = accept( sockFD, reinterpret_cast< sockaddr* >( &clientAddress ), &cliLength ) ;
		if( connection < 0 )
		{
			CommunicationError e ;
			e.addLocation( MEXCEPTION_LOCATION() ) ;
			e.addText( "Accept connection error!" ) ;
			throw e ;
		}

		int childPID = fork() ;
		if( childPID < 0 )
		{
			CommunicationError e ;
			e.addLocation( MEXCEPTION_LOCATION() ) ;
			e.addText( "Fork error!" ) ;
			throw e ;
		}

		if( childPID != 0 )
		{
			close( sockFD ) ;
			return *this ;
		}

		close( connection ) ;
	}
	return *this ;
}


/*******************************************************************
* Function Name: readLine
* Description: 
********************************************************************/
string BSDChannel::readLine()
{
	const int maxLine( 1024 );
	char buffer[maxLine];
	int index = 0;

	do
	{
		recv( connection, buffer+index, 1, 0 );   
	}  while( index < maxLine && buffer[index++] != '\n' );

	if( index == maxLine )
	{
		CommunicationError e ;
		e.addLocation( MEXCEPTION_LOCATION() ) ;
		e.addText( string( "The line exceeds " ) + maxLine + " characters!" ) ;
		throw e ;
	}

	if( index > 1 && buffer[ index-2 ] == '\r' )
		buffer[ index-2 ] = '\0';
	else
		buffer[ index-1 ] = '\0';

	return string( buffer );
}


/*******************************************************************
* Function Name: writeLine
* Description: 
********************************************************************/
CommChannel &BSDChannel::writeLine( const string &line )
{
	string aux( line + "\n" );

	if( send( connection, aux.c_str(), aux.length(), 0 ) != static_cast<int>( aux.length() ) )
	{
		CommunicationError e ;
		e.addLocation( MEXCEPTION_LOCATION() ) ;
		e.addText( "send error!" ) ;
		throw e ;
	}
	return *this ;
}
