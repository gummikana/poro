#include "ccolor.h"

#include <sdl.h>

namespace ceng {

bool CColorUint8::masks_initialized = false;
CColorUint8::uint32	CColorUint8::RMask;
CColorUint8::uint32	CColorUint8::GMask;
CColorUint8::uint32	CColorUint8::BMask;
CColorUint8::uint32	CColorUint8::AMask;
					
CColorUint8::uint8 	CColorUint8::RShift;
CColorUint8::uint8 	CColorUint8::GShift;
CColorUint8::uint8 	CColorUint8::BShift;
CColorUint8::uint8 	CColorUint8::AShift;

void CColorUint8::InitMasks()
{

	if( masks_initialized == false ) {

#		if SDL_BYTEORDER == SDL_BIG_ENDIAN
		RMask = ( 0xFF000000 );
		GMask = ( 0x00FF0000 );
		BMask = ( 0x0000FF00 ),
		AMask = ( 0x000000FF );
		RShift = ( 24 );
		GShift = ( 16 );
		BShift = ( 8 );
		AShift = ( 0 );
#		else
		RMask = ( 0x000000FF );
		GMask = ( 0x0000FF00 );
		BMask = ( 0x00FF0000 );
		AMask = ( 0xFF000000 );
		RShift = ( 0 );
		GShift = ( 8 );
		BShift = ( 16 );
		AShift = ( 24 );
#		endif

		masks_initialized = true;
	}
}

// ---------------

bool CColorFloat::masks_initialized = false;
CColorFloat::uint32	CColorFloat::RMask;
CColorFloat::uint32	CColorFloat::GMask;
CColorFloat::uint32	CColorFloat::BMask;
CColorFloat::uint32	CColorFloat::AMask;
					
CColorFloat::uint8 	CColorFloat::RShift;
CColorFloat::uint8 	CColorFloat::GShift;
CColorFloat::uint8 	CColorFloat::BShift;
CColorFloat::uint8 	CColorFloat::AShift;

void CColorFloat::InitMasks()
{

	if( masks_initialized == false ) {

#		if SDL_BYTEORDER == SDL_BIG_ENDIAN
		RMask = ( 0xFF000000 );
		GMask = ( 0x00FF0000 );
		BMask = ( 0x0000FF00 ),
		AMask = ( 0x000000FF );
		RShift = ( 24 );
		GShift = ( 16 );
		BShift = ( 8 );
		AShift = ( 0 );
#		else
		RMask = ( 0x000000FF );
		GMask = ( 0x0000FF00 );
		BMask = ( 0x00FF0000 );
		AMask = ( 0xFF000000 );
		RShift = ( 0 );
		GShift = ( 8 );
		BShift = ( 16 );
		AShift = ( 24 );
#		endif

		masks_initialized = true;
	}

}


} // end of namespace ceng