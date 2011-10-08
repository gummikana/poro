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


/////////////////////////////////////////////////////////////////////////////////
//
// cvarreference.h
// ===============
//
// Basicly this is set of classes derived from CVarRefBase class
// used to set values to "any" class. If you want to add your
// own class to the supported classes write a new function
// to the stl::stringstream class.
//
//
// Basicly this is set of classes that give you the functionality to cast from
// and to string variable that is given to as a reference. So you can 
// manipulate any variable through a interface that takes strings, this unifies
// things a lot, when your working on a scripting language. You can define some
// random variables as variables in scripting language and manipulate them 
// straigth through the string values.
//
// Also handy when you have to work on some random text parser. And in various
// other places. For example the CAnyContainer.
//
//
// There is a slightly modified version of stl stringstream used here. You can
// define your own types to work with it by adding a specialization for your 
// type. Also note the handy macro, use it to define the values so that we know
// your classes are using this shit.
//
// Mainly CAnyContainer likes it very mucho.
//
//
// Created 11.01.2004 by Pete
//.............................................................................
//
// 11.04.2006 Pete
//		I changed a lot of stuff. Changed the stl::stringstream's namespace from
//		std (yes I know that was horrible) to ceng. Also added the template 
//		specification helper to define which types can be used with the 
//		stl::stringstream and which cannot be used.
//		Also I added the CVarRefDummy class that uses no stream operators what 
//		so ever so it can be used in cases where no stringstreamabilty is 
//		defined.
//		Also added the CVarRefConstruction helpers to help with the 
//		construction of various different types of VarRefs so that no compiler
//		errors are given even if no stream operators are defined.
//
// 23.03.2006 Pete
//		fixed the stl::stringstream so that it compiles now with borland
//		c++ builder.
//
//=============================================================================
#ifndef INC_CVARREFERENCE_H
#define INC_CVARREFERENCE_H


#include <typeinfo>
#include <string>
#include <sstream>
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
#define CENG_IS_CENGSTRINGSTREAMABLE_MACROHELPER( x ) template<> struct stringstream::is_cengstringstreamable< x > { enum { value = 1 }; };

namespace ceng {
	
namespace stl {
using namespace std;

//! This class here is used to setting values to the types...
//
class stringstream : public std::istringstream
{
public:
	stringstream( openmode mode = in  ) : 
		istringstream( mode ), 
		myStr("") 
	{ }
	
	stringstream( const string& str, openmode mode = in ) :
		istringstream( str, mode ), 
		myStr( str ) 
	{ }


	//=========================================================================

	template< class T >
	struct is_cengstringstreamable
	{
		enum { value = 0 };
	};

    //=========================================================================

    //istream& operator>> ( int& val ) { return istringstream::operator>>(val); }
    
    
    //Mac error: binding of reference to type 'istream' (aka 'basic_istream<char>') to a value of type 'const ceng::stl::stringstream' drops qualifiers
#ifndef __APPLE__    
    template< typename T >
    istream& operator>> ( const T& val ) const  { return *this; }
#endif
    template< typename T >
    istream& operator>> ( T& val ) { return *this; }


    istream& operator>> (bool& val )				{ return istringstream::operator>>(val); }
    istream& operator>> (short& val )				{ return istringstream::operator>>(val); }
    istream& operator>> (unsigned short& val )		{ return istringstream::operator>>(val); }
    istream& operator>> (int& val )					{ return istringstream::operator>>(val); }
    istream& operator>> (unsigned int& val )		{ return istringstream::operator>>(val); }
    istream& operator>> (long& val )				{ return istringstream::operator>>(val); }
    istream& operator>> (unsigned long& val )		{ return istringstream::operator>>(val); }
    istream& operator>> (float& val )				{ return istringstream::operator>>(val); }
    istream& operator>> (double& val )				{ return istringstream::operator>>(val); }
    istream& operator>> (long double& val )			{ return istringstream::operator>>(val); }
    istream& operator>> (void*& val )				{ return istringstream::operator>>(val); }

    istream& operator>> (string& sb ) { sb = myStr; return (*this); }

private:
	const string myStr;
};

CENG_IS_CENGSTRINGSTREAMABLE_MACROHELPER( bool )
CENG_IS_CENGSTRINGSTREAMABLE_MACROHELPER( short )
CENG_IS_CENGSTRINGSTREAMABLE_MACROHELPER( unsigned short )
CENG_IS_CENGSTRINGSTREAMABLE_MACROHELPER( int )
CENG_IS_CENGSTRINGSTREAMABLE_MACROHELPER( unsigned int )
CENG_IS_CENGSTRINGSTREAMABLE_MACROHELPER( float )
CENG_IS_CENGSTRINGSTREAMABLE_MACROHELPER( double )
CENG_IS_CENGSTRINGSTREAMABLE_MACROHELPER( long double )
CENG_IS_CENGSTRINGSTREAMABLE_MACROHELPER( void* )
CENG_IS_CENGSTRINGSTREAMABLE_MACROHELPER( std::string )


}

using std::istringstream;
// typedef stl::stringstream stl::stringstream;

} // end of namespace ceng
///////////////////////////////////////////////////////////////////////////////

namespace ceng {


}

///////////////////////////////////////////////////////////////////////////////

namespace ceng {

//! The CVarRefBase is a abstract class, that defines the interface
/*! The idea of CVarRef classes is that you can create a
	reference to any variable. Be it const or nor. And you can
	change that variables value by a string value. Very handy
	in lots of places. Like the CAnyContainer
*/
class CVarRefBase
{
        public:
        CVarRefBase() { }
        virtual ~CVarRefBase() { }

        virtual void        SetValue( const std::string& value ) = 0;
        virtual std::string GetValue() const = 0;

        virtual std::string GetName() const = 0;
        virtual std::string GetType() const = 0;

        virtual bool        GetConst() const = 0;
};


///////////////////////////////////////////////////////////////////////////////
//! This is a the basic reference, that actually does something
template< typename Type >
class CVarRef : public CVarRefBase
{
public:


	CVarRef( const std::string& varname, Type& var ) :
		myVariable( var ),
		myVariableName( varname ),
		myVariableType( typeid( var ).name() ),
		myConst( false )
	{ }

	//=========================================================================
	
	void SetValue( const std::string& value )
	{
		using namespace stl;
		if ( !myConst )
			stl::stringstream( value ) >> myVariable;
	}

	std::string GetValue() const
	{
		std::stringstream ss;
		ss << myVariable;
		return ss.str();
	}

	//=========================================================================

	std::string GetName() const
	{
		return myVariableName;
	}

	std::string GetType() const
	{
		return myVariableType;
	}

	bool GetConst() const
	{
		return myConst;
	}

	//=========================================================================

private:
	Type&        myVariable;

        std::string myVariableName;
        std::string myVariableType;

        bool        myConst;
};

///////////////////////////////////////////////////////////////////////////////
//! This a handy helper class, when sometimes you need to create
//! a variable and a reference to it. This actually fakes the
//! reference and stores the variable in it self.
template< typename Type = std::string >
class CVarRefEmpty : public CVarRefBase
{
public:
	CVarRefEmpty( const std::string& varname, bool isconst = false ) :
		myVariable(),
		myVariableName( varname ),
		myVariableType( typeid( Type ).name() ),
		myConst( isconst )
	{ }

	//=========================================================================
	
	void SetValue( const std::string& value )
	{
		using namespace stl;

		if ( !myConst )
			stl::stringstream( value ) >> myVariable;
	}

	std::string GetValue() const
	{
		std::stringstream ss;
		ss << myVariable;
		return ss.str();
	}

	//=========================================================================

	std::string GetName() const
	{
		return myVariableName;
	}

	std::string GetType() const
	{
		return myVariableType;
	}

	bool GetConst() const
	{
		return myConst;
	}

	//=========================================================================


private:
	Type         myVariable;

	std::string myVariableName;
	std::string myVariableType;

	bool        myConst;

};

///////////////////////////////////////////////////////////////////////////////
//! This another helper class used when you encounter a const
//! class
template< typename Type >
class CVarRefConst : public CVarRefBase
{
public:

	CVarRefConst( const std::string& varname, const Type& var )  :
		myVariable( var ),
		myVariableName( varname ),
		myVariableType( typeid( var ).name() ),
		myConst( true )
	{ }

	//=========================================================================

	void SetValue( const std::string& value )
	{
    
	}

	std::string GetValue() const
	{
		std::stringstream ss;
		ss << myVariable;
		return ss.str();
	}

	//=========================================================================

	std::string GetName() const
	{
		return myVariableName;
	}

	std::string GetType() const
	{
		return myVariableType;
	}

	bool GetConst() const
	{
		return myConst;
	}

	//=========================================================================

private:
	const Type&  myVariable;

	std::string myVariableName;
	std::string myVariableType;

	bool        myConst;
};

///////////////////////////////////////////////////////////////////////////////

//! Sometimes a dummy class is needed, when the ability the cast the class to
//! string has not been defined. You cannot get anything out of this reference
//! and you cannot change the value of the reference. In some cases this kind 
//! of compliteness is needed. Till example the canycontainer needs this.
template< typename Type >
class CVarRefDummy : public CVarRefBase
{
public:

	CVarRefDummy( const std::string& varname, const Type& var )  :
		myVariable( var ),
		myVariableName( varname ),
		myVariableType( typeid( var ).name() ),
		myConst( true )
	{ }

	//=========================================================================

	void SetValue( const std::string& value )
	{
		assert( false );
	}

	std::string GetValue() const
	{
		return "";
	}

	//=========================================================================

	std::string GetName() const
	{
		return myVariableName;
	}

	std::string GetType() const
	{
		return myVariableType;
	}

	bool GetConst() const
	{
		return myConst;
	}

	//=========================================================================

private:
	const Type&  myVariable;

	std::string myVariableName;
	std::string myVariableType;

	bool        myConst;
};

///////////////////////////////////////////////////////////////////////////////
} // end of namespace ceng

namespace ceng {
///////////////////////////////////////////////////////////////////////////////

template< int I >
struct CVarReferenceConstructorHelper
{
	CVarReferenceConstructorHelper() { }
	template< class T >
	CVarRefBase* CreateNew( const std::string& s, T& t ) { return 0; }	
};

template<>
struct CVarReferenceConstructorHelper< 0 >
{
	CVarReferenceConstructorHelper() { }

	template< class T >
	CVarRefBase* CreateNew( const std::string& s, T& t ) { return new CVarRefDummy< T >( s, t ); }
};

template<>
struct CVarReferenceConstructorHelper< 1 >
{
	CVarReferenceConstructorHelper() {  }

	template< class T >
	CVarRefBase* CreateNew( const std::string& s, T& t ) { return new CVarRef< T >( s, t ); }
};

//=============================================================================

template< int I >
struct CVarReferenceConstConstructorHelper
{
	CVarReferenceConstConstructorHelper() { }
	template< class T >
	CVarRefBase* CreateNew( const std::string& s, const T& t ) { return 0; }	
};

template<>
struct CVarReferenceConstConstructorHelper< 0 >
{
	CVarReferenceConstConstructorHelper() { }

	template< class T >
	CVarRefBase* CreateNew( const std::string& s, const T& t ) { return new CVarRefDummy< T >( s, t ); }
};

template<>
struct CVarReferenceConstConstructorHelper< 1 >
{
	CVarReferenceConstConstructorHelper() {  }

	template< class T >
	CVarRefBase* CreateNew( const std::string& s, const T& t ) { return new CVarRefConst< T >( s, t ); }
};

//=============================================================================

template< class T >
struct CVarReferenceConstructor
{
	CVarReferenceConstructor( const std::string& s, T& t ) : p( 0 ) 
	{ 
		CVarReferenceConstructorHelper< stl::stringstream::is_cengstringstreamable< T >::value > factory;
		p = factory.CreateNew( s, t );
	}

	CVarRefBase* p;
};

template< class T >
struct CVarReferenceConstConstructor
{
	CVarReferenceConstConstructor( const std::string& s, const T& t ) : p( 0 ) 
	{ 
		CVarReferenceConstConstructorHelper< stl::stringstream::is_cengstringstreamable< T >::value > factory;
		p = factory.CreateNew( s, t );
	}

	CVarRefBase* p;
};

///////////////////////////////////////////////////////////////////////////////
} // end of namespace ceng

#endif


