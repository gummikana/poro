#include "../vector.h"
#include "../../../utils/debug.h"

#ifdef PORO_TESTER_ENABLED

namespace as {
namespace test {

int VectorTest()
{
	Vector< int > test_case;

	test_assert( test_case.length() == 0 );
	test_case.push( 10 );
	test_assert( test_case.length() == 1 );

	int value = test_case.pop();
	test_assert( value == 10 );
	test_assert( test_case.length() == 0 );




	return 0;
}

TEST_REGISTER( VectorTest );

} // end of namespace test
} // end of namespace as
#endif
