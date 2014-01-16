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
//
//  CXML v1.3.0
//  ===========
//
//
//	CXml (A.K.A ) Ceng Xml is a pretty simple xml parser, handler and convertor
//	for Ceng ( 2D game engine ).
//
//	Ceng Xml includes a parser, so you can read a Xml file to what ever format
//	or classes you want. This rather simple parser can be found at CXmlParser.h
//
//	Ceng Xml also comes with few different handlers to help you parse or create
//	your own dream handler. Just derivate CXmlHandlerInterface and give an
//	instance of your class to the CXmlParser and voila you can parse xml files
//	to your own pretty xml node system.
//
//	Ceng Xml btw also comes with a ready to use node system found in CXmlNode.h
//	Used by Ceng Xml internaly you can use them rather easily externaly or to
//	what ever purpouse you desire. CXmlCast can help you convert your CXmlNodes
//	to more proper types. CXmlHandler class also helps you to parse files
//	directly to CXmlNodes.
//
//	There are also ways of doing the same things reverse.
//
//	But the coolest thing in Ceng Xml is the easy way of converting meshs to
//  xml files and xml files to meshs. The functions for that can be found in
//  here the Ceng Xml header.
//
//	XmlSaveToFile( ... )	saves a mesh to a give file
//  XmlLoadFromFile( ... )	loads a mesh from the given file
//
//  Whats the catch you ask? The catch is kinky.
//
//  You have to implent the method Serialize( CXmlFileSys* file ).
//
//	This gives you a lot of playground. The basic idea is that the mesh can
//	load its ars from a CXmlFileSys and it can save its ars to one of those.
//
//	If you have a good string conversion already implented in your class you
//	can use that if you desire. Heres an example of Serialize method implented
//  for the class Person
/*

	class Person
	{
	public:
		void Serialize( CXmlFileSys* system )
		{
			XML_Bind( system, myName );
			XML_Bind( system, myAge );
		}

		std::string	myName;
		int			myAge;
	};

*/
//  As you can see thats pretty nice and simply way to serialize your meshs.
//	The Serializers should write and load stuff in the same order but that
//	isn't a must. ( If you have CXML_CFG_USE_STRICT defined it will biatch
//	about it )
//
//	To save and load Persons instance from a file:
/*

	Person pers;

	XmlSaveToFile( pers, "person.xml" );
	XmlLoadFromFile( pers, "person.xml" );

*/
//
//	If you want to look around for the inside workings of these thing go ahead.
//	There is a rather extendable base for you xml applications. Use Ìt as you
//  desire.
//
//
// Created 01.10.2004 by Petri Purho
//.............................................................................
//
// 12.06.2006 Pete
//		Fixed couple of crash bugs. (crashing when reading fucked up xml files)
//		Fixed the self ending tags, so that now they should work <element/>
//		Versio number 1.3.0
//
//
// 28.06.2006 Pete
//		Moved the "external libraries" like CAnyContainer and stuff to the
//		xml_libraries.h. Now the moving and swapping between projects where the
//		stuff is already included comes somewhat easier. No real bug-fixes.
//
// 08.05.2006 Pete
//		Cleaned the directory listing to better suit reuseabilty. Added the
//		xml_macro.h header file for easy changing of the assert, log and
//		unit testing macros. So this shit could be called the release 1.1
//		of cxml serializer.
//
// 14.02.2005 Pete
//		Added support for XML_BindMulti( ), but it is rather stupid way to
//		implement this. It is a hack because Visual C++ 6.0 doesn't support
//		template specialization properly. And it sucks!
//
//=============================================================================
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#ifndef INC_CXML_H
#define INC_CXML_H

#include <string>
#include <fstream>

#include "cxmlhandler.h"
#include "cxmlcast.h"
#include "cxmlfilesys.h"
#include "cxmlnode.h"
#include "cxmlparser.h"

//#define PORO_XCODE_ERROR_HACK_TOGGLE

namespace ceng {

	//! Base class for Handlers
	//class CXmlHandlerInterface;
	//class CXmlStreamHandler;


	template< class T >
	CXmlNode* XmlSaveToMemory( T& mesh, const std::string& rootnodename = "rootelement" );

	template< class T >
	void XmlLoadFromMemory( T& mesh, CXmlNode* node, const std::string& rootnodename = "rootelement" );

#ifndef PORO_XCODE_ERROR_HACK_TOGGLE
	template< class T >
	CXmlNode* XmlSaveToMemory( T& mesh, const std::string& rootnodename  )
	{
		CXmlNode* node;
		node = XmlConvertFrom( mesh, rootnodename );
		
		return node;	
	}

	template< class T >
	void XmlLoadFromMemory( T& mesh, CXmlNode* node, const std::string& rootnodename  )
	{
		XmlConvertTo( node, mesh );
	}
#endif

//! Saves the mesh to the xml file.
/*!
	Creates a xml file named file, from the mesh given. You have to give the
	same rootnodename to both saving and loading the mesh. Use the default,
	if you don't want get fancy on you ass.
*/
	template< class T >
	void XmlSaveToFile( T& mesh, const std::string& file, const std::string& rootnodename = "rootelement", bool parse_on_multiple_lines = false );
	//void XmlSaveToFile( CPegManager& mesh, const std::string& file, const std::string& rootnodename = "rootelement" );

#ifndef PORO_XCODE_ERROR_HACK_TOGGLE
	template< class T >
	inline void XmlSaveToFile( T& mesh, const std::string& file, const std::string& rootnodename, bool parse_on_multiple_lines  )
	//void XmlSaveToFile( CPegManager& mesh, const std::string& file, const std::string& rootnodename  )
	{
		CXmlNode* node;
		node = XmlConvertFrom( mesh, rootnodename );

		std::ofstream file_output( file.c_str(), std::ios::out );

		CXmlStreamHandler handler;
		if( parse_on_multiple_lines )
		{
			handler.SetPackThight( false );
			handler.SetWriteAttributesOnLines( true );
			handler.SetExtraLineBetweenTags( true );
		}

		handler.ParseOpen( node, file_output );

		file_output.close();

		ceng::CXmlNode::FreeNode( node );
	}
#endif
//! Loads a mesh from the xml file.
/*!
	Calls the meshs Serialize( CXmlFileSys* file ) method to serialize the
	mesh. You should use the same rootnodenae for saving and loading the
	meshs.
	.
*/

	template< class T >
	void XmlLoadFromFile( T& mesh, const std::string& file, const std::string& rootnodename = "rootelement" );

#ifndef PORO_XCODE_ERROR_HACK_TOGGLE
	template< class T >
	inline void XmlLoadFromFile( T& mesh, const std::string& file, const std::string& rootnodename)
	{
		CXmlParser	parser;
		CXmlHandler handler;

		parser.SetHandler( &handler );
		parser.ParseFile( file.c_str() );

		XmlConvertTo( handler.GetRootElement(), mesh );

		CXmlNode::FreeNode( handler.GetRootElement() );
	}
#endif
/*	void XmlLoadFromFile2( const std::string& mesh, const std::string& file, const std::string& rootnodename = "rootelement" )
	{
		ceng::CXmlParser	parser;
		ceng::CXmlHandler handler;

		parser.SetHandler( &handler );
		parser.ParseFile( file.c_str() );

		// XmlConvertTo( handler.GetRootElement(), mesh );

		CXmlNode::FreeNode( handler.GetRootElement() );

	}
*/

}

#endif
