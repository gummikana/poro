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
// CCrayonLevelList
// =================
//
// A Level list for crayon.
//
// 
// Created 10.05.2006 by Pete
//=============================================================================
//.............................................................................
#ifndef INC_CLEVELLIST_H
#define INC_CLEVELLIST_H

#include <vector>
#include <string>

#include "../xml/cxml.h"
#include "../singleton/csingleton.h"

namespace ceng {

class CLevelList : public CSingleton< CLevelList >
{
public:

	class CLevelFile
	{
	public:
		CLevelFile()
		{
			filename = "";
			completed = false;
		}

		CLevelFile( const std::string& filename, bool completed ) :
			filename( filename ),
			completed( completed )
		{
		}
		
		CLevelFile( const CLevelFile& other ) :
			filename( other.filename ),
			completed( other.completed )
		{

		}

		const CLevelFile& operator= ( const CLevelFile& other )
		{
			filename = other.filename;
			completed = other.completed;

			return *this;
		}


		void Serialize( ceng::CXmlFileSys* filesys )
		{
			XML_BindAttribute( filesys, filename );
			XML_BindAttribute( filesys, completed );
		}

		std::string filename;
		bool		completed;
	};

	~CLevelList() { }

	int			GetCurrentLevel() const;
	void		SetCurrentLevel( int i ) { myCurrentLevel = i; }
	std::string GetLevelFile( int i ) const;

	// returns the name for the next level and sets this
	// level to complete
	std::string GetNextLevel();

	void Serialize( ceng::CXmlFileSys* filesys );
		
private:

	CLevelList() { myCurrentLevel = 0; }

	std::vector< CLevelFile >	myLevels;
	int							myCurrentLevel;
	int							myJumpTo;
	
	friend class CSingleton< CLevelList >;
};


} // end of ceng

#endif