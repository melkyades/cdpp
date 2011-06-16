/*******************************************************************
*
*  DESCRIPTION: IniTest program
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#include <iostream>
#include "ini.h"
#include "prnutil.h"

using namespace std;

int main( int argc, const char *argv[] )
{
	if( argc != 2 )
	{
		cout << "Uso: " << argv[0] << " nombre del ini " << endl ;
		exit(1) ;
	}

	Ini ini ;

	ini.parse( argv[1] ) ;

	cout <<  ini.groupList() ;

	return 0 ;
}
