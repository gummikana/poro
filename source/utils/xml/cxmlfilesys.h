/***************************************************************************
 *
 * Copyright (c) 2003 - 2013 Petri Purho
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
// CXmlFileSys
//
// A warpper class for the CXmlNode and for the functions in the CXmlCast.
// Used also in the Serialize method all over the place, so its a so called
// Bridge between the CXmlNode datastructure and various amounths of classes
//
//.............................................................................
//
// 23.02.2013 Pete
//		Added XmlVectorSerializer implemention.
//
// 15.02.2013 Pete
//		Added XML_BindPtrAlias implementation in here, might have to move it
//		off it is starts causing problems or isn't general enough
//
// 14.03.2006 Pete
//		Fixed a direct filesys calling from the XML_BindAttributeAlias() to
//		use the parameter x.
//
// 13.03.2005 Pete
//		Added XML_BindAttributeAlias() and XML_BindAttribute() macros the help
//		with the tedious task of binding the attributes
//
// 04.03.2005 Pete
//		Added XML_BindAlias() macro the help us keep our code more cleaner
//
// 24.02.2005 Pete
//	  * Changed the XML_ConvertTo() macro so that it checks now if a attribute
//		named exists before it tryies to convert.
//	  * Fixed a creepy bug in the HasChildNamed() method
//
// 20.02.2005 Pete
//		Added HasChildNamed() method to help us create more flexible
//		serialization.
//
// 14.02.2005 Pete
//		Fixed XML_Bind macro because it had a nasty bug.
//		if ( writing ) { } else if ( writing ) { } should have been
//		if ( writing ) { } else if ( reading ) { }
//
// 05.10.2004 Pete
//		Added the XML_Bind macro after JP Johansson's suggestion on the
//		gamedev forum
//
//=============================================================================
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#ifndef INC_CXMLFILESYS_H
#define INC_CXMLFILESYS_H

#include "xml_macros.h"
#include "xml_libraries.h"
#include "cxmlnode.h"

// #define logger std::cout

//! A wrapper for the CXmlNode datastructure and cast functions.
/*!
	Note:	You have to destroy the RootElement by hand after use, because
			we have absolute no idea if your done with your xml structure
			or not

	A warpper class for the CXmlNode and for the functions in the CXmlCast.
	Used also in the Serialize method all over the place, so its a so called
    Bridge between the CXmlNode datastructure and various amounths of classes
*/
namespace ceng {

class CXmlNode;
    
template< class T >
CXmlNode* TEMP_XmlConvertFrom( T& from, const std::string& name );

                              
class CXmlFileSys
{
public:

	//! Represents the current status of the filesystem
	enum FileSysStatus
	{
		empty = -1,
		reading = 0,
		writing = 1
	};

	//! Basic constructors
	CXmlFileSys() :
        myFilename(),
        myLine( 0 ),
		myUseStrict( false ),
		myStatus( empty ),
		myRootElement( NULL ),
		myCurrentNode( NULL ),
		myChildPosition( 0 )
    { }

	//! Basic constructor with node and status
	CXmlFileSys( CXmlNode* node, FileSysStatus status ) :
        myFilename(),
        myLine( 0 ),
		myUseStrict( false ),
        myStatus( status ),
		myRootElement( node ),
		myCurrentNode( node ),
		myChildPosition( 0 )
	{ }

	//! Deconstructor which doesn't do a crap.
	~CXmlFileSys() { }


	//! Returns the current status
	FileSysStatus	GetStatus() const { return myStatus; }

	//! Returns true if we are reading from a file
	bool			IsReading() const { return ( myStatus==reading ); }

	//! Returns true if we are writing a mesh
	bool			IsWriting() const { return ( myStatus==writing ); }

	//! Returns the current node
	CXmlNode*		GetNode()			{ return myCurrentNode; }

	//! Same thing but const, if there is need for that
	const CXmlNode*	GetNode() const		{ return myCurrentNode; }

	//! This here is used to convert bonafied xml to stl container
	template< class T >
	void ConvertToContainer( T& to, const std::string& name )
	{ XmlSerializeToContainer( this, to, name ); }

	//! This is here is used to convert xml to stl container
	template< class T >
	void ConvertFromContainer( T& from, const std::string& name )
	{ XmlSerializeFromContainer( this, from, name );	}



	//! For use of while reading. Converts the current node to mesh
	// Reading
	template< class T >
	void ConvertTo( T& to, const std::string& name )
	{ XmlConvertTo( FindChildByName( name ), to ); }

	//! For use of while reading. Creates new pointer of the type
	// Reading
	template< class T >
	void CreateNew( T*& pointer, const std::string& name )
	{ XmlCreateNew( FindChildByName( name ), pointer ); }

	//! To be used when writing things. Creates a new child node from the type
	// Writing
	template< class T >
	void ConvertFrom( T& from, const std::string& name )
	{
        myCurrentNode->AddChild( TEMP_XmlConvertFrom( from, name ) );
    }
    
	//! Basicly to be used while writing, adds a new child to the current element
	/*!
		Note: Remember to call the EndElement( name ) after this.
	*/
	void AddElement( const std::string& name )
	{
		CXmlNode* tmp = CXmlNode::CreateNewNode();
		tmp->SetName( name );
		myCurrentNode->AddChild( tmp );
		myCurrentNode = tmp;

	}

	//! Adds a attribute to the current node in need
	void AddAttribute( const std::string& attribute_name, const CAnyContainer& value )
	{
		myCurrentNode->AddAttribute( attribute_name, value );
	}

	//! Ends the element, call this after calling the AddElement
	void EndElement( const std::string& name )
	{
		if ( myCurrentNode->GetName() != name )
			if ( myUseStrict ) logger << "Xml warning, trying to end a different node: " << name << " should end: " << myCurrentNode->GetName() << " in file: " << myFilename << " at line " << myLine << std::endl;

		myCurrentNode = myCurrentNode->GetFather();

	}


	//! Finds a child node by the name given.
	CXmlNode* FindChildByName( const std::string& name )
	{
		if ( myUseStrict && myChildPosition >= myCurrentNode->GetChildCount() )
			logger << "Xml warning, ain't no childs left in the current node: " << myCurrentNode->GetName() << " in file: " << myFilename << " at line " << myLine << std::endl;

		if( HasChildNamed( name ) == false )
			return NULL;

		if ( myCurrentNode->GetChild( myChildPosition )->GetName() == name )
		{
			myChildPosition++;

			return myCurrentNode->GetChild( myChildPosition-1 );
		}

		if ( myUseStrict ) logger << "Xml warning a child node wasn't in the place it should be, order fucked up in node: " << myCurrentNode->GetName() << " in file: " << myFilename << " at line " << myLine << std::endl;

		for (	myChildPosition = 0;
				myChildPosition < myCurrentNode->GetChildCount() &&
					myCurrentNode->GetChild( myChildPosition )->GetName() != name;
				myChildPosition++ );

		if ( myChildPosition == myCurrentNode->GetChildCount() )
		{
			if ( myUseStrict ) logger << "Xml error a child node: " << name << " wasn't found at all in the node: " << myCurrentNode->GetName() << " in file: " << myFilename << " at line " << myLine << std::endl;
			return NULL;
		}

		return myCurrentNode->GetChild( myChildPosition );

	}

	//! Returns true if the child is found false if not
	bool HasChildNamed( const std::string& name )
	{

		if( myChildPosition >= 0 && myChildPosition < myCurrentNode->GetChildCount() &&
			myCurrentNode->GetChild( myChildPosition )->GetName() == name ) return true;

		int i;
		for ( i = 0; i < myCurrentNode->GetChildCount(); i++ )
		{
			if( myCurrentNode->GetChild( i )->GetName() == name ) return true;
		}

		return false;

	}

	//! Sets the line to myLine
	void SetLineNumber( unsigned int line ) {  myLine = line; }

	//! Sets the filename to myFilename
	void SetFileName( const std::string& file ) { myFilename = file; }

	//! Sets the strict, default true
	/*!
		This parameter decides does the CXmlFileSys biatch if it confronts
		a warning or error.
	*/
	void SetStrict( bool strict ) { myUseStrict = strict; }

	//! Returns the strict
	bool GetStrict() const { return myUseStrict; }

private:

	std::string		myFilename;
	unsigned int	myLine;
	bool			myUseStrict;


	FileSysStatus	myStatus;

	CXmlNode*		myRootElement;
	CXmlNode*		myCurrentNode;
	int				myChildPosition;

};

template< class T >
T XML_CAnyContainerCast( const CAnyContainer& any, const T& t )
{
	return CAnyContainerCast< T >( any );
}

#define XML_ConvertToAlias( x, y, a )	x->SetLineNumber( __LINE__ ); x->SetFileName( __FILE__ ); if( x->HasChildNamed( a ) ) { x->ConvertTo( y, a ); }
#define XML_ConvertFromAlias( x, y, a ) x->SetLineNumber( __LINE__ ); x->SetFileName( __FILE__ ); x->ConvertFrom( y, a )


#define XML_ConvertTo( x, y )	XML_ConvertToAlias( x, y, #y )
#define XML_ConvertFrom( x, y ) XML_ConvertFromAlias( x, y, #y )

#define XML_ConvertToContainerAlias( x, y, a )		x->SetLineNumber( __LINE__ ); x->SetFileName( __FILE__ ); x->ConvertToContainer( y, a )
#define XML_ConvertFromContainerAlias( x, y, a )	x->SetLineNumber( __LINE__ ); x->SetFileName( __FILE__ ); x->ConvertFromContainer( y, a )

#define XML_ConvertToContainer( x, y )		x->SetLineNumber( __LINE__ ); x->SetFileName( __FILE__ ); x->ConvertToContainer( y, #y )
#define XML_ConvertFromContainer( x, y )	x->SetLineNumber( __LINE__ ); x->SetFileName( __FILE__ ); x->ConvertFromContainer( y, #y )

#define XML_BindAlias( x, y, a) if ( x->IsReading() ) { XML_ConvertToAlias( x, y, a ); } else if ( x->IsWriting() ) { XML_ConvertFromAlias( x, y, a ); }
#define XML_Bind( x, y )		if ( x->IsReading() ) { XML_ConvertTo( x, y ); } else if ( x->IsWriting() ) { XML_ConvertFrom( x, y ); }
#define XML_BindMulti( x, y )	if ( x->IsReading() ) { XML_ConvertToContainer( x, y ); } else if ( x->IsWriting() ) { XML_ConvertFromContainer( x, y ); }
#define XML_BindMultiAlias( x, y, a )	if ( x->IsReading() ) { XML_ConvertToContainerAlias( x, y, a ); } else if ( x->IsWriting() ) { XML_ConvertFromContainerAlias( x, y, a ); }


#define XML_BindAttributeAlias( x, y, a ) if( x->IsWriting() ) { x->AddAttribute( a, y ); } else if( x->IsReading() ) { if( x->GetNode()->HasAttribute( a ) ) y = ::ceng::XML_CAnyContainerCast( x->GetNode()->GetAttributeValue( a ), y ); }
#define XML_BindAttribute( x, y ) XML_BindAttributeAlias( x, y, #y );


// -- Here's an implementation that helps with some cases. Not sure if I should it put it 
// here or not, but at least it's been pretty useful in place so what the hell
//----------------------------------------------------------------------------------
template< class T >
void XML_BindPtrAlias( ceng::CXmlFileSys* filesys, T*& pointer, const std::string& name )
{
	cassert( filesys );
	if( filesys->IsWriting() )
	{
		if( pointer ) XML_BindAlias( filesys, *pointer, name );
	}
	else if( filesys->IsReading() )
	{
		if( filesys->HasChildNamed( name ) == false )
		{
			// if we auto delete these?
			delete pointer;
			pointer = NULL;
		}
		else
		{
			if( pointer == NULL ) pointer = new T;
			XML_BindAlias( filesys, *pointer, name );
		}
	}
}


//-----------------------------------------------------------------------------
//
// Serializing a vector of pointers 
// this one is like one of the most used things I have reimplemented everytime
// now it's in a nice templated class
//

template< class T >
struct XmlVectorSerializer
{
	XmlVectorSerializer( std::vector< T* >& array, const std::string& name ) : array( array ), name( name ) { }

	void Serialize( ceng::CXmlFileSys* filesys )
	{
		if( filesys->IsWriting() )
		{
			for( std::size_t i = 0; i < array.size(); ++i )
			{
				if( array[ i ] )
					XML_BindAlias( filesys, *(array[i]), name );
			}
		}
		else if ( filesys->IsReading() )
		{
			// clears the pointers
			for( std::size_t i = 0; i < array.size(); ++i )
				delete array[ i ];

			array.clear();
			int i = 0;
			for( i = 0; i < filesys->GetNode()->GetChildCount(); i++ )
			{
				if( filesys->GetNode()->GetChild( i )->GetName() == name )
				{
					T* temp = new T;
					ceng::XmlConvertTo( filesys->GetNode()->GetChild( i ), *temp );
					array.push_back( temp );
				}
			}
		}
	}

	std::vector< T* >& array;
	std::string name;
};

//-----------------------------------------------------------------------------

} // end of namespace ceng 


#endif
