/***************************************************************************
 *
 * Copyright (c) 2010 Petri Purho, Dennis Belfrage, Olli Harjola
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

#ifndef INC_ISHADER_H
#define INC_ISHADER_H


#include <vector>

namespace poro {

class ITexture;
class ITexture3d;

class IShader
{
public:
	virtual ~IShader() { }

	virtual void Init( const std::string& vertex_source_filename, const std::string& fragment_source_filename ) { }	
	virtual void Release() { }
	virtual void Enable() { }
	virtual void Disable() { }
	virtual bool HasParameter( const std::string& name ) = 0;
	virtual void SetParameter( const std::string& name, float value ) { }
	virtual void SetParameter( const std::string& name, const types::vec2& value ) { }
	virtual void SetParameter( const std::string& name, const types::vec3& value ) { }
	virtual void SetParameter( const std::string& name, const ITexture* texture ) { }
	virtual void SetParameter( const std::string& name, const ITexture3d* texture ) { }
	
	virtual bool GetIsCompiledAndLinked()const = 0;

};

} // end o namespace poro

#endif