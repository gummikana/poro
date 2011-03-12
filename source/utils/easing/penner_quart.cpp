#include "penner_quart.h"
// t = x
// d = 1
// b = 0
// c = 1
namespace ceng {
namespace easing {

Quart::EaseIn Quart::easeIn;
Quart::EaseOut Quart::easeOut;
Quart::EaseInOut Quart::easeInOut;

float Quart::impl_easeIn (float t,float b , float c, float d) {
	return c*(t/=d)*t*t*t + b;
}
float Quart::impl_easeOut(float t,float b , float c, float d) {
	return -c * ((t=t/d-1)*t*t*t - 1) + b;
}

float Quart::impl_easeInOut(float t,float b , float c, float d) {
	if ((t/=d/2) < 1) return c/2*t*t*t*t + b;
	return -c/2 * ((t-=2)*t*t*t - 2) + b;
}

} // end of namespace easing
} // end of namespace ceng