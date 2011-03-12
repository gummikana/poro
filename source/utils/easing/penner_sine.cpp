#include "penner_sine.h"
#include <math.h>

#ifndef PI
#define PI  3.14159265
#endif

namespace ceng {
namespace easing {

Sine::EaseIn Sine::easeIn;
Sine::EaseOut Sine::easeOut;
Sine::EaseInOut Sine::easeInOut;

float Sine::impl_easeIn (float t,float b , float c, float d) {
	return (float)( -c * cos(t/d * (PI/2)) + c + b );
}
float Sine::impl_easeOut(float t,float b , float c, float d) {	
	return (float)( c * sin(t/d * (PI/2)) + b );	
}

float Sine::impl_easeInOut(float t,float b , float c, float d) {
	return (float)( -c/2 * (cos(PI*t/d) - 1) + b );
}

} // end of namespace easing
} // end of namespace ceng