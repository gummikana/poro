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
//	CXmlNode is a datastructure which reprisents a xml file. Each node can have
//	a x number of childer. Each of these child can have x number children and
//	so on and so on. Each Node also has attributes and some content. Not all
//	ofcourse but basicly each node can have attributes and some content.
//
// Created 01.10.2004 Pete
//
//.............................................................................
//
// 24.08.2006 Pete
//		Fixed some warnings that msvc2005 was throwing around.
//
// 14.03.2006 Pete
//		Fixed the AddChild() method to set the childs father to itself.
//
// 23.02.2005 Pete
//		Added the HasAttribute( string ) method
//
// 08.02.2005 Pete
//		Implented the Escape Characters in xml. They where put in the
//		SetContent method. Also a static variable called myReplaceEscapeChars;
//		was added so you can toggle the option of replacing the escape
//		characters with that variable
//
//=============================================================================
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#ifndef INC_CXMLNODE_H
#define INC_CXMLNODE_H

#include <string>
#include "xml_libraries.h"


namespace ceng {

class CXmlNode;
class CXmlNodeManager
{
public:
	CXmlNodeManager();

	CXmlNode* GetNew();
	void ReleaseNode( CXmlNode* node );

	std::vector< CXmlNode* > myFreeNodes;
};


//! CXmlNode represents a xml element.
class CXmlNode : public CMemoryPoolObject< CXmlNode, 5000 >
{
public:

	static CXmlNode*	CreateNewNode();
	static void			FreeNode( CXmlNode* node );

	void Clear();

	//-------------------------------------------------------------------------
	// Child manipulation

	bool			HasChilds()			const;
	const CXmlNode*	GetChild( int i )	const;
	CXmlNode*		GetChild( int i );
	int				GetChildCount()		const;

	void			AddChild( CXmlNode* child );
	void			RemoveChild( int i );
	void			RemoveChild( CXmlNode* child );

	//-------------------------------------------------------------------------

	bool					HasAttributes()									const;
	bool					HasAttribute( const std::string& attribute )	const;
	int						GetAttributeCount()								const;
	std::string				GetAttributeName( int i )						const;
	const CAnyContainer&	GetAttributeValue( int i )						const;
	const CAnyContainer&	GetAttributeValue( const std::string& name )	const;

	void					AddAttribute( const std::string& name, const CAnyContainer& value );
	void					RemoveAttribute( int i );
	void					RemoveAttribute( const std::string& name );

	//-------------------------------------------------------------------------

	std::string			GetName() const;
	std::string			GetContent() const;
	CXmlNode*			GetFather() const;

	void				SetName( const std::string& name );
	void				SetContent( const std::string& content );
	void				SetFather( CXmlNode* father );

	///////////////////////////////////////////////////////////////////////////

	CXmlNode() : myFather( NULL ) { }
	~CXmlNode();

private:


	static bool										myReplaceEscapeChars;

	CXmlNode*										myFather;
	std::string										myName;
	std::string										myContent;

	std::vector< CXmlNode* >						myXmlNodes;
	CMultiKeyVector< std::string, CAnyContainer >	myAttributes;

	static CXmlNodeManager							myManager;
	friend class CXmlNodeManager;
};

}
#endif
