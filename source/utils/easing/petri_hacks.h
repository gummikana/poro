#ifndef INC_EASING_PETRI_HACKS_H
#define INC_EASING_PETRI_HACKS_H


#include <math.h>
#include "ieasingfunc.h"

namespace ceng {
namespace easing {

class PetriHacks {
	
	public:

		// x goes to 2 and then down to 0
		// good for in and out style effects. 
		// Want something to drop and then dissapper... use this baby
		struct SinGoTo2AndBack : public IEasingFunc {
			virtual float f( float x );
		};

		// Randomf, just gives you a ceng::math::Randomf(0,1) as a result
		struct Randomf : public IEasingFunc {
			virtual float f( float x );
		};
 
		// BooleanRandom, returns 0 or 1 randomly
		struct BooleanRandom : public IEasingFunc {
			virtual float f( float x );
		};

		//----------------

		static SinGoTo2AndBack	sinGoTo2AndBack;
		static Randomf			randomf;
		static BooleanRandom	booleanRandom;
};

} // end of namespace easing
} // end of namespace ceng

#endif