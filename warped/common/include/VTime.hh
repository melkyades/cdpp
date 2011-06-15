/*******************************************************************
*
*  DESCRIPTION: class VTime
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*          Alejandro Troccoli (v2)
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*	 04/11/2000	(v2)
*
*******************************************************************/

#ifndef __VTIME_H
#define __VTIME_H

/** include files **/
#include <string> // class string

/** definitions **/
typedef int Hours ;
typedef int Minutes ;
typedef int Seconds ;
typedef int MSeconds ;

#define PINFINITY VTime::Inf
#define INVALIDTIME VTime::InvalidTime
#define ZERO VTime::Zero

class VTime
{
public:
	VTime( Hours h = 0, Minutes m = 0, Seconds s = 0, MSeconds ms = 0 )
		: hour( h )
		, min( m )
		, sec( s )
		, msec( ms )
		{ normalize(); }

	VTime( const string &t )
		{ makeFrom(t); }
		
	VTime( float mseconds )
		: hour( 0 )
		, min( 0 )
		, sec( 0 )
		, msec( 0 )
		{ makeFrom( mseconds ); }
	
	VTime( const VTime & ) ; // copy constructor

	// ** Modifiers ** //
	VTime &hours( const Hours & ) ;
	VTime &minutes( const Minutes & ) ;
	VTime &seconds( const Seconds & ) ;
	VTime &mseconds( const MSeconds & ) ;

	// ** Queries ** // 
	const Hours    &hours() const ;
	const Minutes  &minutes() const ;
	const Seconds  &seconds() const ;
	const MSeconds &mseconds() const ;

	VTime operator +( const VTime & ) const ;	// addition operator
	VTime operator -( const VTime & ) const ;	// substraction operator
	VTime operator * (const int &mult) const;	// multiplication by an integer
	VTime operator / (const int &div) const;	//division by an integer

	
	VTime &operator =( const VTime & ) ;			// assignment operator

	bool operator ==( const VTime & ) const ;	// Equality test


	VTime &operator =( const string &t )		// assignment operator
		{makeFrom(t); return *this;}
	
	VTime &operator -=( const VTime &t ) ;
	VTime &operator +=( const VTime &t ) ;


	bool operator <( const VTime & ) const ;	// comparission operator 

	string asString() const ;

	float asMsecs() const
		{ return mseconds() + seconds() * 1000 + minutes() * 60000 + hours() * 3600 * 1000; }

	friend istream &operator >>( istream &is, VTime &t );

	
	static const VTime Zero ;
	static const VTime Inf  ;
	static const VTime InvalidTime;

private:
	Hours hour  ;
	Minutes min ;
	Seconds sec ;
	MSeconds msec ;

	VTime &makeFrom( const string & ) ;

	VTime &makeFrom( float miliseconds ) ;
	VTime &normalize() ;
	VTime &adjust( int &, int &, int ) ;
};

/** inline methods **/
inline
const Hours &VTime::hours() const
{
	return hour ;
}

inline
const Minutes &VTime::minutes() const
{
	return min ;
}

inline
const Seconds &VTime::seconds() const
{
	return sec ;
}

inline
const MSeconds &VTime::mseconds() const
{
	return msec ;
}

inline
VTime &VTime::hours( const Hours &h )
{
	hour = h ;
	return *this ;
}

inline
VTime &VTime::minutes( const Minutes &m )
{
	min = m ;
	normalize() ;
	return *this ;
}

inline
VTime &VTime::seconds( const Seconds &s )
{
	sec = s ;
	normalize() ;
	return *this ;
}

inline
VTime &VTime::mseconds( const MSeconds &ms )
{
	msec = ms ;
	normalize() ;
	return *this ;
}

inline
VTime &VTime::operator -=( const VTime &t )
{
	(*this) = (*this) - t ;
	return *this ;
}

inline
VTime &VTime::operator +=( const VTime &t )
{
	(*this) = (*this) + t ;
	return *this ;
}

inline
ostream &operator <<( ostream &os, const VTime &t )
{
	os << t.asString();
	return os;
}



#endif // __VTIME_H
