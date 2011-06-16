/*******************************************************************
*
*  DESCRIPTION: PIniTest program
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*
*  DATE: 25/8/2000
*
*******************************************************************/

#include <iostream>
#include "pini.h"
#include "prnutil.h"

using namespace std;

int main( int argc, const char *argv[] )
{
	if( argc != 2 )
	{
		cout << "Uso: " << argv[0] << " nombre del archivo .par " << endl ;
		exit(1) ;
	}

	PIni pini ;

	pini.parse( argv[1] ) ;

	cout <<  pini.machines() ;

	return 0 ;
}
