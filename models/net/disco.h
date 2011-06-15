/*******************************************************************
*
*  DESCRIPCION: clase DISCO (procesa pedidos a disco de un tamaño deter-
*               minado (pasado como mensaje)
*
*  AUTORES:
*  	    Ing. Carlos Giorgetti
*          Iván A. Melgrati
*          Dra. Ana Rosa Tymoschuk
*	    Alejandro Troccoli (v2)
*
*  EMAIL: mailto://cgiorget@frsf.utn.edu.ar
*         mailto://imelgrat@frsf.utn.edu.ar
*         mailto://anrotym@alpha.arcride.edu.ar
*	   mailto://atroccol@dc.uba.ar (v2)
*
*  FECHA: 09/10/1999
*	   05/01/2001 (v2)
*
*******************************************************************/

#ifndef __DISCO_H
#define __DISCO_H

/** Archivos de Encabezado **/
#include "atomic.h"      // class Atomic
#include "mformats.h"    // formato de mensaje a pasar
#include "distri.h"        // class Distribution

/** declarations **/
class DiscoState:public AtomicState {
public:	
	union request requerimiento;      // trabajo a enviar (a los Discos o CPU)

	DiscoState(){};
	virtual ~DiscoState(){};

	DiscoState& operator=(DiscoState& thisState); //Assignment
	void copyState(DiscoState *);
	int  getSize() const;

};


class Disco: public Atomic
{
public:
	Disco( const string &name = "Disco" );          		// Constructor
	virtual ~Disco() {delete dist;}			   			// Destructor
	virtual string className() const {return "Disco";}

protected:
	Model &initFunction() {return *this;}                   // Función de Inicialización

	Model &externalFunction( const ExternalMessage & );     // Función de Tr. Externa

	Model &internalFunction( const InternalMessage & );     // Función de Tr. Interna

	Model &outputFunction( const CollectMessage & );       // Función de Salida

	ModelState* allocateState() 
	{return new DiscoState;}

private:
	const Port &in ;
	Port &out ;
	Distribution *dist ;           							// Generador de números aleatorios
	float velocidad;										// Tiempo en leer 1 KB
	Distribution &distribution(){return *dist;}

	union request& requerimiento();							// Requrimiento actual
};	// class Disco

/*******************************************************************
* Shortcuts to state paramters
*********************************************************************/
inline 
union request& Disco::requerimiento() {
	return ((DiscoState*)getCurrentState())->requerimiento;
}

#endif   //__DISCO_H 
