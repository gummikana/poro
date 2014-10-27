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


#include "cxmlparser.h"

#include <fstream>
#include <iostream>

#include "cxmlhandler.h"


// #include "../basepath/basepath.h"

#define ADD_BASE_PATH( x ) x

namespace ceng {

void CXmlParser::ParseFile( const std::string& file )
{
	myHandler->StartDocument();

	std::fstream file_input;
    
	std::string line;

	if( file.empty() == false )
	{
		std::string t = ADD_BASE_PATH(file.c_str());
		file_input.open(t.c_str() , std::ios::in );

		if(!file_input.good())
		{
			logger << "Unable to open xml-file: " << file.c_str() << "\n";
		}
		while ( file_input.good() ) 
		{
			std::getline( file_input, line );
			
			if( line.size() > 256 )
			{
				std::vector< std::string > lines = ceng::Split( ">", line );
				for( unsigned int i = 0; i < lines.size(); ++i )
				{
					ParseLine( lines[ i ] + ">" );    
				}
			}
			else
			{
				ParseLine( line );    
			}
		}

		file_input.close();
	}

	myHandler->EndDocument();
}

void CXmlParser::ParseStringData( const std::string& stringdata )
{
	myHandler->StartDocument();

	std::vector< std::string > lines = ceng::Split( ">", stringdata + " " );
	for( unsigned int i = 0; i < lines.size(); ++i )
	{
		if( i < lines.size() - 1 ) 
			ParseLine( lines[ i ] + ">" );    
		else
			ParseLine( lines[ i ] );    
	}
	myHandler->EndDocument();
}

void CXmlParser::ParseLine( const std::string& in_line )
{
	std::string line = in_line;
	
	// added " " to fix the white space bug in attribute parsing
	if ( myRemoveWhiteSpace ) 
		line = RemoveWhiteSpace( line ) + " ";
	
	size_t i;

	if ( myStatus == content )
	{
		i = line.find_first_of("<");
           
        if ( i == line.npos ) 
		{
			myContentBuffer += line;
			return;
		} else {
			myContentBuffer += line.substr( 0, i );
			myStatus = tag;
			ParseContentBuffer();
			ParseLine( line.substr( i + 1 ) );
			return;
		}
	} else if ( myStatus == tag )
	{
		i = line.find_first_of(">");
		
		if ( i == line.npos )
		{
			myTagBuffer += line;
			return;
		} else {
			myTagBuffer += line.substr( 0, i );
			myStatus = content;
			ParseTagBuffer();
			ParseLine( line.substr( i + 1 ) );
			return;
		}
	}
}

void CXmlParser::ParseContentBuffer()
{
	
	while ( myRemoveWhiteSpace && myContentBuffer.size() > 1 && myContentBuffer[ myContentBuffer.size() - 1 ] == '\n' ) 
		myContentBuffer = myContentBuffer.substr( 0, myContentBuffer.size() - 1 );
	
	if ( myContentBuffer.empty() ) return;

	if( myRemoveWhiteSpace ) 
		myContentBuffer = RemoveWhiteSpace( myContentBuffer );	
	
	myHandler->Characters( myContentBuffer );
	myContentBuffer = "";

}

void CXmlParser::ParseTagBuffer()
{
	if ( myRemoveWhiteSpace ) myTagBuffer = RemoveWhiteSpace( myTagBuffer );
	while ( myRemoveWhiteSpace && myTagBuffer.size() > 1 && myTagBuffer[ myTagBuffer.size() - 1 ] == '\n' ) myTagBuffer = myTagBuffer.substr( 0, myTagBuffer.size() - 1 );
	
	if ( myTagBuffer.empty() ) return;

	
	
	std::string line = myTagBuffer;
	myTagBuffer = "";

	if ( line[0] == '/' ) 
	{
		myHandler->EndElement( line.substr( 1 ) );
		return;
	}

	if ( line[0] == '!' )
	{
		return;
	}

	size_t i = line.find_first_of( "=" );
	if ( i != line.npos ) line = ParseAttributes( line );
	
	if ( line.empty() ) return;

	if ( line[ line.size() - 1 ] == '/' )
	{	
		line = line.substr( 0, line.size() - 1 );
		line = RemoveWhiteSpace( line );
		myHandler->StartElement( line, myAttributeBuffer );
		myAttributeBuffer.clear();
		myHandler->EndElement( line );

		return;
	}

	myHandler->StartElement( line, myAttributeBuffer );
	myAttributeBuffer.clear();

}

std::string CXmlParser::ParseAttributes( const std::string& line )
{
	std::string tmp = line;
	size_t i = tmp.find_first_of( "=" );
	if ( i == tmp.npos ) return tmp;
	
	std::string key_part	= RemoveWhiteSpace( tmp.substr( 0, i ) );
	std::string value_part  = RemoveWhiteSpace( tmp.substr( i + 1 ) );

	i = key_part.find_last_of(" \t");
	if ( i != key_part.npos ) 
	{
		tmp = key_part.substr( 0, i );
		key_part = key_part.substr( i + 1 );
	}

	i = StringFindFirstOf( " \t/", value_part );
	if ( i != value_part.npos )
	{
		tmp += value_part.substr( i );
		value_part = value_part.substr( 0, i);
	}

	value_part = RemoveWhiteSpace( value_part );
	value_part = RemoveQuotes( value_part );
	// value_part = value_part.substr( 1, value_part.size() - 2 );
	
	myAttributeBuffer.insert( std::pair< std::string, CAnyContainer >( key_part, value_part ) );

	return ParseAttributes( tmp );	
}

}