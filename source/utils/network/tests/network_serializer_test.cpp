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


#include "../network_serializer.h"
#include "../network_libs.h"

#include <fstream>
#include <algorithm>

#ifdef PORO_TESTER_ENABLED

namespace network_utils
{
namespace test
{
//-----------------------------------------------------------------------------

int NetworkSerializerTest()
{

	{
		bool			test_bool = true;
		uint8			test_uint8 = 56;
		uint32			test_uint32 = 0x12345678;
		float32			test_float32 = 12465.4356f;
		types::ustring	test_ustring;
		test_ustring += (uint8)'n';
		test_ustring += (uint8)'o';
		test_ustring += (uint8)'o';
		test_ustring += (uint8)'b';

		CSerialSaver saver;
		saver.IO( test_bool );
		saver.IO( test_uint8 );
		saver.IO( test_uint32 );
		saver.IO( test_float32 );
		saver.IO( test_ustring );

		types::ustring buffer = saver.GetData();
		// std::cout << buffer.size() << std::endl;

		bool			load_bool = true;
		uint8			load_uint8 = 56;
		uint32			load_uint32 = 0x12345678;
		float32			load_float32 = 12465.4356f;
		types::ustring	load_ustring;
		
		CSerialLoader loader( buffer );
		loader.IO( load_bool );
		loader.IO( load_uint8 );
		loader.IO( load_uint32 );
		loader.IO( load_float32 );
		loader.IO( load_ustring );
		
		test_assert( test_bool == load_bool );
		test_assert( test_uint8 == load_uint8 );
		test_assert( test_uint32 == load_uint32 );
		test_float( test_float32 == load_float32 );
		test_assert( test_ustring == load_ustring );
	}

	// this works fine
	{
		int test_int32 = 1;

		CSerialSaver saver;
		saver.IO( test_int32 );

		types::ustring buffer = saver.GetData();

		int load_int32 = -1;
		
		CSerialLoader loader( buffer );
		loader.IO( load_int32 );

		test_assert( test_int32 == load_int32 );
	}

	// char buffer test
	{
		int test_int32 = 1;

		CSerialSaver saver;
		saver.IO( test_int32 );

		types::ustring buffer = saver.GetData();

		char char_buffer[1024];
		std::copy(buffer.begin(), buffer.end(), char_buffer);
		char_buffer[buffer.size()] = '\0'; // don't forget the terminating 0

		types::ustring buffer_from_char_shit;
		buffer_from_char_shit.resize( buffer.size() );
		std::copy( char_buffer, char_buffer + buffer.size(), buffer_from_char_shit.begin() );

		int load_int32 = -1;
		
		CSerialLoader loader( buffer_from_char_shit );
		loader.IO( load_int32 );

		test_assert( test_int32 == load_int32 );

	}

	return 0;
}

template <typename T>
void swap_endian(T& pX)
{
    // should static assert that T is a POD

    char& raw = reinterpret_cast<char&>(pX);
    std::reverse(&raw, &raw + sizeof(T));
}

inline void endian_swap(unsigned short& x)
{
    x = (x>>8) | 
        (x<<8);
}

unsigned char littleShort( unsigned char l )
{
  unsigned char b1, b2;

  b1 = l & 64;
  b2 = ( l >> 6 ) & 64;

  return ( b1 << 6 ) + b2;
}




int NetworkSerializerEndianTest()
{
	{
		uint8 test_uint8 = 64;
		
		CSerialSaver saver;
		saver.IO( test_uint8 );

		types::ustring buffer = saver.GetData();

		test_assert( buffer.size() == 1 );
		test_assert( buffer[0] == '@' );
	}

	{
		uint32 test_uint32 = 64;
		
		CSerialSaver saver;
		saver.IO( test_uint32 );

		types::ustring buffer = saver.GetData();

		test_assert( buffer.size() == 4 );
		test_assert( buffer[3] == '@' );
		test_assert( buffer[3] == 0x40 );

		
		std::fstream file_output;
		file_output.open( "temp/endian_out.txt" , std::ios::out );
		file_output << buffer;
		file_output.close();
		


		// file read test
		std::fstream file_input;
		file_input.open( "temp/endian_cmpr.txt" , std::ios::in );
		test_assert( file_input.good() );

		types::ustring line_in;
		std::getline( file_input, line_in );
		
		file_input.close();

		test_assert( line_in.size() == buffer.size() );
		for( std::size_t i = 0; i < line_in.size(); ++i )
			test_assert( line_in[i] == buffer[i] );
	}

	
	return 0;
}

//-----------------------------------------------------------------------------
TEST_REGISTER( NetworkSerializerTest );
// TEST_REGISTER( NetworkSerializerEndianTest );

} // end o namespace test
} // end o namespace network_utils

#endif // PORO_TESTER_ENABLED