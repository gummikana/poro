#include "../../debug.h"
#include "../ctexturemanager.h"

#ifdef CENG_TESTER_ENABLED

#pragma warning ( disable : 4503)

namespace ceng {
namespace test {

namespace {
class CTextureManagerTestClass
{
public:
	CTextureManagerTestClass() 
	{ 
		count++;
	}

	~CTextureManagerTestClass() 
	{ 
		count--;
	}

	int i;

	static int count;
};

int CTextureManagerTestClass::count = 0;


} // end of anonymous namespace


int CTextureManagerTest()
{
#ifndef NDEBUG
	{
		
		typedef CTextureManager< CTextureManagerTestClass* > test_manager;
		test_manager::GetSingletonPtr()->SetLogErrors( false );

		CTextureManagerTestClass* t = new CTextureManagerTestClass;
		t->i = 1;
		test_assert( CTextureManagerTestClass::count == 1 );

		test_manager::GetSingletonPtr()->AddNew( "1", t, false );

		// testing HasFile method
		test_assert( test_manager::GetSingletonPtr()->HasFile( "1" ) );
		test_assert( test_manager::GetSingletonPtr()->HasFile( "0" ) == false );
		test_assert( test_manager::GetSingletonPtr()->HasFile( "2" ) == false );
		test_assert( test_manager::GetSingletonPtr()->HasFile( "" ) == false );
		
		// GetFile
		test_assert( test_manager::GetSingletonPtr()->GetPointer( "1" ) == t );
		test_assert( test_manager::GetSingletonPtr()->GetPointer( "0" ) == NULL );
		test_assert( CTextureManagerTestClass::count == 1 );

		// ReleasePointer fun
		test_manager::GetSingletonPtr()->ReleasePointer( t );
		test_assert( CTextureManagerTestClass::count == 1 );
		test_manager::GetSingletonPtr()->ReleasePointer( t );
		test_assert( CTextureManagerTestClass::count == 0 );

		test_manager::GetSingletonPtr()->ReleasePointer( 0 );

		// With the preloaded option
		t = new CTextureManagerTestClass;
		t->i = 1;
		test_assert( CTextureManagerTestClass::count == 1 );

		test_manager::GetSingletonPtr()->AddNew( "1", t, true );
		test_manager::GetSingletonPtr()->ReleasePointer( t );
		test_assert( CTextureManagerTestClass::count == 1 );
		
		// UnloadFile
		test_manager::GetSingletonPtr()->UnloadFile( "1" );
		test_assert( CTextureManagerTestClass::count == 0 );

		t = new CTextureManagerTestClass;
		t->i = 1;
		test_assert( CTextureManagerTestClass::count == 1 );

		test_manager::GetSingletonPtr()->AddNew( "1", t, true );
		test_manager::GetSingletonPtr()->UnloadFile( "1" );
		test_assert( CTextureManagerTestClass::count == 1 );
		test_manager::GetSingletonPtr()->ReleasePointer( t );
		test_assert( CTextureManagerTestClass::count == 0 );

		test_manager::GetSingletonPtr()->UnloadFile( "0" );
		test_manager::GetSingletonPtr()->UnloadFile( "" );

		// GetFilename
		test_assert( test_manager::GetSingletonPtr()->GetFilename( t ) == "" );
		test_assert( test_manager::GetSingletonPtr()->GetFilename( (CTextureManagerTestClass*)0x0001 ) == "" );

		t = new CTextureManagerTestClass;
		t->i = 1;
		test_assert( CTextureManagerTestClass::count == 1 );
		test_manager::GetSingletonPtr()->AddNew( "1", t, false );
		test_assert( test_manager::GetSingletonPtr()->GetFilename( t ) == "1" );
		test_manager::GetSingletonPtr()->ReleasePointer( t );
		test_assert( CTextureManagerTestClass::count == 0 );

		test_manager::GetSingletonPtr()->SetLogErrors( true );
		test_manager::Delete();
	}

#endif
	return 0;
}

TEST_REGISTER( CTextureManagerTest );

} // end of namespace test
} // end of namespace ceng

#endif