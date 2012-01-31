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


#include "../xml_macros.h"
#include "../canycontainer.h"
#include <vector>
#include <list>

#ifdef PORO_TESTER_ENABLED

namespace ceng {
namespace test {

///////////////////////////////////////////////////////////////////////////////

namespace {
	class CAnyFucker
	{
	public:
		CAnyFucker() { }
		CAnyFucker( int i ) : i( i ) { }
		~CAnyFucker() { }
		int i;
	};
}

///////////////////////////////////////////////////////////////////////////////

int CAnyContainerTest()
{
	// basic tests
	{
		CAnyContainer test;
		test_assert( test.IsEmpty() );
		
		{
			std::string t = test.GetType();
			std::string v = test.GetValue();
			const std::type_info& i = test.GetTypeInfo();
		}

		// just plain old int
		{
			test = 1;
			test_assert( test.IsEmpty() == false );
			test_assert( test.GetType() == typeid( int ).name() );
			test_assert( test.GetValue() == "1" );
			test_assert( test.GetTypeInfo() == typeid( int ) );

			int i = CAnyContainerCast< int >( test );
			test_assert( i == 1 );
		}

		// unsigned int
		{
			const unsigned int ui = 2;
			test = ui;

			test_assert( test.IsEmpty() == false );
			test_assert( test.GetType() == typeid( ui ).name() );
			test_assert( test.GetValue() == "2" );
			test_assert( test.GetTypeInfo() == typeid( ui ) );

			unsigned int i = CAnyContainerCast< unsigned int >( test );
			test_assert( i == 2 );
		}

		// std::string
		{
			const std::string si = "pertsa";
			test = si;

			test_assert( test.IsEmpty() == false );
			test_assert( test.GetType() == typeid( si ).name() );
			test_assert( test.GetValue() == "pertsa" );
			test_assert( test.GetTypeInfo() == typeid( si ) );

			std::string i = CAnyContainerCast< std::string >( test );
			test_assert( i == "pertsa" );
		}

		// testing the empty container for a while
		{
			test = CAnyContainer();
			test_assert( test.IsEmpty() );
			std::string t = test.GetType();
			std::string v = test.GetValue();
			const std::type_info& i = test.GetTypeInfo();
		}

		// CAnyFucker
		{
			test = CAnyFucker( 5 );

			test_assert( test.IsEmpty() == false );
			test_assert( test.GetType() == typeid( CAnyFucker ).name() );
			test_assert( test.GetValue() == "" );
			test_assert( test.GetTypeInfo() == typeid( CAnyFucker ) );
			
			CAnyFucker i = CAnyContainerCast< CAnyFucker >( test );
			test_assert( i.i == 5 );
		}
	}

	// now we will fuck with the types
	{
		CAnyContainer test;

		// just plain old int
		{
			test = 1;
			test_assert( test.IsEmpty() == false );
			test_assert( test.GetType() == typeid( int ).name() );
			test_assert( test.GetValue() == "1" );
			test_assert( test.GetTypeInfo() == typeid( int ) );

			int ti = CAnyContainerCast< int >( test );
			test_assert( ti == 1 );

			unsigned int tu = CAnyContainerCast< unsigned int >( test );
			test_assert( tu == 1 );

			float tf = CAnyContainerCast< float >( test );
			test_assert( tf == 1 );

			double td = CAnyContainerCast< double >( test );
			test_assert( td == 1 );

			short ts = CAnyContainerCast< short >( test );
			test_assert( ts == 1 );

			long tl = CAnyContainerCast< long >( test );
			test_assert( tl == 1 );

			bool tb = CAnyContainerCast< bool >( test );
			test_assert( tb == 1 );

			std::string tss = CAnyContainerCast< std::string >( test );
			test_assert( tss == "1" );

			CAnyFucker fucked = CAnyContainerCast< CAnyFucker >( test );
		}

		{
			test = std::string( "1" );
			test_assert( test.IsEmpty() == false );
			test_assert( test.GetType() == typeid( std::string ).name() );
			test_assert( test.GetValue() == "1" );
			test_assert( test.GetTypeInfo() == typeid( std::string ) );

			int ti = CAnyContainerCast< int >( test );
			test_assert( ti == 1 );

			unsigned int tu = CAnyContainerCast< unsigned int >( test );
			test_assert( tu == 1 );

			float tf = CAnyContainerCast< float >( test );
			test_assert( tf == 1 );

			double td = CAnyContainerCast< double >( test );
			test_assert( td == 1 );

			short ts = CAnyContainerCast< short >( test );
			test_assert( ts == 1 );

			long tl = CAnyContainerCast< long >( test );
			test_assert( tl == 1 );

			bool tb = CAnyContainerCast< bool >( test );
			test_assert( tb == 1 );

			std::string tss = CAnyContainerCast< std::string >( test );
			test_assert( tss == "1" );

			CAnyFucker fucked = CAnyContainerCast< CAnyFucker >( test );
		}

	}

	// testing the memory management
	{
		CAnyContainer t1 = 1;
		CAnyContainer t2 = (std::string)"2";
		// t1 = t2;
	}
	// testing with various containers
	{
		// vector
		{
			std::vector< CAnyContainer > v;
			v.push_back( 0 );
			v.push_back( std::string( "1" ) );
			v.push_back( 2 );
			v.push_back( CAnyFucker( 3 ) );

			test_assert( CAnyContainerCast< int >( v[ 0 ] ) == 0 );
			test_assert( CAnyContainerCast< int >( v[ 1 ] ) == 1 );
			test_assert( CAnyContainerCast< std::string >( v[ 2 ] ) == "2" );
			test_assert( CAnyContainerCast< CAnyFucker >( v[ 3 ] ).i == 3 );

			// this fucks things up
			// v[ 0 ] = v[ 1 ];
			/*test_assert( CAnyContainerCast< int >( v[ 0 ] ) == 1 );
			test_assert( CAnyContainerCast< int >( v[ 1 ] ) == 1 );
			test_assert( CAnyContainerCast< std::string >( v[ 0 ] ) == "1" );
			test_assert( CAnyContainerCast< std::string >( v[ 1 ] ) == "1" );
			*/
		}
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////

TEST_REGISTER( CAnyContainerTest );

///////////////////////////////////////////////////////////////////////////////

} // end of namespace test
} // end of namespace ceng

#endif // PORO_TESTER
