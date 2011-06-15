/*******************************************************************
*
*  DESCRIPTION: class Ini
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel A. Rodriguez
*  Colon bug: Alejandro Lopez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*	  mailto://drodrigu@dc.uba.ar
*         mailto://aplopez@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 20/3/1999 (v2)
*  DATE: 15/6/2001 (colon bug)
*
*******************************************************************/

#ifndef _INI_HPP
#define _INI_HPP

/** include files **/
#include <list>
#include <map>
#include "except.h"
#include "tokit.h"

/** forward declarations **/
class istream;

/** declarations **/
class Ini
{
public:
	Ini() ;

	typedef list< string > IdList;
	typedef map< string, IdList, less< string > > DefList;
	typedef map< string, DefList, less< string > > GroupList;

	string join( IdList & );

	Ini &parse( istream & );
	Ini &parse( const string & );

	Ini &save( ostream & );
	Ini &save( const string & );

	const GroupList &groupList() const;
	const DefList &group( const string & ) const;
	const IdList &definition( const string &groupName, const string &defName ) const;

	Ini &addGroup( const string &groupName ) ;
	Ini &addDefinition( const string &groupName, const string &def ) ;
	Ini &addId( const string &groupName, const string &def, const string &id ) ;

	bool exists( const string &groupName, const string &defName ) const ;
	bool exists( const string &groupName ) const ;

private:
	Ini( const Ini& ) ;
	GroupList groups;

	enum Token
	{
		group_TK,
		def_TK,
		id_TK
	} ;

	Token readToken( TokenIterator &, string & ) ;
        bool isNumber( const string & );
};


class IniException : public MException
{
protected:
	IniException( const string &str = "Ini Exception" ): MException( str )
	{} ;
};


class IniParseException : public IniException
{
public:
	IniParseException() : IniException( "Parsing Error in Ini File" )
	{};
};


class IniRequestException : public IniException
{
public:
	IniRequestException( const string &msg ) : IniException( "Data not found!!!\nR(etry), I(gnore), P(anic)?" ) 
	{
		this->addText( msg );
	};
};

/** inline **/
inline
Ini::Ini()
{};

inline
const Ini::GroupList &Ini::groupList() const
{
	return groups;
}

#endif //  _INI_HPP
