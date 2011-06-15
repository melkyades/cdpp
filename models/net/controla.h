/*******************************************************************
*
*  DESCRIPCION: clase CONTROLADORA (Rutea los pedidos a los Discos (sin Espera))
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

#ifndef __CONTROLADORA_H
#define __CONTROLADORA_H

/** Archivos de Encabezado **/
#include "atomic.h"      // class Atomic
#include "mformats.h"    // formato de mensaje a pasar
#include "distri.h"      // Class Distribution

/** declaraciones **/

const int pasiva = 0;             // Fases del modelo
const int sendCPU = 1;
const int senddisk = 2;

class ControladoraState:public AtomicState {
public:	
	char fase;                        // Fase del Modelo
	union request requerimiento;      // trabajo a enviar (a los Discos o CPU)

	ControladoraState(){};
	virtual ~ControladoraState(){};

	ControladoraState& operator=(ControladoraState& thisState); //Assignment
	void copyState(ControladoraState *);
	int  getSize() const;

};

class Controladora: public Atomic
{
public:
	Controladora( const string &name = "Controladora" );    	    // Constructor
	virtual ~Controladora(){delete dist;} 			  				//Destructor
	virtual string className() const {return "Controladora";}       // Devuelve el tipo de modelo

protected:
	Model &initFunction() { return *this; }

	Model &externalFunction( const ExternalMessage & );

	Model &internalFunction( const InternalMessage & );

	Model &outputFunction( const CollectMessage & );

	Distribution &distribution()  {return *dist;}

	ModelState* allocateState() 
	{ return new ControladoraState;}
private:
	const Port &in ;                  // Entrada desde CPU
	Port &out ;                       // Salida a la CPU
	const Port &discoin;              // Entrada de los Discos (solicitud terminada)
	Port &discoout1;                  // Salidas a los Discos
	Port &discoout2;
	Port &discoout3;
	Port &discoout4;
	Port &discoout5;
	Port &discoout6;
	Port &discoout7;
	Port &discoout8;

	unsigned int   cant;		      // Cantidad de discos reales
	Distribution *dist ;              // Distribución de tamaños de petición

	//Shortcuts to state
	char fase() const;
	void fase( char );

	union request& requerimiento();

};	// class Controladora

/*******************************************************************
* Shortcuts to state paramters
*********************************************************************/
inline 
char Controladora::fase() const {
	return ((ControladoraState*)getCurrentState())->fase;
}

inline
void Controladora::fase( char f) {
	((ControladoraState*)getCurrentState())->fase = f;
}

inline 
union request& Controladora::requerimiento() {
	return ((ControladoraState*)getCurrentState())->requerimiento;
}

#endif   //__CONTROLADORA_H
