#ifndef INC_CENG_IEASINGFUNC_H
#define INC_CENG_IEASINGFUNC_H

namespace ceng {
namespace easing {

struct IEasingFunc
{
	virtual ~IEasingFunc() { }

	float operator()( float x ) { return f( x ); }

	virtual float f( float x ) = 0;
};

} // end of namespace_ceng
} // end of namespace ceng


#endif