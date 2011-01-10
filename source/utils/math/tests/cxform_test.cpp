#include "../cxform.h"
#include "../../debug.h"

#ifdef CENG_TESTER_ENABLED

#include "../math_functions.h"
#include "../../random/random.h"

namespace ceng {
namespace math {
namespace test {


#define test( x ) ceng::logger << "Test: " << #x << " = " << ( x ) << std::endl;

int CXFormTest()
{
	return 0;
}

TEST_REGISTER( CXFormTest );

} // end of namespace test
} // end of namespace math
} // end of namespace ceng

#endif
