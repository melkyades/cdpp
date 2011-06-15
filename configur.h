/*******************************************************************
*
*  DESCRIPTION: class Configurator
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __CONFIGURATOR_H
#define __CONFIGURATOR_H

/** include files **/
#include <string>

/** my include files **/
#include "singleton.h"        // template Singleton< T >

/** forward declarations **/
class Configurator ;

/** declarations **/
typedef Singleton< Configurator > SingleConfigurator ;

class Configurator
{
public:
	virtual ~Configurator();	//Destructor

	Configurator &iniFileName( const string & ) ;

	const IniFile &iniFile() const ;

protected:
	friend SingleConfigurator ;

	Configurator();	// Default constructor

	Configurator(const Configurator&);	//Copy constructor
	Configurator& operator=(const Configurator&);	//Assignment operator
	int operator==(const Configurator&) const;	//Equality operator

private:

};	// class Configurator

/** inline **/

#endif   //__CONFIGURATOR_H 
