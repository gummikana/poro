/***************************************************************************
 *
 * Copyright (c) 2003 - 2011 Petri Purho
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ***************************************************************************/


#ifndef INC_CINTERPOLATOR_H
#define INC_CINTERPOLATOR_H

#include <vector>

#include "../../utils/functionptr/cfunctionptr.h"
#include "../../utils/easing/easing.h"
#include "../../utils/math/cangle.h"

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
		
		T value = myStartValue + (T)( ( myEndValue - myStartValue ) * t );
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
class CInterpolatorGetterSetterWithAngle : public IInterpolator
{
public:
	CInterpolatorGetterSetterWithAngle( const ceng::CFunctionPtr<>& getter, const ceng::CFunctionPtr<>& setter, T start_value, T end_value, ceng::easing::IEasingFunc* math_func = NULL ) :
		myDead( false ),
		myGetter( getter ),
		mySetter( setter ),
		myTarget( NULL ),
		myStartValue( start_value ),
		myEndValue( end_value ),
		myMathFunc( math_func )
	{
		SetAngleValues( myStartValue, myEndValue );
	}

	CInterpolatorGetterSetterWithAngle( T* target, T start_value, T end_value, ceng::easing::IEasingFunc* math_func = NULL ) :
		myDead( false ),
		myGetter(),
		mySetter(),
		myTarget( target ),
		myStartValue( start_value ),
		myEndValue( end_value ),
		myMathFunc( math_func )
	{
		SetAngleValues( myStartValue, myEndValue );
	}

	void Update( float t )
	{
		if( myDead ) 
			return;

		if( myMathFunc )
			t = myMathFunc->f( t );
		
		ceng::math::CAngle< T > value( myStartValue + (T)( t * ( myEndValue - myStartValue ) ) );

		if( myTarget )
			*myTarget = value.GetValue();
		else
			mySetter( value.GetValue() );
	}

	void Kill() { myDead = true;  }
	bool IsDead() const { return myDead; }

	
	virtual void Reset() 
	{
		if( myTarget ) 
			myStartValue = *myTarget;
		else
			myStartValue = ( ceng::AnyCast< T >( myGetter() ) );
		SetAngleValues( myStartValue, myEndValue );
	}

	void SetAngleValues( float start_value, float end_value ) 
	{
		myStartValue = ceng::math::CAngle< T >( start_value ).GetValue();
		myEndValue = ceng::math::CAngle< T >( end_value ).GetValue();
		if( ceng::math::Absolute( myEndValue - myStartValue ) > ceng::math::pi )
		{
			if( myEndValue > myStartValue )
				myEndValue -= 2.f * ceng::math::pi;
			else
				myStartValue -= 2.f * ceng::math::pi;
		}
	}

	virtual bool UsesPointer( void* pointer ) 
	{ 
		if( myGetter == pointer ) return true;
		if( mySetter == pointer ) return true;
		if( myTarget == pointer ) return true;

		return false;
	}


	bool myDead;
	ceng::CFunctionPtr<> myGetter;
	ceng::CFunctionPtr<> mySetter;
	T* myTarget;
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

template< typename T >
IInterpolator* CreateInterpolatorForAngles( 
								  const ceng::CFunctionPtr<>& getter,
								  const ceng::CFunctionPtr<>& setter,
								  const T& to_what, 
								  ceng::easing::IEasingFunc* math_func = NULL )
{	
	ceng::CFunctionPtr<> value_getter = getter;
	IInterpolator* result = new CInterpolatorGetterSetterWithAngle< T >( getter, setter, AnyCast< T >( value_getter()  ), to_what, math_func );

	return result;
}


template< typename T >
IInterpolator* CreateInterpolatorForAngles( 
								  T& rotation,
								  const T& to_what, 
								  ceng::easing::IEasingFunc* math_func = NULL )
{	
	ceng::CFunctionPtr<> value_getter = getter;
	IInterpolator* result = new CInterpolatorGetterSetterWithAngle< T >( &rotation, rotation, to_what, math_func );

	return result;
}
//-----------------------------------------------------------------------------

} // end o namespace ceng

#endif
