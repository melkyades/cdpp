/*******************************************************************
*
*  DESCRIPTION: class Congestion
*
*  AUTHOR: Andrea Díaz && Verónica Vazquez
*
*  EMAIL: mailto://rodiaz@dc.uba.ar
*         mailto://vvazqu@dc.uba.ar
*
*  DATE: 24/01/2000
*
*******************************************************************/

#ifndef __CONGESTION_H
#define __CONGESTION_H

/** include files **/
#include <map.h>
#include "atomic.h"     // class Atomic
#include "except.h"     // class InvalidMessageException

class Congestion: public Atomic 
{
public:
	Congestion( const string &name = "Congestion"); // Default Constructor

	virtual string className() const;


protected:
  	Model &initFunction();
	Model &externalFunction(const ExternalMessage & );
	Model &internalFunction(const InternalMessage &);
	Model &outputFunction(const InternalMessage & );

private:
	const Port &IngresaAuto;
	const Port &SaleAuto;
	const Port &peso;
	const Port &stop;
	int cant_autos;
	
	
}; // class Congestion


// ** inline ** // 
inline
string Congestion::className() const
{
	return "Congestion" ;
}

inline

#endif   //__CONGESTION_H 