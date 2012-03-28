#include "ccolor.h"

namespace ceng {

bool CColorFloat::masks_initialized = false;
CColorFloat::uint32	CColorFloat::RMask;
CColorFloat::uint32	CColorFloat::GMask;
CColorFloat::uint32	CColorFloat::BMask;
CColorFloat::uint32	CColorFloat::AMask;
					
CColorFloat::uint8 	CColorFloat::RShift;
CColorFloat::uint8 	CColorFloat::GShift;
CColorFloat::uint8 	CColorFloat::BShift;
CColorFloat::uint8 	CColorFloat::AShift;
	
}