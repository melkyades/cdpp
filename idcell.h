/*******************************************************************
*
*  DESCRIPTION: class InertialDelayCell
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 17/9/1999 (v2)
*
*******************************************************************/

#ifndef __INERTIAL_DELAY_ATOMIC_CELL_H
#define __INERTIAL_DELAY_ATOMIC_CELL_H

/** include files **/
#include "atomcell.h"         // base class header

/** foward declarations **/
#define	INERTIAL_DELAY_CELL_NAME	"InertialDelayCell"

/** declarations **/
class IDCellState : public AtomicCellState
{
public:

	map<string, Real> futureValue;
	map<string, Real> actualValue;

	IDCellState(){};
	virtual ~IDCellState(){};

	IDCellState& operator=(IDCellState& thisState); //Assignment
	void copyState(IDCellState *);
	int  getSize() const;

};

class InertialDelayCell: public AtomicCell
{
public:
	InertialDelayCell( const CellPosition& cellPos, const string &name = INERTIAL_DELAY_CELL_NAME , const LocalTransAdmin::Function &fn = LocalTransAdmin::InvalidFn )
	: AtomicCell( cellPos, name, fn )
	{}

	string className() const
	{return INERTIAL_DELAY_CELL_NAME;}

protected:

	Model &initFunction();
	Model &internalFunction( const InternalMessage & );
	Model &externalFunction( const MessageBag & ) ;
	Model &outputFunction( const CollectMessage & );

	ModelState* allocateState()
	{ return new IDCellState;}

	void initializeCell();
private:

	Real futureValue( const string& ) const;
	void futureValue( const string&, const Real& );

	Real actualValue( const string& ) const;
	void actualValue( const string&, const Real & );

} ;  // InertialDelayCell

/** inline **/
inline
	Real InertialDelayCell::futureValue(const string &port) const
{
	return ((IDCellState*)getCurrentState())->futureValue[port];	
}

inline
	void InertialDelayCell::futureValue (const string &port, const Real& r)
{
	((IDCellState*)getCurrentState())->futureValue[port] = r;
}

inline
	Real InertialDelayCell::actualValue(const string &port) const
{
	return ((IDCellState*)getCurrentState())->actualValue[port];	
}

inline
	void InertialDelayCell::actualValue (const string &port, const Real& r)
{
	((IDCellState*)getCurrentState())->actualValue[port] = r;
}



#endif // __INERTIAL_DELAY_ATOMIC_CELL_H
