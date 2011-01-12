///////////////////////////////////////////////////////////////////////////////
//
// ILogListener
// ==========
//
//.............................................................................
//
//	04.08.2005 Pete
//		Refactored the CLogListener to ILogListner and changed the Write()
//		to WriteLine with proper line_level
//
//=============================================================================
#ifndef INC_ILOGLISTENER_H
#define INC_ILOGLISTENER_H

#include <string>

namespace ceng {

//! Interface class created for helping the badgers at CLog
class ILogListener
{
public:
	ILogListener() { }
	virtual ~ILogListener() { }

	virtual void WriteLine( const std::string& line, int line_level ) = 0;
};

} // end of namespace ceng

#endif
