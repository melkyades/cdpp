/*******************************************************************
*
*  DESCRIPTION: class Real
*
*  AUTHOR: Daniel A. Rodriguez 
*
*  EMAIL: mailto://drodrigu@dc.uba.ar
*
*  DATE: 2/3/1999 (v2)
*
*******************************************************************/

/** include files **/
#include "real.h"
#include "realprec.h"
#include "mathincl.h"

#define MAX_WIDTH	50

using namespace std;

Real Real::tundef(UNDEFINED);	// Constante
Real Real::one(1);		// Constante
Real Real::zero(0);		// Constante

string Real::asString(int width, int precision) const
{
	char	ff[MAX_WIDTH];

	if (IsUndefined())
		sprintf(ff, "%*s?", width-1, "");
	else
		sprintf(ff, "%*.*f", width, precision, rvalue );

	return string(ff);
}

TValBool Real::Equal ( const Real & cv ) const
{
	// if both are undefined then are equals.
	if ( IsUndefined() && cv.IsUndefined() )
		return TValBool::ttrue;

	// if only one is undefined:
	else if ( IsUndefined() || cv.IsUndefined() )
		return TValBool::tundef;

	// else they are real numbers.
	return (rvalue >= cv.value() - RealPrecision::Tol.precision()  &&
		rvalue <= cv.value() + RealPrecision::Tol.precision() ) ?
		TValBool::ttrue : TValBool::tfalse;
}

TValBool Real::NotEqual ( const Real & cv ) const
{
	return *this != cv;
}

Real::operator TValBool () const
{
	if (_isUndefined(rvalue))
		return TValBool::tundef;
	else if (rvalue == 1)
		return TValBool::ttrue;

	MASSERTMSG( rvalue == 0, string("Ilegal casting from a Real to a TValBool") );
	return TValBool::tfalse;
}

TValBool Real::Greater ( const Real & cv ) const
{
	// if both are undefined:
	if ( IsUndefined() && cv.IsUndefined() )
		return TValBool::tfalse;

	// if only one is undefined:
	else if ( IsUndefined() || cv.IsUndefined() )
		return TValBool::tundef;

	// else they are real numbers.
	return rvalue > cv.value();
}

TValBool Real::Less ( const Real & cv ) const
{
	// if both are undefined:
	if ( IsUndefined() && cv.IsUndefined() )
		return TValBool::tfalse;

	// if only one is undefined:
	else if ( IsUndefined() || cv.IsUndefined() )
		return TValBool::tundef;

	// else they are real numbers.
	return rvalue < cv.value();
}

TValBool Real::LessEqual ( const Real & cv ) const
{
	return (*this == cv) || (*this < cv);
}

TValBool Real::GreaterEqual ( const Real & cv ) const
{
	return (*this == cv) || (*this > cv);
}

TValBool Real::operator != ( const Real &c ) const
{
	return !(*this == c);
}

TValBool Real::operator == ( const Real &c ) const
{
	TValBool	r = Equal(c);
	return (r == TValBool::tundef) ? TValBool::tfalse : r;
}

TValBool Real::operator > ( const Real &c ) const
{
	return Greater(c);
}

TValBool Real::operator < ( const Real &c ) const
{
	return Less(c);
}

TValBool Real::operator >= ( const Real &c ) const
{
	return GreaterEqual(c);
}

TValBool Real::operator <= ( const Real &c ) const
{
	return LessEqual(c);
}

const Real Real::operator / (const Real &c) const
{
	if (IsUndefined() || c.IsUndefined())
		return Real();	// Devuelve un indefinido
	if (c == zero)
		return Real();

	// else:
	return Real( value() / c.value() );
}

const Real Real::operator * (const Real &c) const
{
	if (IsUndefined() || c.IsUndefined())
		return Real();	// Devuelve un indefinido
	// else:
	return Real( value() * c.value() );
}

const Real Real::operator + (const Real &c) const
{
	if (IsUndefined() || c.IsUndefined())
		return Real();	// Devuelve un indefinido
	// else:
	return Real( value() + c.value() );
}

const Real Real::operator - (const Real &c) const
{
	if (IsUndefined() || c.IsUndefined())
		return Real();	// Devuelve un indefinido
	// else:
	return Real( value() - c.value() );
}

Real::Real( Value Valor )
{
	switch (isInfinite(Valor)) {
		case 1:		rvalue = M_INFINITO;
				break;

		case -1:	rvalue = M_MINFINITO;
				break;

		default:	rvalue = Valor;
	}
}
