/*******************************************************************
*
*  DESCRIPTION: class PIni
*
*  AUTHOR:    Alejandro Troccoli 
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 17/8/2000
*
*******************************************************************/

#ifndef _PARINI_HPP
#define _PARINI_HPP

/** include files **/
#include <list>
#include <map>
#include "except.h"

/** forward declarations **/
class istream;

/** declarations **/
class PIni
{
public:
	PIni() ;

	typedef list< string > IdList;
	typedef map< int, IdList, less< int > > MachineList;

	string join( IdList & );

	Ini &parse( istream & );
	Ini &parse( const string & );

	Ini &save( ostream & );
	Ini &save( const string & );

	const MachineList &machines() const;
	const IdList &machine( const int idMachine ) const;

	Ini &addMachine( const int idMachine ) ;
	Ini &addDependent( const int idMachine, const string &dependent ) ;

	bool exists( const int idMachine) const ;


private:

	PIni( const Ini& ) ;
	MachineList machines;

};


class PIniException : public MException
{
protected:
	PIniException( const string &str = "Parallel Ini Exception" ): MException( str )
	{} ;
};


class PIniParseException : public IniException
{
public:
	PIniParseException() : IniException( "Parsing Error in Parallel Ini File" )
	{};
};



/** inline **/
inline
PIni::PIni()
{};

inline
const PIni::MachineList &PIni::machineList() const
{
	return machines;
}

#endif //  _PARINI_HPPpi
