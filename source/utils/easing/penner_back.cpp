#include "penner_back.h"

namespace ceng {
namespace easing {

Back::EaseIn Back::easeIn;
Back::EaseOut Back::easeOut;
Back::EaseInOut Back::easeInOut;

float Back::impl_easeIn (float t,float b , float c, float d) {
	float s = 1.70158f;
	float postFix = t/=d;
	return c*(postFix)*t*((s+1)*t - s) + b;
}
float Back::impl_easeOut(float t,float b , float c, float d) {	
	float s = 1.70158f;
	return c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
}

float Back::impl_easeInOut(float t,float b , float c, float d) {
	float s = 1.70158f;
	if ((t/=d/2) < 1) return c/2*(t*t*(((s*=(1.525f))+1)*t - s)) + b;
	float postFix = t-=2;
	return c/2*((postFix)*t*(((s*=(1.525f))+1)*t + s) + 2) + b;
}

} // end of namespace easing
} // end of namespace ceng