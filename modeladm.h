/*******************************************************************
*
*  DESCRIPTION: classModelAdmin and SingleModelAdmin
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian
*	   Alejandro Troccoli (v3)
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*	  mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*  	 27/9/2000 (v3)
*
*******************************************************************/

#ifndef __MODEL_ADMIN_H
#define __MODEL_ADMIN_H

/** include files **/
#include <map.h>            // Template map
#include <string>           // Template string
#include "except.h"

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
class ParallelMainSimulator ;
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
	
	//Functions for accesing the ModelDB.
	typedef map< string, Model *, less < string > > ModelDB;
	
	const ModelDB &models() const;
	virtual Model& model( const string modelName );	
	
	ModelAdmin();
	virtual ~ModelAdmin(){};
	
protected:
	friend class ParallelMainSimulator ;
	friend class StandAloneMainSimulator ;
	friend class Coordinator ;
	friend class ProcessorAdmin ;
	friend class Root ;
	friend class ParallelRoot;
	friend class CoupledCell ;	// para crear los atomicCell
	friend class SingleModelAdm ;
	
	ModelAdmin( const ModelAdmin & );	// Copy constructor
//	virtual ModelAdmin& operator =( const ModelAdmin & ) = 0 ; //Assignment operator
//	virtual int operator ==( const ModelAdmin & ) const = 0; //Equality operator
	
	virtual Atomic &newAtomic( const AtomicType &, const string &modelName ) = 0;
	virtual Atomic &newAtomic( const string &typeNmae, const string &modelName ) = 0;
	
	virtual AtomicCell &newAtomicCell( bool inertial = false, const string &modelName = "AtomicCell" ) = 0;
	
	virtual Coupled &newCoupled( const string &modelName ) = 0;
	virtual CoupledCell &newCoupledCell( const string &modelName ) = 0;
	virtual FlatCoupledCell &newFlatCoupledCell( const string &modelName ) = 0;
	
	typedef map< string, AtomicType, less< string > > AtomicTypes ;
	typedef map< AtomicType, NewFunction*, less< AtomicType > > AtomicKinds ;
	
	AtomicKinds kinds  ;
	AtomicType  typeCount ;
	AtomicTypes types ;
	
	//Functions for accesing the Model Database
	ModelAdmin& add2DB( Model * );
	
private:
	
	ModelDB modelDb;
	
public: 
	const AtomicTypes &newTypes() const ;
};	// class ModelAdmin

class SingleModelAdm
{
public:
	static ModelAdmin &CreateParallel();
	static ModelAdmin &CreateStandAlone();
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
	MASSERTMSG( instance, "No SingleModelAdm (Parallel or StandAlone) has been defined." );
	return *instance;
}

#endif   //__MODEL_ADMIN_H 
