#include "../cstatisticshelper.h"
#include "../../debug.h"

#ifdef CENG_TESTER_ENABLED

namespace ceng {
namespace math {
namespace test {

int CStatisticsHelperTest()
{
	{
		CStatisticsHelper< int > test_sample;
		test_assert( test_sample.GetAverage() == 0 );

		test_sample += 0;
		test_assert( test_sample.GetAverage() == 0 );
		test_assert( test_sample.GetMin() == 0 );
		test_assert( test_sample.GetMax() == 0 );

		test_sample += 10;
		test_assert( test_sample.GetAverage() == 5 );
		test_assert( test_sample.GetMin() == 0 );
		test_assert( test_sample.GetMax() == 10 );

		test_sample += 17;
		test_assert( test_sample.GetAverage() == 9 );
		test_assert( test_sample.GetMin() == 0 );
		test_assert( test_sample.GetMax() == 17 );

		test_sample += 1;
		test_assert( test_sample.GetAverage() == 7 );
		test_assert( test_sample.GetMin() == 0 );
		test_assert( test_sample.GetMax() == 17 );
	}

	{
		CStatisticsHelper< float > test_sample;
		test_assert( test_sample.GetAverage() == 0 );

		for( int i = 0; i < 30; ++i )
		{
			test_sample += 5.f;
		}

		test_float( test_sample.GetAverage() == 5.f );
		test_float( test_sample.GetMin() == 5.0f );
		test_float( test_sample.GetMax() == 5.0f );
	}
	return 0;
}

TEST_REGISTER( CStatisticsHelperTest );

} // end of namespace test
} // end of namespace math
} // end of namespace ceng

#endif
