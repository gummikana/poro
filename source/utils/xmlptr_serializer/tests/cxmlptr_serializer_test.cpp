#include "../../debug.h"
#include "../cxmlptr_serializer.h"

#include <memory>

#ifdef CENG_TESTER_ENABLED


namespace ceng {
namespace test {

namespace {
class CPointerTest
{
public:
	CPointerTest() 
	{ 
		count++;
	}

	~CPointerTest() 
	{ 
		count--;
	}

	void Serialize( ceng::CXmlFileSys* filesys ) 
	{
		XML_BindAttribute( filesys, i );
	}

	int i;

	static int count;
};

int CPointerTest::count = 0;

//------------------------------------


//--------------



//--------------

struct PtrContainingStruct
{
	PtrContainingStruct()
	{
		p1 = new CPointerTest;
		p2 = new CPointerTest;
	}

	void Serialize( ceng::CXmlFileSys* filesys )
	{
		// XML_BindPtr( filesys, p1 );
		// XML_BindPtr( filesys, p2 ); 

		XML_BindPtrAlias( filesys, p1, "p1" );
		XML_BindPtr( filesys, p2 );

		/*
		if( filesys->IsWriting() ) 
		{
			int id = CXmlPtrSerializer_Save( p1 );
			XML_BindAlias( filesys, id, "p1" ); 
		}
		else if( filesys->IsReading() ) 
		{
			if( p1 ) 
			{  
				// should we delete this?
			}
			int id = -1;
			XML_BindAlias( filesys, id, "p1" );
			p1 = CXmlPtrSerializer_Load( p1, id );
		}*/
	}

	CPointerTest* p1;
	CPointerTest* p2;

	
};

//------------------------------------


//------------------------------------

struct SerializerHelper
{
	SerializerHelper( PtrContainingStruct& save_me ) : save_me( save_me ) { }

	void Serialize( ceng::CXmlFileSys* filesys )
	{
		// load the pointers
		XML_LOADPOINTERS_Begin( filesys, CPointerTest );

		XML_BindAlias( filesys, save_me, "PtrContainingStruct" );


		XML_SAVEPOINTERS_End( filesys, CPointerTest );
	}

	PtrContainingStruct& save_me;
};

} // end of anonymous namespace


int CXmlPtrSerializerTest()
{
	// saving part
	{
		std::auto_ptr< CPointerTest > p( new CPointerTest );
		
		// test_assert( p.get() == NULL );
		{
			PtrContainingStruct t;
			t.p1->i = 12;
			t.p2->i = 32;
			std::cout << "Saved.p1: " << t.p1 << std::endl;

			SerializerHelper helper( t );
			ceng::XmlSaveToFile( helper, "temp/cxmlptr_serializer_test1.xml", "Test1" );
		}

		{
			PtrContainingStruct t;
			SerializerHelper helper( t );
			ceng::XmlLoadFromFile( helper, "temp/cxmlptr_serializer_test1.xml", "Test1" );

			test_assert( t.p1 );
			test_assert( t.p1->i == 12 );
			std::cout << "Loaded.p1: " << t.p1 << std::endl;

		}

		
	}

	return 0;
}

TEST_REGISTER( CXmlPtrSerializerTest );

} // end of namespace test
} // end of namespace ceng

#endif