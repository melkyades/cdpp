/*******************************************************************
*
*  DESCRIPTION: class VTime/ (originally named Time in v1)
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*	  mailto://atroccol@dc.uba.ar (v2)
*
*  DATE: 27/6/1998
*  DATE: 06/11/2000 (v2)
*
*******************************************************************/

/** include files **/
#include "VTime.hh"         // base class
#include "stringp.h"      // operator + int
#include <cstdlib>

using namespace std;

/** public data **/
const VTime VTime::Zero(0,0,0,0)	 ;	  // Zero constant
const VTime VTime::Inf( 32767, 59, 59, 999 ) ;	// Infinity constant
const VTime VTime::InvalidTime(-1,0,0,0);	//Invalid time. Must be < Zero

/*******************************************************************
* Function Name: VTime
* Description: copy constructor
********************************************************************/
VTime::VTime( const VTime &t )
	: hour( t.hours() )
	, min( t.minutes() )
	, sec( t.seconds() )
	, msec( t.mseconds() )
{}

/*******************************************************************
* Function Name: operator +
********************************************************************/
VTime VTime::operator +( const VTime &t ) const
{
	if ( *this == VTime::Inf || t == VTime::Inf )
		return VTime::Inf;
	
	if ( *this == VTime::InvalidTime || t == VTime::InvalidTime)
		return VTime::InvalidTime;
	
	VTime st( hours() + t.hours(),
		minutes() + t.minutes(),
		seconds() + t.seconds(),
		mseconds() + t.mseconds() );
	
	//st.normalize();	// It's called by constructor and is not needed
	return st;
}

/*******************************************************************
* Function Name: opeartor -
********************************************************************/
VTime VTime::operator -( const VTime &t ) const
{
	if ( *this == VTime::Inf || t == VTime::Inf )
		return VTime::Inf;
	
	if ( *this == VTime::InvalidTime || t == VTime::InvalidTime)
		return VTime::InvalidTime;
	
	VTime st( hours() - t.hours(),
		minutes() - t.minutes(),
		seconds() - t.seconds(),
		mseconds() - t.mseconds() );
	
	//st.normalize();	// It's called by constructor and is not needed
	return st; 
}


/*******************************************************************
* Function Name: opeartor *
********************************************************************/
VTime VTime::operator * (const int &mult) const
{
	if (*this == VTime::Inf)
		return VTime::Inf;
	
	if ( *this == VTime::InvalidTime )
		return VTime::InvalidTime;	
	
	VTime st( hours() * mult, 
		minutes() * mult,
		seconds() * mult,
		mseconds() * mult);
	
	return st;
	
}

/*******************************************************************
* Function Name: opeartor /
********************************************************************/
VTime VTime::operator / (const int &div) const
{
	
	Hours h;
	Minutes m;
	Seconds s;
	MSeconds ms;
	
	if (*this == VTime::Inf)
		return VTime::Inf;
	
	if (div == 0)
		return VTime::Inf;
	
	
	h = hours() / div;
	m = (minutes() + hours()%div * 60 )/ div;
	s = (seconds() + ( minutes() + hours() % div * 60 ) % div * 60 ) / div;
	ms = ( mseconds() + (seconds() + ( minutes() + hours() % div * 60 ) % div ) % div * 1000 ) / div;
	
	VTime st( h,m,s, ms);
	
	return st;
	
}
/*******************************************************************
* Function Name: opeartor ==
********************************************************************/
bool VTime::operator ==( const VTime &t ) const
{
	return ( hours()    == t.hours()    &&
		minutes()  == t.minutes()  &&
		seconds()  == t.seconds()  &&
		mseconds() == t.mseconds() );
}

/*******************************************************************
* Function Name: opeartor =
********************************************************************/
VTime &VTime::operator =( const VTime &t )
{
	hour = t.hours();
	sec = t.seconds();
	min = t.minutes();
	msec = t.mseconds();
	return *this;
}

/*******************************************************************
* Function Name: operator <<
********************************************************************/
bool VTime::operator <( const VTime &t ) const
{
	return( hours() < t.hours()    ||
		( hours() == t.hours() &&
			( minutes() < t.minutes()  ||
				( minutes() == t.minutes()  &&
					( seconds() < t.seconds() ||
						( seconds() == t.seconds() &&
							mseconds() < t.mseconds() ) ) ) ) ) ) ;
}


bool VTime::operator <=( const VTime &t ) const
{
	return( hours() < t.hours()    ||
		( hours() == t.hours() &&
			( minutes() < t.minutes()  ||
				( minutes() == t.minutes()  &&
					( seconds() < t.seconds() ||
						( seconds() == t.seconds() &&
							mseconds() <= t.mseconds() ) ) ) ) ) ) ;
}

bool VTime::operator >=( const VTime &t ) const
{
	return t < *this or *this == t;
}

/*******************************************************************
* Function Name: asString
********************************************************************/
string VTime::asString() const
{
	char buf[100];
	
	if (*this == VTime::Inf)
		sprintf( buf, "..." );
	else
		sprintf( buf, "%02d:%02d:%02d:%03d", hours(), minutes(), seconds(), mseconds() );
	
	return string( buf );
}

/*******************************************************************
* Function Name: makeFrom
* Description: make from string
********************************************************************/
VTime &VTime::makeFrom( const string &str )
{
	sscanf( str.c_str(), "%d:%d:%d:%d", &hour, &min, &sec, &msec );
	normalize();
	return *this;
}

/*******************************************************************
* Function Name: makeFrom
* Description: make from miliseconds
********************************************************************/
VTime &VTime::makeFrom( float milsec )
{
	seconds( static_cast< int >( milsec ) ) ;
	milsec -= seconds() ;
	mseconds( static_cast< int >( milsec * 1000 ) ) ;
	normalize() ;
	return *this ;
}

/*******************************************************************
* Function Name: normalize
********************************************************************/
VTime &VTime::normalize()
{
	// 0..999 Miliseconds
	adjust( msec, sec , 1000 );
	
	// 0..59 seconds
	adjust( sec , min , 60   );
	
	// 0..59 minutes
	adjust( min , hour, 60   );
	
	return *this ;
}

/*******************************************************************
* Function Name: adjust
********************************************************************/
VTime &VTime::adjust( int &left, int &right, int maxVal )
{
	if( left >= maxVal )
	{
		right += left / maxVal ;
		left %= maxVal ;
	} else if( left < 0 )
	{
		register int aux = abs( left / maxVal ) + ( left % maxVal == 0 ? 0 : 1 );
		right -= aux ;
		left += maxVal * aux ;
	}
	return *this ;
}

/*******************************************************************
* Function Name: operator >>
********************************************************************/
istream &operator >>( istream &is, VTime &t )
{
	string s;
	is >> s;
	t.makeFrom(s);
}
