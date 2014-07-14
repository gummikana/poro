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
//.............................................................................
//
// 23.02.2005 Pete
//		Moved the non-template functions to their own .cpp file for the fucking
//		fact that it seemed to break the linker, if they where defined in the
//		header.
//
// 20.02.2005 Pete
//		Added support for char and unsigned char
//
// 14.02.2005 Pete
//		Added the support for stl containers vector and list. Could have added
//		the support for map if we where not using VC6.0.
//		The XmlSerializeToContainer and the XmlSerializeFromContainer methods
//		where added.
//
// 13.02.2005 Pete
//		Added the XmlSerializeTo() and XmlSerializeFrom() methods to help
//		couping with classes we don't have access to or the ability to go
//		adding methods.
//
// 01.10.2004 Pete
//		Includes some casting functions from CXmlNode to mesh and from mesh
//		to CXmlNode
//
//=============================================================================
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#ifndef INC_CXMLCAST_H
#define INC_CXMLCAST_H

#include <vector>
#include <assert.h>

#include "xml_libraries.h"

#include "cxmlfilesys.h"
#include "cxmlnode.h"
// #include "../logger.h"

//.............................................................................

namespace ceng {



}

#endif
