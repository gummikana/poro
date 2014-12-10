#include "easing.h"

#include <string>
#include <math.h>
#include "../random/random.h"

#ifndef PI
#define PI  3.14159265
#endif

namespace ceng {
namespace easing {

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

Bounce::EaseIn Bounce::easeIn;
Bounce::EaseOut Bounce::easeOut;
Bounce::EaseInOut Bounce::easeInOut;

float Bounce::impl_easeIn (float t,float b , float c, float d) {
	return c - impl_easeOut (d-t, 0, c, d) + b;
}
float Bounce::impl_easeOut(float t,float b , float c, float d) {
	if ((t/=d) < (1/2.75f)) {
		return c*(7.5625f*t*t) + b;
	} else if (t < (2/2.75f)) {
		float postFix = t-=(1.5f/2.75f);
		return c*(7.5625f*(postFix)*t + .75f) + b;
	} else if (t < (2.5/2.75)) {
			float postFix = t-=(2.25f/2.75f);
		return c*(7.5625f*(postFix)*t + .9375f) + b;
	} else {
		float postFix = t-=(2.625f/2.75f);
		return c*(7.5625f*(postFix)*t + .984375f) + b;
	}
}

float Bounce::impl_easeInOut(float t,float b , float c, float d) {
	if (t < d/2) return impl_easeIn (t*2, 0, c, d) * .5f + b;
	else return impl_easeOut (t*2-d, 0, c, d) * .5f + c*.5f + b;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

Elastic::EaseIn Elastic::easeIn;
Elastic::EaseOut Elastic::easeOut;
Elastic::EaseInOut Elastic::easeInOut;

float Elastic::impl_easeIn (float t,float b , float c, float d) {
	if (t==0) return b;  if ((t/=d)==1) return b+c;  
	float p=d*.3f;
	float a=c; 
	float s=p/4;
	float postFix =a*powf(2,10*(t-=1)); // this is a fix, again, with post-increment operators
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
		float postFix =a*powf(2,10*(t-=1)); // postIncrement is evil
		return (float)( -.5f*(postFix* sin( (t*d-s)*(2*PI)/p )) + b );
	} 
	float postFix =  a*powf(2,-10*(t-=1)); // postIncrement is evil
	return (float)( postFix * sin( (t*d-s)*(2*PI)/p )*.5f + c + b );
}

//-----------------------------------------------------------------------------

Expo::EaseIn Expo::easeIn;
Expo::EaseOut Expo::easeOut;
Expo::EaseInOut Expo::easeInOut;

float Expo::impl_easeIn (float t,float b , float c, float d) {
	return (t==0) ? b : c * powf(2, 10 * (t/d - 1)) + b;
}
float Expo::impl_easeOut(float t,float b , float c, float d) {
	return (t==d) ? b+c : c * (-powf(2, -10 * t/d) + 1) + b;	
}

float Expo::impl_easeInOut(float t,float b , float c, float d) {
	if (t==0) return b;
	if (t==d) return b+c;
	if ((t/=d/2) < 1) return c/2 * powf(2, 10 * (t - 1)) + b;
	return c/2 * (-powf(2, -10 * --t) + 2) + b;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

Quad::EaseIn Quad::easeIn;
Quad::EaseOut Quad::easeOut;
Quad::EaseInOut Quad::easeInOut;


float Quad::impl_easeIn (float t,float b , float c, float d) {
	return c*(t/=d)*t + b;
}
float Quad::impl_easeOut(float t,float b , float c, float d) {
	return -c *(t/=d)*(t-2) + b;
}

float Quad::impl_easeInOut(float t,float b , float c, float d) {
	if ((t/=d/2) < 1) return ((c/2)*(t*t)) + b;
	return -c/2 * (((t-2)*(--t)) - 1) + b;
	/*
	originally return -c/2 * (((t-2)*(--t)) - 1) + b;
	
	I've had to swap (--t)*(t-2) due to diffence in behaviour in 
	pre-increment operators between java and c++, after hours
	of joy
	*/
	
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

Quint::EaseIn Quint::easeIn;
Quint::EaseOut Quint::easeOut;
Quint::EaseInOut Quint::easeInOut;

float Quint::impl_easeIn (float t,float b , float c, float d) {
	return c*(t/=d)*t*t*t*t + b;
}
float Quint::impl_easeOut(float t,float b , float c, float d) {
	return c*((t=t/d-1)*t*t*t*t + 1) + b;
}

float Quint::impl_easeInOut(float t,float b , float c, float d) {
	if ((t/=d/2) < 1) return c/2*t*t*t*t*t + b;
	return c/2*((t-=2)*t*t*t*t + 2) + b;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

float PetriHacks::SinGoTo2AndBack::f( float x ) { return sin(x*3.141596f)*2.f; }

float PetriHacks::LittleBackAndForth::f( float x ) { return ((pow(x+0.17f, 1.3f) * sin(1.f/(x+0.17f))) * 1.043f) + 0.035f; }

float PetriHacks::DimishingShakeBack::f( float x ) { return ( ( cos( x * 3.1415f ) + 1 ) * 0.5f )  * sin( x * 30.0f ); }

float PetriHacks::DimishingShake::f( float x ) { return ( ( cos( x * 3.1415f ) + 1 ) * 0.5f )  * sin( x * 30.0f ) + x; }

// Randomf, just gives you a ceng::math::Randomf(0,1) as a result
float PetriHacks:: Randomf::f( float x ) { return ceng::Randomf( 0.f, 1.f ); }

// BooleanRandom, returns 0 or 1 randomly
float PetriHacks:: BooleanRandom::f( float x ) { return (float)ceng::Random( 0, 1 ); }


PetriHacks::SinGoTo2AndBack		PetriHacks::sinGoTo2AndBack;
PetriHacks::DimishingShakeBack	PetriHacks::dimishingShakeBack;
PetriHacks::DimishingShakeBack	PetriHacks::dimishingShake;
PetriHacks::LittleBackAndForth	PetriHacks::littleBackAndForth;
PetriHacks::Randomf				PetriHacks::randomf;
PetriHacks::BooleanRandom		PetriHacks::booleanRandom;

//-----------------------------------------------------------------------------


IEasingFunc& GetEasing( const char* ename )
{
	std::string easing_name = ename;

	#define STR_RETURN_EASING( x, y ) if( easing_name == #x "::" #y ) return x::y;

	STR_RETURN_EASING( Back, easeIn );
	STR_RETURN_EASING( Back, easeOut );
	STR_RETURN_EASING( Back, easeInOut );

	STR_RETURN_EASING( Bounce, easeIn );
	STR_RETURN_EASING( Bounce, easeOut );
	STR_RETURN_EASING( Bounce, easeInOut );

	STR_RETURN_EASING( Circ, easeIn );
	STR_RETURN_EASING( Circ, easeOut );
	STR_RETURN_EASING( Circ, easeInOut );

	STR_RETURN_EASING( Cubic, easeIn );
	STR_RETURN_EASING( Cubic, easeOut );
	STR_RETURN_EASING( Cubic, easeInOut );

	STR_RETURN_EASING( Elastic, easeIn );
	STR_RETURN_EASING( Elastic, easeOut );
	STR_RETURN_EASING( Elastic, easeInOut );

	STR_RETURN_EASING( Expo, easeIn );
	STR_RETURN_EASING( Expo, easeOut );
	STR_RETURN_EASING( Expo, easeInOut );

	STR_RETURN_EASING( Linear, easeIn );
	STR_RETURN_EASING( Linear, easeOut );
	STR_RETURN_EASING( Linear, easeInOut );

	STR_RETURN_EASING( Quad, easeIn );
	STR_RETURN_EASING( Quad, easeOut );
	STR_RETURN_EASING( Quad, easeInOut );

	STR_RETURN_EASING( Quart, easeIn );
	STR_RETURN_EASING( Quart, easeOut );
	STR_RETURN_EASING( Quart, easeInOut );

	STR_RETURN_EASING( Quint, easeIn );
	STR_RETURN_EASING( Quint, easeOut );
	STR_RETURN_EASING( Quint, easeInOut );

	STR_RETURN_EASING( Sine, easeIn );
	STR_RETURN_EASING( Sine, easeOut );
	STR_RETURN_EASING( Sine, easeInOut );

	STR_RETURN_EASING( PetriHacks, sinGoTo2AndBack );
	STR_RETURN_EASING( PetriHacks, dimishingShakeBack );
	STR_RETURN_EASING( PetriHacks, dimishingShake );
	STR_RETURN_EASING( PetriHacks, littleBackAndForth );
	STR_RETURN_EASING( PetriHacks, randomf );
	STR_RETURN_EASING( PetriHacks, booleanRandom );

	#undef STR_RETURN_EASING

	return Linear::easeIn;
}

//-----------------------------------------------------------------------------

} // end of namespace easing
} // end of namespace ceng
