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
class std::istream;

/** declarations **/
class PIni
{
public:
	PIni() ;

	typedef std::list< std::string > IdList;
	typedef std::map< int, IdList, std::less< int > > MachineList;

	std::string join( IdList & );

	Ini &parse( std::istream & );
	Ini &parse( const std::string & );

	Ini &save( std::ostream & );
	Ini &save( const std::string & );

	const MachineList &machines() const;
	const IdList &machine( const int idMachine ) const;

	Ini &addMachine( const int idMachine ) ;
	Ini &addDependent( const int idMachine, const std::string &dependent ) ;

	bool exists( const int idMachine) const ;


private:

	PIni( const Ini& ) ;
	MachineList machines;

};


class PIniException : public MException
{
protected:
	PIniException( const std::string &str = "Parallel Ini Exception" ): MException( str )
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
