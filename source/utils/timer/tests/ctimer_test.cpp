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


#include "../../debug.h"
#include "../ctimer.h"
#include <math.h>
#include <stdlib.h>

#ifdef PORO_TESTER_ENABLED

#ifdef PORO_PLAT_LINUX
    #define _sleep mssleep
    void mssleep(int ms) { usleep(ms*1000); return; }

#endif

namespace ceng {
namespace test {
namespace types {
	typedef impl::types::ticks ticks;
}

namespace {
	const types::ticks  timer_safty_margin_int = 10;
	const float			timer_safty_margin_float = 0.002f;

	bool CheckInt( types::ticks a, types::ticks b )
	{
		return ( abs( (double)(a - b) ) < timer_safty_margin_int );
	}

	bool CheckFloat( float a, float b )
	{
		return ( abs( (double)(a - b) ) < timer_safty_margin_float );
	}
}

int CTimerTest()
{
	CTimer basetimer;

	{
		test_assert( CheckInt( basetimer.GetTime(), 0 ) );
		test_assert( CheckFloat( basetimer.GetSeconds(), 0 ) );
		test_assert( CheckInt( basetimer.GetDerivate(), 0 ) );
		test_assert( CheckFloat( basetimer.GetDerivateSeconds(), 0 ) );
	}

	{
		types::ticks	time_now_int = basetimer.GetTime();
		float			time_now_float = basetimer.GetSeconds();

		_sleep( 100 );
		basetimer.Pause();

		test_assert( CheckInt( basetimer.GetTime(),					time_now_int + 100 ) );
		test_assert( CheckFloat( basetimer.GetSeconds(),			time_now_float + 0.1f ) );
		test_assert( CheckInt( basetimer.GetDerivate(),				time_now_int + 100 ) );
		test_assert( CheckFloat( basetimer.GetDerivateSeconds(),	time_now_float + 0.1f ) );

		_sleep( 100 );

		test_assert( CheckInt( basetimer.GetTime(),					time_now_int + 100 ) );
		test_assert( CheckFloat( basetimer.GetSeconds(),			time_now_float + 0.1f ) );
		test_assert( CheckInt( basetimer.GetDerivate(),				time_now_int + 100 ) );
		test_assert( CheckFloat( basetimer.GetDerivateSeconds(),	time_now_float + 0.1f ) );


	}

	{
		CTimer test;

		test.SetTime( 10 );
		test_assert( CheckInt( test.GetTime(), 10 ) );

		test.SetTime( 100 );
		test.Resume();
		test_assert( CheckInt( test.GetTime(), 100 ) );

		test.SetTime( 120 );
		test.Pause();
		test_assert( CheckInt( test.GetTime(), 120 ) );
		test.Resume();
		test_assert( CheckInt( test.GetTime(), 120 ) );
		test.Pause();
		test_assert( CheckInt( test.GetTime(), 120 ) );
		test.Resume();
		test_assert( CheckInt( test.GetTime(), 120 ) );

		test -= 50;

		test_assert( CheckInt( test.GetTime(), 70 ) );


	}

	return 0;
}

// TEST_REGISTER( CTimerTest );

} // end of namespace test
} // end of namespace ceng


#endif
