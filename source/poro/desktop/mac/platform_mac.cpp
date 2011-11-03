/***************************************************************************
 *
 * Copyright (c) 2010 Petri Purho, Dennis Belfrage
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

#include "platform_mac.h"

#include "../../libraries.h"

namespace poro {
    
void PlatformMac::SetWorkingDir(poro::types::string dir){
    static std::string path_str = "";
    const int maxpath = 1024;
    char buffer[maxpath];
        
    if(path_str=="") {
        //Set the default working directory inside the app pacakge for MAC and iPHOHE.
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
        if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)buffer, maxpath))
        {
            std::cerr << "Failed to change to the default working directory!" << std::endl;
            poro_assert(false);
        }
        CFRelease(resourcesURL);
        chdir(buffer);
	}
    
    if(chdir(dir.c_str())==0){
        getcwd(buffer, maxpath);
	    poro_logger << "Changing working dir to " << buffer << std::endl;
	} else {
        getcwd(buffer, maxpath);
	    poro_logger << "Error: Failed to change working dir to " << buffer << std::endl;
	}
	
}
	
} // end o namespace poro