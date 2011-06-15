/*******************************************************************
*
*  DESCRIPCION: Client (Genera las solicitudes a los servidores)
*
*  AUTORES:
*  		Ing. Carlos Giorgetti
*          	Iván A. Melgrati
*          	Dra. Ana Rosa Tymoschuk
*
*	v2:	Alejandro Troccoli
*
*  EMAIL: mailto://cgiorget@frsf.utn.edu.ar
*         mailto://imelgrat@frsf.utn.edu.ar
*         mailto://anrotym@alpha.arcride.edu.ar
*         mailto://atroccol@dc.uba.ar (v2)
*
*  FECHA: 19/10/1999
*	   05/02/2001 (v2)
*******************************************************************/
#ifndef __CLIENT_H
#define __CLIENT_H
#include "atomic.h"
#include "except.h"
#include "mformats.h"
#include "distri.h"

class clientState : public AtomicState {

public:

	//Id task
	int idtask;

	clientState(){};
	virtual ~clientState(){};

	clientState& operator=(clientState& thisState); //Assignment
	void copyState(clientState *);
	int  getSize() const;

};

class Client : public Atomic
{
public:
	Client( const string &name = "Client" );				  // Default constructor

	virtual ~Client(){delete dist;}

	virtual string className() const  {return "Client";}

protected:
	Model &initFunction() ;

	Model &externalFunction( const ExternalMessage & )    // No puede recibir mensajes
	{throw InvalidMessageException();}                // ya que no tiene entradas

	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const CollectMessage & );

	ModelState* allocateState() 
	{return new clientState;}

private:
	union request pid;
	int initial, increment;
	float cargaserver1;
	int IPAddress;

	Port &out ;

	Distribution *dist;
	Distribution &distribution(){return *dist;}

	int idtask() const;
	void idtask(int );


};	// class Client

/*******************************************************************
* Shortcuts to state paramters
*********************************************************************/
inline 
int Client::idtask() const {
	return ((clientState*)getCurrentState())->idtask;
}

inline
void Client::idtask( int p) {
	((clientState*)getCurrentState())->idtask = p;
}


#endif   //__CLIENT_H
