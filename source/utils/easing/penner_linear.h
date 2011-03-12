#ifndef INC_EASING_PENNER_LINEAR_H
#define INC_EASING_PENNER_LINEAR_H

#include "ieasingfunc.h"

namespace ceng {
namespace easing {


class Linear 
{

	public:
		
		struct EaseNone : public IEasingFunc {
			virtual float f( float x ) { return impl_easeNone( x, 0, 1, 1 ); }
		};

		struct EaseIn : public IEasingFunc {
			virtual float f( float x ) { return impl_easeIn( x, 0, 1, 1 ); }
		};

		struct EaseOut : public IEasingFunc {
			virtual float f( float x ) { return impl_easeOut( x, 0, 1, 1 ); }
		};

		struct EaseInOut : public IEasingFunc {
			virtual float f( float x ) { return impl_easeInOut( x, 0, 1, 1 ); }
		};

		static EaseNone easeNone;
		static EaseIn easeIn;
		static EaseOut easeOut;
		static EaseInOut easeInOut;

		static float impl_easeNone(float t,float b , float c, float d); // ??
		static float impl_easeIn(float t,float b , float c, float d);
		static float impl_easeOut(float t,float b , float c, float d);
		static float impl_easeInOut(float t,float b , float c, float d);
		
};

} // end of namespace easing
} // end of namespace ceng
#endif