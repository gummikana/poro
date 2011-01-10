#ifndef INC_SIGNALSLOT_LIBS_H
#define INC_SIGNALSLOT_LIBS_H

#define CENG_SERIALIZABLE_SIGNALSLOTS

#include "../ceng_macro.h"
#include "../debug.h"

// #define CENG_Join( X, Y ) CENG_DO_JOIN( X, Y )
// #define CENG_DO_JOIN( X, Y ) CENG_DO_JOIN2(X,Y)
// #define CENG_DO_JOIN2( X, Y ) X##Y

#include <string>
#include <list>
#include <iostream>

#include "../autolist/cautolist.h"
#include "../singleton/csingleton.h"
#include "../maphelper/cmaphelper.h"
#include "../xml/canycontainer.h"

#ifdef CENG_SERIALIZABLE_SIGNALSLOTS
#	include "../xml/cxml.h"
#endif
// #include "cautolist.h"
// #include "../../tester/csingleton.h"
// #include "cmaphelper.h"


#endif
