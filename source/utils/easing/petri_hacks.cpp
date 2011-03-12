#include "petri_hacks.h"
#include <math.h>

#ifndef PI
#define PI  3.14159265
#endif

namespace ceng {
namespace easing {

float PetriHacks::SinGoTo2AndBack::f( float x ) { return sin(x*3.14f)*2.f; }

PetriHacks::SinGoTo2AndBack PetriHacks::sinGoTo2AndBack;



} // end of namespace easing
} // end of namespace ceng