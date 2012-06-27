#include "petri_hacks.h"
#include <math.h>
#include "../random/random.h"

#ifndef PI
#define PI  3.14159265
#endif

namespace ceng {
namespace easing {

float PetriHacks::SinGoTo2AndBack::f( float x ) { return sin(x*3.14f)*2.f; }

// Randomf, just gives you a ceng::math::Randomf(0,1) as a result
float PetriHacks:: Randomf::f( float x ) { return ceng::Randomf( 0.f, 1.f ); }

// BooleanRandom, returns 0 or 1 randomly
float PetriHacks:: BooleanRandom::f( float x ) { return (float)ceng::Random( 0, 1 ); }


PetriHacks::SinGoTo2AndBack		PetriHacks::sinGoTo2AndBack;
PetriHacks::Randomf				PetriHacks::randomf;
PetriHacks::BooleanRandom		PetriHacks::booleanRandom;


} // end of namespace easing
} // end of namespace ceng