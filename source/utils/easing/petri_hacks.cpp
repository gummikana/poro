#include "petri_hacks.h"
#include <math.h>
#include "../random/random.h"

#ifndef PI
#define PI  3.14159265
#endif

namespace ceng {
namespace easing {

float PetriHacks::SinGoTo2AndBack::f( float x ) { return sin(x*3.14f)*2.f; }

float PetriHacks::LittleBackAndForth::f( float x ) { return ((pow(x+0.17f, 1.3f) * sin(1.f/(x+0.17f))) * 1.043f) + 0.035f; }

float PetriHacks::DimishingShakeBack::f( float x ) { return ( ( cos( x * 3.1415f ) + 1 ) * 0.5f )  * sin( x * 30.0f ); }

float PetriHacks::DimishingShake::f( float x ) { return ( ( cos( x * 3.1415f ) + 1 ) * 0.5f )  * sin( x * 30.0f ) + x; }

// Randomf, just gives you a ceng::math::Randomf(0,1) as a result
float PetriHacks:: Randomf::f( float x ) { return ceng::Randomf( 0.f, 1.f ); }

// BooleanRandom, returns 0 or 1 randomly
float PetriHacks:: BooleanRandom::f( float x ) { return (float)ceng::Random( 0, 1 ); }


PetriHacks::SinGoTo2AndBack		PetriHacks::sinGoTo2AndBack;
PetriHacks::DimishingShakeBack	dimishingShakeBack;
PetriHacks::DimishingShakeBack	dimishingShake;
PetriHacks::LittleBackAndForth	littleBackAndForth;
PetriHacks::Randomf				PetriHacks::randomf;
PetriHacks::BooleanRandom		PetriHacks::booleanRandom;


} // end of namespace easing
} // end of namespace ceng