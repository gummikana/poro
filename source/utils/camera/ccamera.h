/***************************************************************************
 *
 * Copyright (c) 2003 - 2011 Petri Purho
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ***************************************************************************/


///////////////////////////////////////////////////////////////////////////////
//
// CCamera
// =======
//
// Camera class for pluto. Is used to the conversions from meters to pixels
//
// Created 07.06.2007 by Pete
//=============================================================================
//.............................................................................
#ifndef INC_CCAMERA_H
#define INC_CCAMERA_H

#include "../../types.h"

#include "../singleton/csingleton.h"


namespace ceng {

namespace types {
    using namespace ::types;
}

class CCamera : public ceng::CSingleton< CCamera >
{
public:

	~CCamera();

	struct PhysicsConverter
	{
		PhysicsConverter() { }
		PhysicsConverter( const types::vector2& center, const types::vector2& rate ) : center( center ), rate( rate ) { }

		types::vector2 center;
		types::vector2 rate;
	};

	PhysicsConverter myConverts;

	//----------------------------------------------------------------------------

	types::point ConvertPhysicsRadiusToPixels( const types::vector2& radius ) const
	{
		types::point result;
		result.x = (types::point::unit_type)( radius.x * myConverts.rate.x + 0.5f );
		result.y = (types::point::unit_type)( radius.y * myConverts.rate.y + 0.5f );

		return result;
	}

	types::vector2 ConvertScreenRadiusToPhysics( const types::point& radius ) const
	{
		types::vector2 result;
		result.x = ( (types::vector2::unit_type)radius.x / myConverts.rate.x );
		result.y = ( (types::vector2::unit_type)radius.y / myConverts.rate.y );

		return result;
	}

	types::point ConvertPhysicsToScreen( const types::vector2& p ) const
	{
		types::point result;
		result.x = (types::point::unit_type)( p.x * myConverts.rate.x + 0.5f );
		result.y = (types::point::unit_type)( p.y * myConverts.rate.y + 0.5f );

		result.x += (types::point::unit_type)myConverts.center.x;
		result.y += (types::point::unit_type)myConverts.center.y;

		return result;
	}

	types::vector2 ConvertScreenToPhysics( const types::point& p ) const
	{
		types::vector2 result;
		result.x = p.x - myConverts.center.x;
		result.y = p.y - myConverts.center.y;
		result.x /= myConverts.rate.x;
		result.y /= myConverts.rate.y;

		return	result;
	}

	//-------------------------------------------------------------------------
	/*
	void SetTransformAngle( float angle );
	void SetTransformScale( float scale );
	void SetTransformOffset( const types::vector2& v );

	float GetTransformAngle() const;
	float GetTransformScale() const;
	types::vector2 GetTransformOffset() const;
	*/
	//-------------------------------------------------------------------------

	/*types::vector2 ConvertMousePosition( const types::point& p ) const;*/

private:
	// CCamera() : myConverts( types::vector2( 400.f, 500.f ), types::vector2( 320.f / 10.f, 320.f / 10.f ) ) { }
	CCamera();

	/*
	class CCameraTransformer;
	CCameraTransformer* myTransformer;
	*/

	friend class ceng::CSingleton< CCamera >;
};

} // end of namespace ceng

#endif
