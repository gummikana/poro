#include "penner_circ.h"
// t = x
// d = 1
// b = 0
// c = 1

namespace ceng {
namespace easing {

Circ::EaseIn Circ::easeIn;
Circ::EaseOut Circ::easeOut;
Circ::EaseInOut Circ::easeInOut;

float Circ::impl_easeIn (float t,float b , float c, float d) {
	return -c * (sqrt(1 - (t/=d)*t) - 1) + b;
}
float Circ::impl_easeOut(float t,float b , float c, float d) {
	return c * sqrt(1 - (t=t/d-1)*t) + b;
}

float Circ::impl_easeInOut(float t,float b , float c, float d) {
	if ((t/=d/2) < 1) return -c/2 * (sqrt(1 - t*t) - 1) + b;
	return c/2 * (sqrt(1 - t*(t-=2)) + 1) + b;
}

} // end of namespace easing
} // end of namespace ceng