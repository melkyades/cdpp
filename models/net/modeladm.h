/*******************************************************************
*
*  DESCRIPTION: classModelAdmin and SingleModelAdmin
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __MODEL_ADMIN_H
#define __MODEL_ADMIN_H

/** include files **/
#include <map>            // Template map
#include <string>           // Template string

/** foward declarations **/
class ModelAdmin;
class SingleModelAdm ;
class Atomic ;
class AtomicCell ;
class Coupled ;
class CoupledCell ;
class Model ;
class ProcessorAdmin ;
class Root ;
class MainSimulator ;
class FlatCoupledCell ;

/** definitions **/

struct NewFunction
{
	virtual Atomic *operator()( const string &  ) = 0 ;   
	virtual NewFunction *duplicate() const = 0 ;
};

template <class T> 
struct NewAtomicFunction : NewFunction
{
	Atomic *operator()( const string &name )
	{return new T( name ) ;}
	NewFunction *duplicate() const
	{return new NewAtomicFunction<T>() ;}
} ;


class ModelAdmin
{
public:   
	typedef int AtomicType ;

	AtomicType registerAtomic( const NewFunction &f, const string & );

protected:
	friend class MainSimulator ;
	friend class Coordinator ;
	friend class ProcessorAdmin ;
	friend class Root ;
	friend class CoupledCell ;	// para crear los atomicCell

	ModelAdmin( const ModelAdmin & );	// Copy constructor
	ModelAdmin& operator =( const ModelAdmin & ) ; // Assignment operator
	int operator ==( const ModelAdmin & ) const;	 // Equality operator

	Atomic &newAtomic( const AtomicType &, const string &modelName );
	Atomic &newAtomic( const string &typeNmae, const string &modelName );

	AtomicCell &newAtomicCell( bool inertial = false, const string &modelName = "AtomicCell" ) ;

	Coupled &newCoupled( const string &modelName ) ;
	CoupledCell &newCoupledCell( const string &modelName ) ;
	FlatCoupledCell &newFlatCoupledCell( const string &modelName ) ;


private:
	friend class SingleModelAdm ;

	typedef map< string, AtomicType, less< string > > AtomicTypes ;
	typedef map< AtomicType, NewFunction*, less< AtomicType > > AtomicKinds ;

	AtomicKinds kinds  ;
	AtomicType  typeCount ;
	AtomicTypes types ;

	ModelAdmin() ;	 // Default constructor

public:
	const AtomicTypes &newTypes() const ;

};	// class ModelAdmin

class SingleModelAdm
{
public:
	static ModelAdmin &Instance() ;
private:
	static ModelAdmin *instance ;
	SingleModelAdm() ;
};	// SingleModelAdm

/** inline methods **/

inline
SingleModelAdm::SingleModelAdm()
{}

inline
ModelAdmin &SingleModelAdm::Instance()
{
	if( !instance )
		instance = new ModelAdmin() ;

	return *instance ;
}

#endif   //__MODEL_ADMIN_H 
