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


///////////////////////////////////////////////////////////////////////////////
//
// CCSSObject
// ==========
//
// A way to serialize the signal / slots
//
//
// Created 17.02.2006 by Pete
//
//=============================================================================
//
// 24.08.2006 Pete
//		Fixed AddSlot to use a type specifier.
//
//.............................................................................
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#ifndef INC_CCSSOBJECT_H
#define INC_CCSSOBJECT_H

#include "signalslot_libs.h"

#include "cmyincomingsignals.h"
#include "cslot.h"

#include "ccssobject_info.h"

namespace ceng {

namespace css {

///////////////////////////////////////////////////////////////////////////////



/*template< class T >  bool			CSSObject< T >::is_a_css_object = false;
template< class T >  std::string	CSSObject< T >::name;
*/


///////////////////////////////////////////////////////////////////////////////

class CCSSSlotFactory : public CStaticSingleton< CCSSSlotFactory >
{
public:
	typedef std::pair< std::string, CSlot* > pair;
	~CCSSSlotFactory() { }

	void AddSlot( const std::string& class_name, const std::string& slot_name, CSlot* slot )
	{
		mySlotMap.Insert( class_name, pair( slot_name, slot ) );
	}

	CSlot* GetSlot( const std::string& class_name, const std::string& slot_name )
	{
		CSlot* result = NULL;

		CMapHelperForEach( mySlotMap, class_name, if( mySlotMap.i->first == slot_name ) result = mySlotMap.i->second  );

		return result;
	}

	std::pair< std::string, std::string > GetSlotName( CSlot* slot )
	{

		if( slot && slot->GetGenericEventBase() )
		{
			CMapHelper< std::string, pair >::Iterator i;

			for( i = mySlotMap.Begin(); i != mySlotMap.End(); ++i )
			{
				CMapHelper< std::string, pair >::ListIterator j;
				for( j = i->second.begin(); j != i->second.end(); j++ )
				{
					if( j->second->GetGenericEventBase() )
					{
						if( j->second->GetGenericEventBase()->CheckEvent( slot->GetGenericEventBase() ) )
						{
							return std::pair< std::string, std::string >( i->first, j->first );
						}
					}
				}
			}
		}

		return std::pair< std::string, std::string >();
	}

private:
	CCSSSlotFactory() { }

	CMapHelper< std::string, pair > mySlotMap;

	friend class CStaticSingleton< CCSSSlotFactory >;
};

///////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////

class ICCSSObject : public CAutoList< ICCSSObject >
{
public:
	CMyIncomingSignals cssIncomingSignals;

	ICCSSObject()
	{
	}

	virtual ~ICCSSObject()
	{
	}

	static ICCSSObject* GetCCSSObject( const std::string& id )
	{
		std::list< ICCSSObject* >& list = CAutoList< ICCSSObject >::GetList();
		std::list< ICCSSObject* >::iterator i;

		for( i = list.begin(); i != list.end(); ++i )
		{
			if( (*i)->CssGetId() == id )
				return ( *i );
		}

		return NULL;
	}

	static CSlot* GetSlot( const std::string& id, const std::string& slot_name )
	{
		ICCSSObject* object = ICCSSObject::GetCCSSObject( id );
		if( object )
		{
			return object->CssCreateSlot( slot_name );
		}

		return NULL;
	}

	static std::pair< std::string, std::string > GetObjectName( CSlot* slot )
	{
		std::pair< std::string, std::string > result = css::CCSSSlotFactory::GetSingletonPtr()->GetSlotName( slot );

		if( slot && slot->GetGenericEventBase() && slot->GetGenericEventBase()->GetPointer() && result.first.empty() == false )
			result.first = static_cast< ICCSSObject* >( slot->GetGenericEventBase()->GetPointer() )->CssGetId();

		return result;
	}

	virtual CSlot*	CssCreateSlot( const std::string& name ) = 0;
	virtual void	CssExecute( const std::string& line ) = 0;

	virtual std::string CssGetObjectName() const = 0;
	virtual std::string CssGetId() const = 0;

private:

};



///////////////////////////////////////////////////////////////////////////////

template< class T >
class CCSSObject : public ICCSSObject
{
public:

	virtual ~CCSSObject() { }

	static void AddSlot( const std::string& slot_name, CSlot* slot )
	{
		css::CCSSSlotFactory::GetSingletonPtr()->AddSlot( css::CSSObject< T >::GetSingletonPtr()->name, slot_name, slot );
	}


	virtual CSlot*	CssCreateSlot( const std::string& name )
	{
		CSlot* parent = css::CCSSSlotFactory::GetSingletonPtr()->GetSlot( css::CSSObject< T >::GetSingletonPtr()->name, name );

		if( parent == NULL )
			return NULL;

		CSlot* result = new CSlot( parent );

		result->GetGenericEventBase()->SetPointer( this );
		this->cssIncomingSignals.AddSignal( result );
		result->SetRemoveSlot( &(this->cssIncomingSignals) );


		return result;
	}

	virtual void	CssExecute( const std::string& line )
	{
	}

	virtual std::string CssGetObjectName() const
	{
		return css::CSSObject< T >::GetSingletonPtr()->name;
	}

	virtual std::string CssGetId() const = 0;

};

///////////////////////////////////////////////////////////////////////////////

#include <iostream>

template< class T >
class CCSSObjectHelper
{
public:

	CCSSObjectHelper( const std::string& object_name, const std::string& slot_name, CSlot* slot )
	{
		css::CSSObject< T >::GetSingletonPtr()->name = object_name;
		css::CSSObject< T >::GetSingletonPtr()->is_a_css_object = true;

		CCSSObject< T >::AddSlot( slot_name, slot );

	}

};


#define CSS_REGISTER_SLOT( x, y ) static CCSSObjectHelper< x > css_hidden_helper_object( #x, #y, new CSlot( ( x* )0, & CENG_Join( CENG_Join( x, :: ), y ) ) );

///////////////////////////////////////////////////////////////////////////////
} // end of namespace ceng

#endif
