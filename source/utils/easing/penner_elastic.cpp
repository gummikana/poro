#include "penner_elastic.h"

#ifndef PI
#define PI  3.14159265
#endif

namespace ceng {
namespace easing {

Elastic::EaseIn Elastic::easeIn;
Elastic::EaseOut Elastic::easeOut;
Elastic::EaseInOut Elastic::easeInOut;

float Elastic::impl_easeIn (float t,float b , float c, float d) {
	if (t==0) return b;  if ((t/=d)==1) return b+c;  
	float p=d*.3f;
	float a=c; 
	float s=p/4;
	float postFix =a*pow(2,10*(t-=1)); // this is a fix, again, with post-increment operators
	return (float) -(postFix * sin((t*d-s)*(2*PI)/p )) + b;
}

float Elastic::impl_easeOut(float t,float b , float c, float d) {
	if (t==0) return b;  if ((t/=d)==1) return b+c;  
	float p=d*.3f;
	float a=c; 
	float s=p/4;
	return (float) (a*pow(2,-10*t) * sin( (t*d-s)*(2*PI)/p ) + c + b);	
}

float Elastic::impl_easeInOut(float t,float b , float c, float d) {
	if (t==0) return b;  if ((t/=d/2)==2) return b+c; 
	float p=d*(.3f*1.5f);
	float a=c; 
	float s=p/4;
	 
	if (t < 1) {
		float postFix =a*pow(2,10*(t-=1)); // postIncrement is evil
		return (float)( -.5f*(postFix* sin( (t*d-s)*(2*PI)/p )) + b );
	} 
	float postFix =  a*pow(2,-10*(t-=1)); // postIncrement is evil
	return (float)( postFix * sin( (t*d-s)*(2*PI)/p )*.5f + c + b );
}

} // end of namespace easing
} // end of namespace ceng