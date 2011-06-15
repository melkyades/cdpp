/******************************************************************************
* Model: 	Airport
* Component:	Modulo
* Author:	Luis De Simoni
*		Alejandro Troccoli (v2)
*
* Mail:	mailto://atroccol@dc.uba.ar
*
* DATE:	02/02/2001
********************************************************************************/
#ifndef __modulo_H
#define __modulo_H


#include "atomic.h"     // class Atomic

class moduloState : public AtomicState {

public: 
	string accion;

	moduloState(){};
	virtual ~moduloState(){};

	moduloState& operator=(moduloState& thisState); //Assignment
	void copyState(moduloState *);
	int  getSize() const;

};

class modulo : public Atomic
{
public:
	modulo( const string &name = "modulo" );                                  //Default constructor
	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const CollectMessage & );

	ModelState* allocateState() {
		return new moduloState;
	}

private:
	const Port &aterriza; // Port de entrada de avion
	const Port &despega ; // Port de entrada de avion
	Port &salida_a      ; //  Port de Salida en cado de accidente
	Port &salida_d      ; //  Port de Salida en cado de accidente


	void accion (const string& s);
	const string& accion() const;

	VTime Pista_en_uso;



	float  avion_numero;       // el identificador del Avion

};      // class modulo

// ** inline ** // 
inline
	string modulo::className() const
{
	return "modulo" ;
}


/*******************************************************************
* Shortcuts to state paramters
*********************************************************************/
inline
const string &modulo::accion() const {
	return ((moduloState*)getCurrentState())->accion;	
}

inline 
void modulo::accion(const string& s) {
	((moduloState*)getCurrentState())->accion = s;
}

#endif   //__modulo_H
