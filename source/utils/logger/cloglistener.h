#ifndef INC_CLOGLISTENER_H
#define INC_CLOGLISTENER_H

#include <string>

namespace ceng {

//! Abstract class created for helping the badgers at CLogBridge
class CLogListener
{
public:
	CLogListener();
	virtual ~CLogListener() { }

	virtual void Write( const std::string& line ) = 0;
};

}

#endif