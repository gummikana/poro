#include "penner_linear.h"
// t = x
// d = 1
// b = 0
// c = 1


namespace ceng {
namespace easing {

Linear::EaseNone Linear::easeNone;
Linear::EaseIn Linear::easeIn;
Linear::EaseOut Linear::easeOut;
Linear::EaseInOut Linear::easeInOut;


float Linear::impl_easeNone (float t,float b , float c, float d) {
	return c*t/d + b;
}
float Linear::impl_easeIn (float t,float b , float c, float d) {
	return c*t/d + b;
}
float Linear::impl_easeOut(float t,float b , float c, float d) {	
	return c*t/d + b;
}

float Linear::impl_easeInOut(float t,float b , float c, float d) {
	return c*t/d + b;
}

} // end of namespace easing
} // end of namespace ceng