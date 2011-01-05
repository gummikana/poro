#ifndef INC_CENG_CCAMERANULL_H
#define INC_CENG_CCAMERANULL_H

#include "icamera.h"

namespace ceng {

template< typename Vec2 = math::CVector2< float > >
class CCameraNull : public ICamera< Vec2 >
{
public:
	CCameraNull() : ICamera< Vec2 >() { }
	~CCameraNull() { }

	bool IsNull() const { return true; }

	virtual Vec2 Transform( const Vec2& point )
	{
		return point;
	}


};

} // end of namespace ceng

#endif
