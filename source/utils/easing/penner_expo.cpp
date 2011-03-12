#include "penner_expo.h"

namespace ceng {
namespace easing {

Expo::EaseIn Expo::easeIn;
Expo::EaseOut Expo::easeOut;
Expo::EaseInOut Expo::easeInOut;

float Expo::impl_easeIn (float t,float b , float c, float d) {
	return (t==0) ? b : c * pow(2, 10 * (t/d - 1)) + b;
}
float Expo::impl_easeOut(float t,float b , float c, float d) {
	return (t==d) ? b+c : c * (-pow(2, -10 * t/d) + 1) + b;	
}

float Expo::impl_easeInOut(float t,float b , float c, float d) {
	if (t==0) return b;
	if (t==d) return b+c;
	if ((t/=d/2) < 1) return c/2 * pow(2, 10 * (t - 1)) + b;
	return c/2 * (-pow(2, -10 * --t) + 2) + b;
}

} // end of namespace easing
} // end of namespace ceng