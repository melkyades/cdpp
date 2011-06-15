/*******************************************************************
*
*  DESCRIPTION: exceptions definition
*               class InvalidModelIdException
*               class InvalidMessageException
*               class MException (base class)
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel A. Rodriguez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 27/2/1999 (v2)
*
*******************************************************************/

#ifndef _MEXCEPT_HPP
#define _MEXCEPT_HPP

/** include files **/
#include <list.h>
#include <iostream.h>
#include <stdlib.h>
#include "stringp.h"     // operator +( const string &, int ) ;

/** definitions **/
#define MEXCEPTION_LOCATION() ( string("File: ") + __FILE__ + " - Method: " + __FUNCTION__ + " - Line: " + __LINE__ )

#define MASSERT(lexp) if( !( lexp ) ) { AssertException e( "Invalid assertion" ) ; e.addText( #lexp ) ; e.addLocation( MEXCEPTION_LOCATION() ) ; throw e ; }
#define MASSERTMSG(lexp,text) if( !( lexp ) ) { AssertException e( "Invalid assertion" ) ; e.addText( #lexp ) ; e.addText( text ) ; e.addLocation( MEXCEPTION_LOCATION() ) ; throw e ; }

#define MASSERT_ERR(str)  { cout << str; exit(-1); }

#define MTHROW( e ) e.addLocation( MEXCEPTION_LOCATION() ) ; throw e ;

class MException
{
public:
	// ** Constructors ** //
	MException( const string &description = "" )
	: descr( description )
	{}

	MException( const MException & ) ;
	virtual ~MException() ;

	const string &description() const
	{return descr;}

	MException &addText( const string & ) ;
	MException &addLocation( const string & ) ;

	typedef list< string > TextList ;
	typedef list< string > LocationList ;

	const TextList &textList() const
	{return texts;}

	const LocationList &locationList() const
	{return locations;}

	ostream &print( ostream & ) const ;

	virtual const string type() const
	{return "MException";}

private:
	string descr ;
	TextList texts ;
	LocationList locations ;

};	// MException

class AssertException: public MException
{
public:
	// ** Constructors ** //
	AssertException( const string &description = ""): MException( description )
	{};

	const string type() const
	{return "AssertException";}
};	// AssertException


class InvalidMessageException: public MException
{
public:
	const string type() const
	{return "InvalidMessageException";}
};	// InvalidMessageException


class InvalidModelIdException: public MException
{
public:
	const string type() const
	{return "InvalidModelIdException";}
};	// InvalidModelIdException

class InvalidProcessorIdException: public MException
{
public:
	const string type() const
	{return "InvalidProcessorIdException";}
};	// InvalidProcessorIdException

class InvalidAtomicTypeException: public MException
{
public:
	const string type() const
	{return "InvalidAtomicTypeException";}
};	// InvalidAtomicTypeException

class InvalidPortRequest: public MException
{
public:
	const string type() const
	{return "InvalidPortRequest";}
};	// InvalidPortRequest


/********************/
/** inline methods **/
/********************/

inline
	MException::MException( const MException &me ) 
	: descr( me.description() )
	, texts( me.texts )
	, locations( me.locations )
{}

inline
	MException::~MException()
{}

inline
	MException &MException::addText( const string &str )
{
	texts.push_back( str ) ;
	return *this ;
}

inline
	MException &MException::addLocation( const string &str )
{
	locations.push_back( str ) ;
	return *this ;
}

inline
	ostream &operator << (ostream &os, const MException &e)
{
	e.print( os ) ;
	return os ;
}

#endif // _MEXCEPT_HPP
