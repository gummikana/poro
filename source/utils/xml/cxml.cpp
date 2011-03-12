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


/*
 *  cxml.cpp
 *  Xcode_iPhoneTest2
 *
 *  Created by Dennis Belfrage on 4.3.10.
 *  Copyright 2010 Kloonigames. All rights reserved.
 *
 */

#include "cxml.h"

namespace ceng {
	
	//! Base class for Handlers
	//class CXmlHandlerInterface;
	//class CXmlStreamHandler;
#ifdef PORO_XCODE_ERROR_HACK_TOGGLE	 

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
	
#ifdef PORO_XCODE_ERROR_HACK_TOGGLE	 
	 template< class T >
	 void XmlSaveToFile( T& mesh, const std::string& file, const std::string& rootnodename  )
	 //void XmlSaveToFile( CPegManager& mesh, const std::string& file, const std::string& rootnodename  )
	 {
		 CXmlNode* node;
		 node = XmlConvertFrom( mesh, rootnodename );
		 
		 std::ofstream file_output( file.c_str(), std::ios::out );
		 
		 CXmlStreamHandler handler;
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
#ifdef PORO_XCODE_ERROR_HACK_TOGGLE	
	template< class T >
	void XmlLoadFromFile( T& mesh, const std::string& file, const std::string& rootnodename)
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
