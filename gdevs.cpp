/*******************************************************************
*
*  DESCRIPTION: Atomic Model Gdevs
*
*  AUTHOR: AMD & GC
*
*  EMAIL: 
*
*  DATE: 21/11/1999
*
*******************************************************************/

/** include files **/
#include "gdevs.h"      // class Gdevs
#include "message.h"    // class ExternalMessage, InternalMessage
#include "parsimu.h"   // ParallelMainSimulator::Instance().getParameter( ... )

/** public functions **/

/*******************************************************************
* Function Name: Gdevs
* Description: 
********************************************************************/
Gdevs::Gdevs( const string &name )
: Atomic( name )
{
    // myGp contiene la estructura del modelo DEVS
    myGP = new GdevsParser(name);

  
    // agrego los ports del modelo a la simulación
    list<GdevsParser::GdevsPort>::iterator cursor2;
    for( cursor2 = myGP->inports.begin() ; cursor2 != myGP->inports.end() ; cursor2 ++ )
        addInputPort( *cursor2 );

    for( cursor2 = myGP->outports.begin() ; cursor2 != myGP->outports.end() ; cursor2 ++ )
        addOutputPort( *cursor2 );

}

/*******************************************************************
* Function Name: ~Gdevs
* Description: Destructor
********************************************************************/
Gdevs::~Gdevs()
{
    delete myGP;
}
/*******************************************************************
* Function Name: initFunction
* Description: Resetea la lista
* Precondition: El tiempo del proximo evento interno es Infinito
********************************************************************/
Model &Gdevs::initFunction()
{

    myGP->estadoActual = myGP->burbujas.front();
    return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description: 
********************************************************************/
Model &Gdevs::externalFunction( const ExternalMessage &msg )
{
    GdevsParser::GdevsEstado estado; 
    estado = myGP->externa( myGP->estadoActual, msg.port().name(), msg.value() );
    if( estado != "" ) {
		myGP->estadoActual = estado;
        cout << "Transicion EXTERNA, Hora: " << msg.time() << " ,estado destino:"<< myGP->estadoActual<< endl  ;
	    GdevsParser::GdevsEstado estado1 = myGP->interna( myGP->estadoActual );
		if( estado1 != "" ) {
			cout << "Luego de la transición se queda en holdin con estado destino " << estado1 << endl<< endl;
			//cout << "Timeout:" << myGP->timeout << endl;
			holdIn(active, myGP->timeout );
		} else {
			//cout << "sin timeout" << endl;
			passivate();
		}
	}
    else cout << "Transición externa no definida!" << endl;
    
//    cout << "Transicion EXTERNA, Hora: " << msg.time() << " ,estado destino:"<< myGP->estadoActual<< endl << endl ;
    
    return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description: 
********************************************************************/
Model &Gdevs::internalFunction( const InternalMessage &msg )
{
	// cout << "Empieza internal function" << endl;
	GdevsParser::GdevsEstado estado; 
    estado = myGP->interna( myGP->estadoActual );
    if( estado != "" ) {
		myGP->estadoActual = estado;
        cout << "Transicion INTERNA, Hora: " << msg.time() << " ,estado destino:"<< myGP->estadoActual<< endl ;

		GdevsParser::GdevsEstado estado1 = myGP->interna( myGP->estadoActual );
		if( estado1 != "" ) {
			cout << "Luego de la transición se queda en holdin con estado destino " << estado1 << endl<< endl;
			//cout << "Timeout:" << myGP->timeout << endl;
			holdIn(active, myGP->timeout );
		} else {
			//cout << "sin timeout" << endl;
			passivate();
		}
	}
    else cout << "Transición interna no definida!" << endl;

    //cout << "Transición interna- Estado post trans. :"<< myGP->estadoActual<< endl <<endl ;

    return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description: 
********************************************************************/
Model &Gdevs::outputFunction( const InternalMessage &msg )
{
	GdevsParser::GdevsPort portname; 
	int valor; 
	myGP->salida( myGP->estadoActual, portname, valor);
    sendOutput( msg.time(), port(portname), valor) ;
    return *this ;
}

// copylist
// copia una lista de Ini::IdList a una. 
// El tipo de la lista destino se especifica como Template
template <class T> static copylist( const Ini::IdList &l, list<T> &lista2 )
{   
    list<T>::const_iterator cursor ;

    lista2.insert (lista2.end(), l.begin(), l.end());
    
    // descomentar para ver el resultado
    //for( cursor = lista2.begin() ; cursor != lista2.end() ; cursor ++ )
    //{
    //  cout << '[' << (*cursor) << ']' << endl ;
    //}
    
}

template <class T> static int find( const T &e, list<T> &x )
{   
    list<T>::const_iterator cursor ;
	int status = 0;

    for( cursor = x.begin() ; cursor != x.end() ; cursor ++ )
    {
		if ( e == *cursor ) {
			status = 1;
			break;
		}
	}
	return status;

}

// Parsea las transiciones externas del archivo ini.
GdevsParser::parseTransExt( )
{   
    const Ini::IdList &l = ParallelMainSimulator::Instance().getParameters(modelName, "ext");
    Ini::IdList::const_iterator cursor ;

    // recorre las transiciones por separado
    int campo=0;
    transicion t;
    for( cursor = l.begin() ; cursor != l.end() ; cursor ++ )
    {
        switch(campo) {
            case 0: // origen
				if( find<GdevsEstado>( *cursor, burbujas ) ) {
	                t.origen = *cursor;
				} else // error
					cout << "Estado origen invalido : " << *cursor << endl;
                break;
            case 1: // destino
				if( find<GdevsEstado>( *cursor, burbujas ) ) {
	                t.destino = *cursor;
				} else // error
					cout << "Estado destino invalido : "  << *cursor << endl;
                break;
            case 2: // port
				if( find<GdevsPort>( *cursor, inports ) ) {
	                t.port = *cursor;
				} else // error
					cout << "Port invalido : " << *cursor << endl;
                break;
            case 3: // valor
				if( atoi((*cursor).c_str()) > 0 ) {
					t.valor = atoi((*cursor).c_str());
				} else // error
					cout << "Valor inválido : "  << *cursor << endl;

                transExterna.insert(transExterna.end(), t);
                break;
        }
        campo++;
        campo %= 4;
        
    }

	if (campo != 0)
		cout << "Transición incompleta" << endl;

    // descomente para ver las transiciones
    //list<transicion>::iterator cursor2;
    //for( cursor2 = transExterna.begin() ; cursor2 != transExterna.end() ; cursor2 ++ )
    //  cout << "Externa=Origen:" << ((*cursor2).origen) << ". Destino:"
    //  << ((*cursor2).destino) << ". Port:"<< ((*cursor2).port) 
    //  << ". Valor:" << ((*cursor2).valor) << endl ;
    
}

// Parsea las transiciones externas del archivo ini.
GdevsParser::parseTransInt( )
{   
    const Ini::IdList &l = ParallelMainSimulator::Instance().getParameters(modelName, "int");
    Ini::IdList::const_iterator cursor ;

    //lista2.insert (lista2.end(), l.begin(), l.end());
    
    int campo=0;
    transicion t;
    for( cursor = l.begin() ; cursor != l.end() ; cursor ++ )
    {
        switch(campo) {
            case 0: // origen
				if( find<GdevsEstado>( *cursor, burbujas ) ) {
	                t.origen = *cursor;
				} else // error
					cout << "Estado origen invalido : " << *cursor << endl;
                break;
            case 1: // destino
				if( find<GdevsEstado>( *cursor, burbujas ) ) {
	                t.destino = *cursor;
				} else // error
					cout << "Estado destino invalido : "  << *cursor << endl;
                break;
            case 2: // port
				if( find<GdevsPort>( *cursor, outports ) ) {
	                t.port = *cursor;
				} else // error
					cout << "Port invalido : " << *cursor << endl;
                break;
            case 3: // valor
				if( atoi((*cursor).c_str()) > 0 ) {
					t.valor = atoi((*cursor).c_str());
				} else // error
					cout << "Valor inválido : "  << *cursor << endl;

                transInterna.insert(transInterna.end(), t);
                break;
        }
        campo++;
        campo %= 4;
        
    }

	if (campo != 0)
		cout << "Transición incompleta" << endl;

    // descomente para ver las transiciones
    //list<transicion>::iterator cursor2;
    //for( cursor2 = transInterna.begin() ; cursor2 != transInterna.end() ; cursor2 ++ )
    //  cout << "Interna=Origen:" << ((*cursor2).origen) << ". Destino:"
    //  << ((*cursor2).destino) << ". Port:"<< ((*cursor2).port) 
    //  << ". Valor:" << ((*cursor2).valor) << endl ;
    
}




GdevsParser::loadData() {
    copylist<GdevsParser::GdevsEstado>(ParallelMainSimulator::Instance().getParameters(modelName, "burbuja"), burbujas);
    copylist<GdevsParser::GdevsPort>(ParallelMainSimulator::Instance().getParameters(modelName, "inport"), inports);
    copylist<GdevsParser::GdevsPort>(ParallelMainSimulator::Instance().getParameters(modelName, "outport"), outports);
    parseTransExt();
    parseTransInt();
}

// busca un transición externa que parta de estado y con entrada valor por port. Devuelve la
// primera transición que encuentra.
GdevsParser::GdevsEstado GdevsParser::externa( GdevsEstado estado, GdevsPort port, int valor )
{
	//cout << "entro en GP externa" << endl;
    list<transicion>::const_iterator cursor ;
    int hallo=0;
    for( cursor = transExterna.begin() ; cursor != transExterna.end() ; cursor ++ )
    {
		//cout << "valor(msg,cursor)" << valor <<" , " << (*cursor).valor << endl;
		if( estado == estadoActual 
            && estado == (*cursor).origen 
            && port == (*cursor).port 
            && valor == (*cursor).valor
            ) {
            hallo = 1;
            break;
        }
        //cout << "port: " << port  << " valor (msg , cursor): " << valor <<","<< (*cursor).valor <<  endl;
    }
    if( hallo ) {
        //cout << "SALIO !!!  port: " << port  << " valor: " << valor << " destino: " << (*cursor).destino << endl;
		return (*cursor).destino;
	}
	else {
		//cout << "salio por no hallado de GP externa" << endl;
		return "";
    }
}

// busca la transición interna que sale de estado
GdevsParser::GdevsEstado GdevsParser::interna( GdevsEstado estado )
{
    list<transicion>::const_iterator cursor ;
    int hallo=0;
	//cout << "entro en GP interna" << endl;
    for( cursor = transInterna.begin() ; cursor != transInterna.end() ; cursor ++ )
    {
        if( estado == (*cursor).origen
			) {
            hallo = 1;
            break;
        }
		//cout << "Estado (inicial, destino)" << estado <<","<< (*cursor).destino <<  endl;
    }
    if( hallo ) {
        //cout << "SALIO !!! Estado (inicial, destino)" << estado <<","<< (*cursor).destino  << endl;
		return (*cursor).destino;
	}
    else return "";
}


// devuelve el port y el valor de salida de la transición interna que sale del estado

GdevsParser::salida( GdevsEstado estado, GdevsPort &port, int &valor )
{
    list<transicion>::const_iterator cursor ;
    int hallo=0;
    for( cursor = transInterna.begin() ; cursor != transInterna.end() ; cursor ++ )
    {
        if( estado == (*cursor).origen
			) {
            hallo = 1;
            break;
        }
    }
    if( hallo ) {
		port = (*cursor).port;
		valor = (*cursor).valor;
	}

	// return "";
}

/*const Port &Gdevs::PortByName( string &name )
{
    PortList::const_iterator cursor ;
    int hallo=0;
    for( cursor = outputPorts().begin() ; cursor != outputPorts().end() ; cursor ++ )
    {
        if( name == (*cursor).name()
			) {
            hallo = 1;
            break;
        }
    }
    if( hallo ) {
		return (*cursor);
	} else return NULL;

	// return "";
}
*/