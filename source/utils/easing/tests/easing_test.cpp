#include "../easing.h"
#include "../../debug.h"

#ifdef CENG_TESTER_ENABLED

namespace ceng {
namespace easing {
namespace test {

int Easing_Test()
{
	// linear
	{
		float value = 0;
		value = Linear::easeIn( 0.5f );
		test_assert( value == 0.5f );
	}
	return 0;
}

TEST_REGISTER( Easing_Test );

} // end of namespace test
} // end of namespace easing
} // end of namespace ceng

#endif
