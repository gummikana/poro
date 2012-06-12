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
// CAnyContainer.h
// ===============
//
// A basic container used with stl containers to make them type
// free containers. You don't have to worry about the template
// coding, you just declare a stl container with CAnyContainer
// type and you can push there variables of any type.
//
// example:
/*
std::vector< CAnyContainer > myVector;
myVector.push_back( 1 );    // integer
myVector.push_back( std::string( "stringvalue" );
myVector.push_back( 1.5 );  // double

myVector[ 0 ] = std::string( "string" );

*/
// To get the values you can use CAnyContainerCast function
// or GetValue method. GetValue() returns the value as a string.
//
//
// Created 30.01.2004 by Pete
//.............................................................................
//
// 11.04.2006 Pete
//		Fixed random bugs in GetType(), GetValue(), GetTypeInfo() and in the
//		operator==(). Also added a unit test to bust the balls of
//		CAnyContainer and unfortunaltely for me I found some nasty bugs that
//		need busting. Also updated the CAnyContainer to use the new
//		cvarreference.h.
//
// 07.02.2005 Pete
//		Fixed the std::string bug ( CAnyContainer could not be implicitly
//		converted from std::string, now it can )
//
// 09.01.2005 Pete
//		Made the CAnyContainerCast much more efficent. Now it has a nice
//		back-up functionality so that things don't have to be so strict with
//		with the template parameters.
//
//=============================================================================

// begin of canycontainer.h
#ifndef INC_CANYCONTAINER_H
#define INC_CANYCONTAINER_H


#include <string>
#include <sstream>
#include <list>
#include <algorithm>
#include <typeinfo>

#include "xml_libraries.h"

namespace ceng {

////////////////////////////////////////////////////////////////

//! This here is a basic set of containers, which stores
//! variables of any type, used by CAnyContainer
class CContainerBase
{
public:
	CContainerBase() { }
    virtual ~CContainerBase() { }

	virtual const std::type_info& GetTypeInfo() const = 0;
};

///////////////////////////////////////////////////////////////////////////////

//! Normal templated container, used by CAnyContainer
template< class _Ty >
class CContainer : public CContainerBase
{
        public:
			CContainer( const _Ty& ref ) : myVar( ref ) { }
        ~CContainer() { }

		const std::type_info& GetTypeInfo() const { return typeid( myVar ); }

        _Ty myVar;
};

///////////////////////////////////////////////////////////////////////////////

//! A container that can suck in any type and output any other.
/*!
	Actualy after writing this one I found out that boost had a similar class
	boost::any
*/
class CAnyContainer
{
public:
    CAnyContainer() : myEmpty( true ), myContainer( NULL ), myReference( NULL ) {  AddBan( this ); }

	template < class _Ty >
    CAnyContainer( const _Ty& reference ) : myContainer( NULL ), myReference( NULL )
    {
           if ( IsBanned( this ) == false )
            {
                    delete myContainer;
                    delete myReference;
            } else
            RemoveBan( this );


            CContainer<_Ty>* tmp_ptr = new CContainer< _Ty >( reference );
			CVarReferenceConstConstructor< _Ty > f( "", tmp_ptr->myVar );
            myReference = f.p;
            myContainer = tmp_ptr;

    }


	// template<>
	CAnyContainer( const std::string& reference ) : myContainer( NULL ), myReference( NULL )
	{
		    if ( IsBanned( this ) == false )
            {
                    delete myContainer;
                    delete myReference;
            }
            else
            {
                RemoveBan( this );
            }


            CContainer< std::string >* tmp_ptr = new CContainer< std::string >( reference );
			CVarReferenceConstConstructor< std::string > f( "", tmp_ptr->myVar );
            myReference = f.p;
            myContainer = tmp_ptr;
	}

	// template<>
	CAnyContainer( const CAnyContainer& other ) : myContainer( NULL ), myReference( NULL )
    {

            if ( IsBanned( this ) == false )
            {
                    delete myContainer;
                    delete myReference;
            } else RemoveBan( this );


            other.BanMe();

            myContainer = other.myContainer;
            myReference = other.myReference;
    }


    ~CAnyContainer()
    {
            if ( IsBanned( this ) == false )
            {
                    delete myContainer;
                    delete myReference;
            } else RemoveBan( this );

    }

    template< class _Ty >
    CAnyContainer& operator= ( const _Ty& reference )
    {
            if ( IsBanned( this ) == false )
            {
                    delete myContainer;
                    delete myReference;
            } else
            RemoveBan( this );


            CContainer<_Ty>* tmp_ptr = new CContainer< _Ty >( reference );
			CVarReferenceConstConstructor< _Ty > f( "", tmp_ptr->myVar );
            myReference = f.p;
            myContainer = tmp_ptr;

			myEmpty = false;

            return *this;
    }

	template< class _Ty >
	bool operator == ( const _Ty& reference )
	{
		if ( myReference && myReference->GetType() != typeid( reference ).name() )
			return false;

		_Ty tmp;
		GetValue( tmp );
		return ( tmp == reference );
	}


	template< class _Ty >
	bool operator < ( const _Ty& reference )
	{
		_Ty tmp;
		GetValue( tmp );
		return reference < tmp;
	}


    std::string GetValue() const
    {
		return myReference ? myReference->GetValue() : "";
    }

	template< class _Ty >
	std::string GetValue( _Ty& var ) const
	{
		using namespace stl;
		stl::stringstream t( GetValue() );
		t.operator>>( var );
		return GetValue();
	}

	template< class _Ty >
    std::string GetValue( const _Ty& var ) const
    {
            stl::stringstream ss( GetValue() );
            ss >> ( const_cast< _Ty& >( var ) );

            return GetValue();
    }

	std::string GetType() const
	{
		return myReference ? myReference->GetType() : "";
	}

	const std::type_info& GetTypeInfo() const
	{
		return myContainer ? myContainer->GetTypeInfo() : typeid( void );
	}

	bool IsEmpty()				{ return myEmpty; }

// private:
	void SetEmpty( bool value ) { myEmpty = value; }

	// Should not be used for nothing else than for the function CAnyContainerCast...
	CContainerBase* _getContainerBase() { return myContainer; }
	const CContainerBase* _getContainerBase() const { return myContainer; }

private:

    void BanMe() const
    {
            AddBan( this );
    }

    void AddBan( const CAnyContainer* banme ) const
    {
            if ( IsBanned( banme ) ) return;
            myDeleteBanList.push_back( banme );
    }

    void RemoveBan( const CAnyContainer* banned )
    {
            if ( IsBanned( banned ) == false ) return;
            myDeleteBanList.remove( banned );

    }

    bool IsBanned( const CAnyContainer* ban ) const
    {
            std::list< const CAnyContainer* >::iterator i;
            i = std::find( myDeleteBanList.begin(), myDeleteBanList.end(), ban );
            return ( i != myDeleteBanList.end() );
    }

	bool			myEmpty;

    CContainerBase* myContainer;
    CVarRefBase*    myReference;

    static std::list< const CAnyContainer* > myDeleteBanList;

	// template < class T > friend T CAnyContainerCasto( const CAnyContainer* container );

};


// std::list< const CAnyContainer* > CAnyContainer::myDeleteBanList;

////////////////////////////////////////////////////////////////


std::ostream& operator << ( std::ostream& os, CAnyContainer& container );


////////////////////////////////////////////////////////////////
/*
template< class _Ty >
_Ty* CAnyContainerCast( CAnyContainer* container )
{
	if ( typeid( _Ty ) == container->GetTypeInfo() )
		return &static_cast< CContainer< _Ty >* >(container->_getContainerBase())->myVar;
	else return NULL;
}
*/

template< class _Ty >
const _Ty* CAnyContainerCasto( const CAnyContainer* container )
{
	// return CAnyContainerCast< _Ty >( const_cast< CAnyContainer* >( container ) );
	CAnyContainer* t = const_cast< CAnyContainer* >( container );

	if ( typeid( _Ty ) == container->GetTypeInfo() )
		return &static_cast< const CContainer< _Ty >* >( container->_getContainerBase() )->myVar;
	else return NULL;

}

template< class _Ty >
_Ty CAnyContainerCast( const CAnyContainer& container )
{
	const _Ty* tmp_var = CAnyContainerCasto< _Ty >( &container );

	if ( tmp_var == NULL )
	{
		_Ty temp_var;
		container.GetValue( temp_var );
		return temp_var;
	}

	return (*tmp_var);
}

////////////////////////////////////////////////////////////////

}

#endif
// end of canycontainer.h
