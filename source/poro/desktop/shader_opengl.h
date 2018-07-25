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

#ifndef INC_SHADER_OPENGL_H
#define INC_SHADER_OPENGL_H

#include <map>
#include "../poro_types.h"
#include "graphics_opengl.h"
#include "texture_opengl.h"
#include "texture3d_opengl.h"
#include "../ishader.h"

namespace poro {
	
class ShaderOpenGL :  public IShader
{
public:
	ShaderOpenGL() : IShader(), 
		isCompiledAndLinked(false), 
		program(0), 
		vertexShader(0), 
		fragmentShader(0),
		lastAllocatedTextureUnit(2) 
	{

	}
	virtual ~ShaderOpenGL(){ Release(); }

	virtual void Init( const std::string& vertex_source_filename, const std::string& fragment_source_filename ) PORO_OVERRIDE;	
    virtual void InitFromString( const std::string& vertex_source, const std::string& fragment_source ) PORO_OVERRIDE;
	virtual void Release() PORO_OVERRIDE;
	virtual void Enable() PORO_OVERRIDE;
	virtual void Disable() PORO_OVERRIDE;
	virtual bool HasParameter( const std::string& name ) PORO_OVERRIDE;
	virtual void SetParameter( const std::string& name, float value ) PORO_OVERRIDE;
	virtual void SetParameter( const std::string& name, const types::vec2& value ) PORO_OVERRIDE;
	virtual void SetParameter( const std::string& name, const types::vec3& value ) PORO_OVERRIDE;
	virtual void SetParameter( const std::string& name, const types::vec2& value_xy, types::vec2& value_zw ) PORO_OVERRIDE;
	virtual void SetParameter( const std::string& name, float x, float y, float z, float w ) PORO_OVERRIDE;
	virtual void SetParameter( const std::string& name, const ITexture* texture ) PORO_OVERRIDE;
	virtual void SetParameter( const std::string& name, const ITexture3d* texture ) PORO_OVERRIDE;
	virtual bool GetIsCompiledAndLinked() const PORO_OVERRIDE;

private:
    bool isCompiledAndLinked;
    int program;
    int vertexShader;
    int fragmentShader;
	int lastAllocatedTextureUnit;
	std::map<std::string, int> parameterLocationCache;
	
	int LoadShader( const std::string& filename, bool is_vertex_shader );
	int LoadShaderFromString( const char* source, const int source_length, bool is_vertex_shader );
    void Init();
	int GetParameterLocation( const std::string& name );

    //const int MAX_PARAMETER_COUNT = 100;
    /*int lastSetParameterId = -1;
    int[] parameterHandles = new int[MAX_PARAMETER_COUNT];
    int[] bindedTextures = new int[MAX_PARAMETER_COUNT];
    Dictionary<int, int> allocatedTextureUnits;
    int nextAllocatedTextureUnit;*/
};
	
} // end o namespace poro

#endif