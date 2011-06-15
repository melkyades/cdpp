/*******************************************************************
*
*  DESCRIPTION: class MacroExpansion
*
*  AUTHOR: Daniel A. Rodriguez 
*
*  EMAIL: mailto://drodrigu@dc.uba.ar
*
*  DATE: 25/7/1999 (v2)
*
*******************************************************************/

#ifndef __MACROEXPANSION_H
#define __MACROEXPANSION_H

/** include files **/
#include <list>
#include <stdlib.h>
#include <string.h>
#include "except.h"

/** declarations **/
class macroExpansion
{
public:
	macroExpansion(string fileName);	// Constructor

	~macroExpansion();			// Destructor
	
	string expand();			// Make the Macro Expansion

	string tempFileName()			// Devuelve el nombre del
		{return newFileName;}		// archivo temporario creado
						// por Expand.	
private:
	int isInclude( char *line );
	int isInclude( char *line, long posLine );

	int isMacro( char *line )
			{ return isMacro(line, 0); }
			
	int isMacro( char *line, long posLine );
	int isEndOfMacro( char *line );
	int isBeginOfMacro( char *line );
	
	string getData( char *line, string macroCmdName )
			{ return getData( line, 0, macroCmdName); }

	string getData( char *line, long posLine, string macroCmdName );
	
	void addIncludeFile( string file );
	void putMacroExpansion( FILE *fileOut, string macroName );
	int  putMacroExpansionInFile( FILE *fileOut, string macroName, string macroFile );
	int  copyMacro( FILE *fileOut, FILE *fileIn, char *macroLine, string macroName);

	///////////////////////
	// Internal structure
	///////////////////////
	string		fileName;
	string		newFileName;
	list<string>	includeFiles;
	
}; // macroExpansion


extern macroExpansion	*instanceMacroExpansion;

inline macroExpansion &MacroExpansion(string fileName = "")
{
	if (instanceMacroExpansion == NULL)
		instanceMacroExpansion = new macroExpansion(fileName);
		
	return *instanceMacroExpansion;
}


#endif	// macroExpansion
