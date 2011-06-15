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
#include "machineid.h"	//type MachineId
#include "except.h"
#include "tokit.h"

/** forward declarations **/
class istream;

/** declarations **/
class PIni
{
public:
	PIni() ;

	typedef list< string > IdList;
	typedef map< MachineId, IdList, less< int > > MachineList;

	PIni &parse( istream & );
	PIni &parse( const string & );

	PIni &save( ostream & );
	PIni &save( const string & );

	const MachineList &machines() const;
	const IdList &machine( const MachineId idMachine ) const;

	PIni &addMachine( const MachineId idMachine ) ;
	PIni &addDependent( const MachineId idMachine, const string &dependent ) ;
	

	bool exists( const MachineId idMachine) const ;


private:

	PIni( const PIni& ) ;
	MachineList machinesList;

	enum Token
	{
		def_TK,
		id_TK
	} ;

	Token readToken( TokenIterator &, string & ) ;

};


class PIniException : public MException
{
protected:
	PIniException( const string &str = "Parallel Ini Exception" ): MException( str )
	{} ;
};


class PIniParseException : public PIniException
{
public:
	PIniParseException() : PIniException( "Parsing Error in Parallel Ini File" )
	{};
};

class PIniRequestException : public PIniException
{
public:
	PIniRequestException ( const string &msg ) : PIniException( "Data not found!!!\n R(etry), I(gnore), P(anic) ?") {
		this->addText ( msg);
	};
};


/** inline **/
inline
PIni::PIni()
{};

inline
const PIni::MachineList &PIni::machines() const
{
	return machinesList;
}

#endif //  _PARINI_HPP







