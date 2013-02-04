#ifndef INC_EASING_PETRI_HACKS_H
#define INC_EASING_PETRI_HACKS_H


#include <math.h>
#include "ieasingfunc.h"

namespace ceng {
namespace easing {

class PetriHacks {
	
	public:

		// Back functions are functions that return to 0,
		// Why? well sometimes you need things to return nicely as well

		// x goes to 2 and then down to 0
		// good for in and out style effects. 
		// Want something to drop and then dissapper... use this baby
		// sin(x*3.14f)*2.f
		struct SinGoTo2AndBack : public IEasingFunc {
			virtual float f( float x );
		};

		// Shakes between 0.9933 and -0.9407  and returns to 0 
		// does like 5 shakes, think of shaking the angle of a tree when player bumps into it
		// ( ( cos( x * 3.1415 ) + 1 ) * 0.5 )  * sin( x * 30.0 )
		struct DimishingShakeBack : public IEasingFunc {
			virtual float f( float x );
		};

		// Shakes between 1.102 and -0.7851 and goes to 1
		// same as DimishingShakeBack but actually ends up at the target
		// does like 5 shakes, don't really know what to use this for, but 
		// I'm sure I'll figure something out
		// ( ( cos( x * 3.1415 ) + 1 ) * 0.5 )  * sin( x * 30.0 ) + x
		struct DimishingShake : public IEasingFunc {
			virtual float f( float x );
		};

		// A function that goes to -0.1 and then to 1.0, like taking a bit of speed before launching
		// ((pow(x+0.17f, 1.3f) * sin(1.f/(x+0.17f))) * 1.043f) + 0.035f
		struct LittleBackAndForth : public IEasingFunc {
			virtual float f( float x );
		};

		// ----- 

		// Randomf, just gives you a ceng::math::Randomf(0,1) as a result
		struct Randomf : public IEasingFunc {
			virtual float f( float x );
		};
 
		// BooleanRandom, returns 0 or 1 randomly
		struct BooleanRandom : public IEasingFunc {
			virtual float f( float x );
		};

		//----------------

		static SinGoTo2AndBack		sinGoTo2AndBack;
		static DimishingShakeBack	dimishingShakeBack;
		static DimishingShakeBack	dimishingShake;
		static LittleBackAndForth	littleBackAndForth;
		static Randomf				randomf;
		static BooleanRandom		booleanRandom;
};

} // end of namespace easing
} // end of namespace ceng

#endif