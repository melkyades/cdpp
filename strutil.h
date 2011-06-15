/*******************************************************************
*
*  DESCRIPTION: String utility functions
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 18/02/1999 (v2)
*
*******************************************************************/

#ifndef __STRUTIL_H
#define __STRUTIL_H

#include <ctype.h>   // tolower 
#include <stdio.h>
#include <hash_map>
#include "real.h"

string lowerCase( const string &str );
float str2float( const string &str );
Value str2Value( const string &str );

inline string Value2StrReal( const double f)
{ Real r(f); return r.asString(); }

inline float char2float( const char c )
{return c - '0';}

inline Real str2Real( const string &str )
{ return str[0] == '?' ? Real() : Real( str2Value(str)); }

int str2Int( const string &str );
string int2Str( int val );
bool isNumeric( const string &str);
bool isDigitOrSpace( const string &str );
string trimSpaces( const string &str );
string trimSpaces( const char *str );
string substringLenght( const string str, int len );
string upcase( const string str );

class HashString
{
public:
	size_t operator()(const string &str) const
	{
		hash<const char*> h;
		return (h(str.c_str()));
	}
};


#endif   //__STRUTIL_H
