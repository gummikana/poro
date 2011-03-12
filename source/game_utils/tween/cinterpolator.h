#ifndef INC_CINTERPOLATOR_H
#define INC_CINTERPOLATOR_H

#include <vector>

#include "../../utils/functionptr/cfunctionptr.h"
#include "../../utils/easing/easing.h"

namespace ceng {

//-----------------------------------------------------------------------------

class IInterpolator
{
public:
	virtual ~IInterpolator() { }

	virtual void Update( float t ) = 0;
	virtual void Kill() = 0;
	virtual bool IsDead() const = 0;

	virtual void Reset() = 0;

	virtual void			SetName( const std::string& name ) { mName = name; }
	virtual std::string		GetName() const { return mName; }

	virtual bool UsesPointer( void* pointer ) { return false; } 
protected:
	std::string mName;
};

// ----------------------------------------------------------------------------
// templated interpolator
// requires the cases 
// operator -, operator +, operator *(float, type) 
//
// 
template< typename T >
class CInterpolator : public IInterpolator
{
public:
	CInterpolator( T* who, T start_value, T end_value, ceng::easing::IEasingFunc* math_func = NULL ) :
		myDead( false ),
		myTarget( who ),
		myStartValue( start_value ),
		myEndValue( end_value ),
		myMathFunc( math_func )
	{
	}

	void Update( float t )
	{
		if( myDead || myTarget == NULL ) 
			return;
		
		if( myMathFunc )
			t = myMathFunc->f( t );

		T value = myStartValue + (T)( t * ( myEndValue - myStartValue ) );
		*myTarget = value;
	}

	void Kill() { myDead = true; myTarget = NULL; }
	bool IsDead() const { return myDead; }

	virtual void Reset() 
	{
		myStartValue = *myTarget;
	}

	virtual bool UsesPointer( void* pointer ) 
	{ 
		return ( (void*)(myTarget) == pointer );
	}

	bool myDead;
	T* myTarget;
	T myStartValue;
	T myEndValue;
	ceng::easing::IEasingFunc* myMathFunc;
};

//-----------------------------------------------------------------------------

template< typename T >
class CInterpolatorGetterSetter : public IInterpolator
{
public:
	CInterpolatorGetterSetter( const ceng::CFunctionPtr<>& getter, const ceng::CFunctionPtr<>& setter, T start_value, T end_value, ceng::easing::IEasingFunc* math_func = NULL ) :
		myDead( false ),
		myGetter( getter ),
		mySetter( setter ),
		myStartValue( start_value ),
		myEndValue( end_value ),
		myMathFunc( math_func )
	{
	}

	void Update( float t )
	{
		if( myDead ) 
			return;

		if( myMathFunc )
			t = myMathFunc->f( t );
		
		T value = myStartValue + (T)( t * ( myEndValue - myStartValue ) );
		mySetter( value );
	}

	void Kill() { myDead = true;  }
	bool IsDead() const { return myDead; }

	
	virtual void Reset() 
	{
		myStartValue = ceng::AnyCast< T >( myGetter() );
	}

	virtual bool UsesPointer( void* pointer ) 
	{ 
		if( myGetter == pointer ) return true;
		if( mySetter == pointer ) return true;

		return false;
	}


	bool myDead;
	ceng::CFunctionPtr<> myGetter;
	ceng::CFunctionPtr<> mySetter;
	T myStartValue;
	T myEndValue;
	ceng::easing::IEasingFunc* myMathFunc;

};

//-----------------------------------------------------------------------------

template< typename T >
IInterpolator* CreateInterpolator( 
								  T& who, 
								  const T& to_what, 
								  ceng::easing::IEasingFunc* math_func = NULL )
{
	IInterpolator* result = new CInterpolator< T >( &who, who, to_what, math_func );

	return result;
}


//-----------------------------------------------------------------------------

template< typename T >
IInterpolator* CreateInterpolator( 
								  const ceng::CFunctionPtr<>& getter,
								  const ceng::CFunctionPtr<>& setter,
								  const T& to_what, 
								  ceng::easing::IEasingFunc* math_func = NULL )
{	
	ceng::CFunctionPtr<> value_getter = getter;
	IInterpolator* result = new CInterpolatorGetterSetter< T >( getter, setter, AnyCast< T >( value_getter()  ), to_what, math_func );

	return result;
}

//-----------------------------------------------------------------------------

} // end o namespace ceng

#endif
