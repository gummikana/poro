#include "penner_cubic.h"

namespace ceng {
namespace easing {

Cubic::EaseIn Cubic::easeIn;
Cubic::EaseOut Cubic::easeOut;
Cubic::EaseInOut Cubic::easeInOut;

float Cubic::impl_easeIn (float t,float b , float c, float d) {
	return c*(t/=d)*t*t + b;
}
float Cubic::impl_easeOut(float t,float b , float c, float d) {
	return c*((t=t/d-1)*t*t + 1) + b;
}

float Cubic::impl_easeInOut(float t,float b , float c, float d) {
	if ((t/=d/2) < 1) return c/2*t*t*t + b;
	return c/2*((t-=2)*t*t + 2) + b;	
}

} // end of namespace easing
} // end of namespace ceng