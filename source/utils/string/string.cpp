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
// string.cpp
//
// Writen 10.10.2003
// by Petri Purho ( pete@markkupurho.fi )
//
// You are free to use this as you wish.                                  
//                                                                    
///////////////////////////////////////////////////////////////////////////////

#include "string.h"
#include <stdlib.h>
#include <ctype.h>

// #include "../../config/cengdef.h"

// #include "../../outside resources/memory management/debug_memorymanager.h"
namespace ceng {

///////////////////////////////////////////////////////////////////////////////

std::vector <std::string> Split( const std::string& _separator, std::string _string )
{

    std::vector <std::string> array;

    size_t position;
    
    // we will find the position of first of the separators
    position = _string.find( _separator );
    
    // We will loop true this until there are no separators left
    // in _string
    while ( position != _string.npos )
    {
    
        // This thing here checks that we dont push empty strings
        // to the array
        if ( position != 0 )
            array.push_back( _string.substr( 0, position ) );

        // When the cutted part is pushed into the array we
        // remove it and the separator from the _string
        _string.erase( 0, position + _separator.length() );

        // And the we look for a new place for the _separator
        position = _string.find( _separator );
    }

    // We will push the rest of the stuff in to the array
    if ( _string.empty() == false )
        array.push_back( _string );

    // Then we'll just return the array
    return array;
}

//=============================================================================

// Same thing as above but with _limit
std::vector <std::string> Split( const std::string& _separator, std::string _string, int _limit )
{

    std::vector <std::string> array;

    size_t position;
    position = _string.find( _separator );

    // The only diffrence is here
    int count = 0;  

    // and in this while loop the count <= _limit
    while ( position != _string.npos &&  count < _limit - 1 )
    {
    
        if ( position != 0 )
            array.push_back( _string.substr( 0, position ) );

        _string.erase( 0, position + _separator.length() );

        position = _string.find( _separator );

        // And here
        count++;
    }
    if ( _string.empty() == false )
        array.push_back( _string );

    return array;
}

//=============================================================================

std::vector<std::string> StringSplit( const std::string& _separator, std::string _string )
{

    std::vector <std::string> array;

    size_t position;
	
    // we will find the position of first of the separators
    position = StringFind( _separator, _string );

	
	// We will loop true this until there are no separators left
    // in _string
    while ( position != _string.npos )
    {
    
        // This thing here checks that we dont push empty strings
        // to the array
        if ( position != 0 )
            array.push_back( _string.substr( 0, position ) );

        // When the cutted part is pushed into the array we
        // remove it and the separator from the _string
        _string.erase( 0, position + _separator.length() );

        // And the we look for a new place for the _separator
        position = StringFind( _separator, _string );
    }

    // We will push the rest of the stuff in to the array
    if ( _string.empty() == false )
        array.push_back( _string );

    // Then we'll just return the array
    return array;


}

//=============================================================================
// Same thing as above but with _limit
std::vector <std::string> StringSplit( const std::string& _separator, std::string _string, int _limit )
{

    std::vector <std::string> array;

    size_t position;
    position = StringFind( _separator, _string );

    // The only diffrence is here
    int count = 0;  

    // and in this while loop the count <= _limit
    while ( position != _string.npos &&  count < _limit - 1 )
    {
    
        if ( position != 0 )
            array.push_back( _string.substr( 0, position ) );

        _string.erase( 0, position + _separator.length() );

        position = StringFind( _separator, _string );

        // And here
        count++;
    }
    if ( _string.empty() == false )
        array.push_back( _string );

    return array;
}

///////////////////////////////////////////////////////////////////////////////

size_t StringFind( const std::string& _what, const std::string& _line, size_t _begin )
{
	
	size_t return_value = _line.find( _what, _begin );
	
	if ( return_value == _line.npos )
	{
		return return_value;
	}

	// for the very begin we'll first find out
	// if there even is a quetemark in the string
	size_t quete_begin = _line.find("\"", _begin );

	// if there isn't well return the position of _what
	if ( quete_begin == _line.npos )
	{
		return return_value;
	}

	if ( quete_begin > return_value )
	{
		return return_value;
	}

	// Then heres the other vars used here
	size_t quete_end	= _line.find( "\"", quete_begin+1 );
	

	while ( quete_begin < return_value )
	{
		if ( quete_end > return_value )
		{
			return_value = _line.find( _what, return_value+1 );
			if ( return_value == _line.npos ) return return_value;
		}

		if ( quete_end < return_value )
		{
			quete_begin = _line.find( "\"", quete_end+1 );
			quete_end   = _line.find( "\"", quete_begin+1 );
		}
	}

	return return_value;
}

///////////////////////////////////////////////////////////////////////////////

size_t StringFindFirstOf( const std::string& what, const std::string& line, size_t begin )
{
	size_t return_value = line.find_first_of( what, begin );
	
	if ( return_value == line.npos )
	{
		return return_value;
	}

	// for the very begin we'll first find out
	// if there even is a quetemark in the string
	size_t quete_begin = line.find("\"", begin );

	// if there isn't well return the position of what
	if ( quete_begin == line.npos )
	{
		return return_value;
	}

	if ( quete_begin > return_value )
	{
		return return_value;
	}

	// Then heres the other vars used here
	size_t quete_end	= line.find( "\"", quete_begin+1 );
	

	while ( quete_begin < return_value )
	{
		if ( quete_end > return_value )
		{
			return_value = line.find_first_of( what, return_value+1 );
			if ( return_value == line.npos ) return return_value;
		}

		if ( quete_end < return_value )
		{
			quete_begin = line.find( "\"", quete_end+1 );
			quete_end   = line.find( "\"", quete_begin+1 );
		}
	}

	return return_value;
}

///////////////////////////////////////////////////////////////////////////////

std::string StringReplace( const std::string& what, const std::string& with, const std::string& in_here, int limit )
{

	int i = 0;
	std::string return_value = in_here;
	size_t pos = return_value.find( what, 0 );

	while( pos != return_value.npos && ( limit == -1 || i < limit )  )
	{
		return_value.replace( pos, what.size(), with );
		pos = return_value.find( what, pos );
		i++;
	}

	return return_value;
}

///////////////////////////////////////////////////////////////////////////////

std::string RemoveWhiteSpace( std::string line )
{
	// std::string line( _line );

	size_t position = line.find_first_not_of(" \t");
    if( position != 0 ) 
		line.erase( 0,  position );

    position = line.find_last_not_of(" \t\r");
    if( position != line.size() - 1 )
		line.erase( position+1 );

	return line;
}

std::string RemoveWhiteSpaceAndEndings( std::string line )
{
	size_t position = line.find_first_not_of(" \t\r\n");
    if( position != 0 ) 
		line.erase( 0,  position );

    position = line.find_last_not_of(" \t\r\n");
    if( position != line.size() - 1 )
		line.erase( position+1 );

	return line;
}

///////////////////////////////////////////////////////////////////////////////

std::string RemoveQuotes( std::string line )
{
	if( line.empty() == false && (line[ 0 ] == '"' || line[ 0 ] == '\''))
		line = line.substr( 1, line.size() - 1 );
	
	int end = ((int)line.size()) - 1;
	if( end >= 0 && ( line[ end ] == '"' || line[ end ] == '\'' ) )
		line = line.substr( 0, end );

	return line;
}

///////////////////////////////////////////////////////////////////////////////

std::string Uppercase( const std::string& _string )
{
    std::string return_value;

    for ( unsigned int i = 0; i < _string.size(); i++ )
    {
        return_value += toupper( _string[ i ] );
    }

    return return_value;

}

//=============================================================================

std::string Lowercase( const std::string& _string )
{
    std::string return_value;

    for ( unsigned int i = 0; i < _string.size(); i++ )
    {
        return_value += tolower( _string[ i ] );
    }

    return return_value;

}

///////////////////////////////////////////////////////////////////////////////

std::string convertNumberToString( int num )
{
	std::string result = "";
	while( num >= 94 )
	{
		result += char( 126 );
		num -= 94;
	}

	result += char( num + 32 );

	return result;
}

//=============================================================================

std::string ConvertNumbersToString( const std::vector< int >& array )
{
	std::string result;

	for( unsigned int i = 0; i < array.size(); i++ )
	{
		result += convertNumberToString( array[ i ] );
	}

	return result;
}

//=============================================================================

std::vector< int > ConvertStringToNumbers( const std::string& str )
{
	std::vector< int > result;
	
	int vector_pos = 0;
	result.push_back( 0 );
	for( unsigned int i = 0; i < str.size(); i++ )
	{
		result[ vector_pos ] += str[ i ] - 32;
		if( str[ i ] != char( 126 ) )
		{
			vector_pos++;
			result.push_back( 0 );
		}
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////

bool ContainsString( const std::string& what, const std::string& in_where )
{
	if( in_where.empty() )
		return what.empty();

	size_t pos = in_where.find( what );
	
	if( pos == in_where.npos )
		return false;

	return true;
}

bool ContainsStringChar( const std::string& what, const char* in_where, int length )
{
	if( what.empty() )
		return ( length == 0 );

	const char* pos = in_where;
	unsigned int j = 0;

	for( int i = 0; i < length; ++i )
	{
		if( *pos == what[ j ] )
		{
			j++;
			if( j >= what.size() )
				return true;
		}
		else
		{
			j = 0;
		}
		pos++;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////

bool IsNumericString( const std::string& what )
{
	return (what.empty() == false && what.find_first_not_of("0123456789.+-") == what.npos );
}

///////////////////////////////////////////////////////////////////////////////

bool CheckIfTheBeginningIsSame( const std::string& beginning, const std::string& line )
{
	if( line.size() > beginning.size() )
	{
		if( line.substr( 0, beginning.size() ) == beginning )
			return true;
	}
	else if( line == beginning )
	{
		return true;
	}
	else
	{
		return false;
	}

	return false;
}

std::string RemoveBeginning( const std::string& beginning, const std::string& line )
{
	return line.substr( beginning.size() );
}

///////////////////////////////////////////////////////////////////////////////

namespace {
	bool IsAlphaNumeric( char c )
	{
		if( c >= '0' && c <= '9' )
			return true;
		if( c >= 'a' && c <= 'z' )
			return true;
		if( c >= 'A' && c <= 'Z' )
			return true;
		return false;
	}

	char ConvertToAlphaNumeric( char what )
	{
		int randnum = (int)what % 61;
		if( randnum < 10 )
		{
			return char( randnum + 48 );
		}
		else if( randnum < 36 )
		{
			return char( randnum + 55 );
		}
		else
		{
			return char( randnum + 61 );
		}

		return 'a';
	}
}

std::string MakeAlphaNumeric( const std::string& who )
{
	std::string result;
	for( unsigned int i = 0; i < who.size(); ++i )
	{
		char t = who[ i ];
		if( IsAlphaNumeric( t ) )
			result += t;
	}
	return result;
}

std::string ConvertToAlphaNumeric( const std::string& who )
{
	std::string result;
	for( unsigned int i = 0; i < who.size(); ++i )
	{
		char t = who[ i ];
		if( IsAlphaNumeric( t ) )
			result += t;
		else
			result += ConvertToAlphaNumeric( t );
	}
	return result;
}


///////////////////////////////////////////////////////////////////////////////
} // end of namespace ceng
 
