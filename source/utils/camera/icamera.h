#ifndef INC_CENG_ICAMERA_H
#define INC_CENG_ICAMERA_H

#include "../rect/cpoint.h"
#include "../rect/crect.h"
#include "../math/cvector2.h"

namespace ceng {

//-----------------------------------------------------------------------------

/*
template< typename Vec2 = math::CVector2< float > >
struct CCameraResult
{
	CCameraResult() : rect(), render_me( true ) { }

	Vec2 rect[4];	
	bool render_me;
};
*/

///////////////////////////////////////////////////////////////////////////////

template< typename TVector2 = math::CVector2< float > >
class ICamera
{
public:
	typedef TVector2 Vec2;

	ICamera() { }
	virtual ~ICamera() { }
	
	virtual bool IsNull() const = 0;

	virtual Vec2 Transform( const Vec2& point ) = 0;

	virtual CPoint< int > ConvertMousePosition( const CPoint< int >& p )
	{
		Vec2 result = Transform( Vec2( (float)p.x, (float)p.y ) );
		return CPoint< int >( (int)result.x, (int)result.y );
	}

	virtual void Update( unsigned int step ) { }

	static ICamera* CreateNull();
};

///////////////////////////////////////////////////////////////////////////////
} // end of namespace ceng

#endif
