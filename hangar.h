#ifndef __modulo_H
#define __modulo_H


#include "atomic.h"     // class Atomic

class modulo : public Atomic
{
public:
        modulo( const string &name = "modulo" );                                  //Default constructor
	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
        const Port &recibe_avion; // Port de entrada de avion
        Port &emergencia;        // Port de Salida en cado de accidente
        Port &salida_hangar;    //  Port de Salida en cado de accidente
        Port &en_uso      ;     //  Port de Salida en cado de accidente
        Port &sigue_avion ;     //  Port de Salida en cado de accidente


       VTime preparationTime;

        VTime tiempo_sobre_modulo ; //tiempo que el avion esta sobre el modulo
        float  avion_numero;       // el identificador del Avion
        float velocidad;
};      // class modulo

// ** inline ** // 
inline
string modulo::className() const
{
        return "modulo" ;
}


#endif   //__modulo_H
