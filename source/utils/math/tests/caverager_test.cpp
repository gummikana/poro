#include "../caverager.h"
#include "../../debug.h"

#ifdef CENG_TESTER_ENABLED

namespace ceng {
namespace math {
namespace test {

int CAveragerTest()
{
	{
		CAverager< int > test_sample;
		test_assert( test_sample.GetAverage() == 0 );

		test_sample += 0;
		test_assert( test_sample.GetAverage() == 0 );

		test_sample += 10;
		test_assert( test_sample.GetAverage() == 5 );

		test_sample += 17;
		test_assert( test_sample.GetAverage() == 9 );

	}

	{
		CAverager< float > test_sample;
		test_assert( test_sample.GetAverage() == 0 );

		for( int i = 0; i < 30; ++i )
		{
			test_sample += 5.f;
		}

		test_float( test_sample.GetAverage() == 5.f );
	}
	return 0;
}

TEST_REGISTER( CAveragerTest );

} // end of namespace test
} // end of namespace math
} // end of namespace ceng

#endif
