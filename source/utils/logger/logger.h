#ifndef INC_LOGGER_H
#define INC_LOGGER_H

#include "clog.h"

namespace ceng {


#define logger			CLog::GetSingleton()
#define logger_error	CLog::GetSingleton().Error()
#define logger_warning	CLog::GetSingleton().Warning()
#define logger_debug	CLog::GetSingleton().Debug()


} // end of namespace ceng

#endif
