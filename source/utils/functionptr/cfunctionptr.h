///////////////////////////////////////////////////////////////////////////////
//
//	Event class is to be used when a event is fired.
//	
//	Difference between delegates and events are that events doen't return
//  return value from the function delegates do. And because of that events
//  can be used without defining the return value in advance.
//
//.............................................................................
//
// 07.01.2005 Pete
//		Created the Delegate and Event classes. 
//		
//
//=============================================================================
#ifndef INC_FUNCTION_PTR_H
#define INC_FUNCTION_PTR_H

#include <algorithm>
#include "cfunctionptr_impl.h"

///////////////////////////////////////////////////////////////////////////////

namespace ceng {

	
template< class _Ty >
_Ty AnyCast( const CAnyContainer& container )
{
	return ceng::CAnyContainerCast< _Ty >( container );
}

template<>
CAnyContainer AnyCast( const CAnyContainer& container );

template<>
inline CAnyContainer AnyCast( const CAnyContainer& container ) { return container; }


///////////////////////////////////////////////////////////////////////////////

//! The FunctionPtr class. This reprisents a single function / method
template< class ReturnValue = CAnyContainer >
class CFunctionPtr
{
public:

	CFunctionPtr() : myFunc( NULL ) { }
	
	CFunctionPtr( const CFunctionPtr& other )
	{
		if ( other.myFunc ) 
			myFunc = other.myFunc->Clone();
		else myFunc = NULL;
	}

	template< class Class, class Return >
	CFunctionPtr( Class* object, Return (Class::*func)() )
	{
		myFunc = impl::CreateFunctionPointer( object, func );
	}

	template< class Class, class Return, class Arg1 >
	CFunctionPtr( Class* object, Return (Class::*func)(Arg1) )
	{
		myFunc = impl::CreateFunctionPointer( object, func );
	}

#if 0
	template< class Class, class Return, class Arg1, class Arg2 >
	CFunctionPtr( Class* object, Return (Class::*func)(Arg1, Arg2) )
	{
		myFunc = new CGeneriCFunctionPtr2< Class, Return, Arg1, Arg2 >( object, func );
	}

	template< class Return > 
	CFunctionPtr( Return (*func)() )
	{
		myFunc = new CSGeneriCFunctionPtr0< Return >( func );
	}

	template< class Return, class Arg1 >
	CFunctionPtr( Return (*func)( Arg1 ) )
	{
		myFunc = new CSGeneriCFunctionPtr1< Return, Arg1 >( func );
	}

	template< class Return, class Arg1, class Arg2 >
	CFunctionPtr( Return (*func)( Arg1, Arg2 ) )
	{
		myFunc = new CSGeneriCFunctionPtr2< Return, Arg1, Arg2 >( func );
	}
#endif
	~CFunctionPtr() { delete myFunc; myFunc = NULL; }

	
	CFunctionPtr& operator=( const CFunctionPtr& other )
	{
		CFunctionPtr( other ).Swap( *this );
		return *this;
	}


	template< class T >
	bool operator==( const T* pointer ) 
	{
		return myFunc->IsA( pointer );
	}

	bool operator==( const CFunctionPtr& other ) const
	{
		if( myFunc == other.myFunc ) return true;
		return myFunc->Compare( other.myFunc );
	}

	bool operator!=( const CFunctionPtr& other ) const { return !operator==(other);	}

	ReturnValue operator()() { return AnyCast< ReturnValue >( myFunc->Call() ); }
	
	template< class Arg1 >
	CAnyContainer operator()( const Arg1& a1 ) { return myFunc->Call( a1 ); }

	template< class Arg1, class Arg2 >
	CAnyContainer operator()( const Arg1& a1, const Arg2& a2 ) { return myFunc->Call( a1, a2 ); }
	

	//-------------------------------------------------------------------------
	
	bool Empty() const { return (myFunc == NULL); }

	//-------------------------------------------------------------------------
	
	int GetNumberOfParameters() const
	{
		cassert( myFunc );
		return myFunc->GetNumberOfParameters();
	}
	
	const std::type_info& GetTypeInfo() const
	{
		cassert( myFunc );
		return myFunc->GetTypeInfo();
	}

	//-------------------------------------------------------------------------
private:

	CFunctionPtr& Swap( CFunctionPtr& other)
    {
		std::swap( myFunc, other.myFunc );
        return *this;
    }

	impl::IGenericFunctionPtr* myFunc;
};


} // end of namespace ceng

#endif
