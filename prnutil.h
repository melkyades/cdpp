/*******************************************************************
*
*  DESCRIPTION: 
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __PRNUTIL_H
#define __PRNUTIL_H

/** include files **/
#include <iostream.h>
#include <map>
#include <pair.h>
#include <list>

/** inline **/

template< class T1, class T2 >          
inline
ostream &operator<<( ostream &out,  const map< T1, T2, less< T1 > > &c )
{

	map< T1, T2, less< T1 > >::const_iterator cursor( c.begin() ) ;

	for( ; cursor != c.end() ; cursor ++ )
		out << cursor->first << " = " << cursor->second << endl ;

	return out;
}

template< class T >
inline
ostream &operator<<( ostream &out, const list< T > &l )
{
	list< T >::const_iterator cursor( l.begin() ) ;

	for( ; cursor != l.end() ; cursor ++ )
		out << *cursor << " " ;

	return out;
}

template< class T1, class T2 >
inline
ostream &operator <<( ostream &out, const pair<T1, T2> &p )
{
	out << "First: " << p.first << endl << "Second: " << p.second << endl ;  
	return out ;
}

#endif   //__PRNUTIL_H 
