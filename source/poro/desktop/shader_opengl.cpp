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

#include "shader_opengl.h"

#include "../libraries.h"
#include "../itexture.h"
#include "../iplatform.h"
#include "../fileio.h"


namespace poro {

void ShaderOpenGL::Init( const std::string& vertex_source_filename, const std::string& fragment_source_filename )
{
	Release();

	vertexShader = LoadShader( vertex_source_filename, true );
    fragmentShader = LoadShader( fragment_source_filename, false );

	Init();
}

void ShaderOpenGL::InitFromString( const std::string& vertex_source, const std::string& fragment_source )
{ 
    Release();

    vertexShader = LoadShaderFromString( vertex_source, true );
    fragmentShader = LoadShaderFromString( fragment_source, false );

    if (vertexShader == 0 || fragmentShader == 0)
    {
        Release();
        return;
    }

    Init();
}	

void ShaderOpenGL::Release()
{
	isCompiledAndLinked = false;

	if ( program != 0 )
	{
		glDeleteProgram( program );
		program = 0;
	}

	if ( vertexShader != 0 )
	{
		glDeleteShader( vertexShader );
		vertexShader = 0;
	}

	if ( fragmentShader != 0 )
	{
		glDeleteShader( fragmentShader );
		fragmentShader = 0;
	}

	parameterLocationCache.clear();
}

void ShaderOpenGL::Enable()
{
	lastAllocatedTextureUnit = 2;
    glUseProgram( program );
}

void ShaderOpenGL::Disable()
{
	lastAllocatedTextureUnit = 2;
    glUseProgram( 0 );
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_TEXTURE_3D );
}

bool ShaderOpenGL::HasParameter( const std::string& name)
{
	const int location = GetParameterLocation( name.c_str() );
    return location > -1;
}

void ShaderOpenGL::SetParameter( const std::string& name, float value )
{
	const int location = GetParameterLocation( name.c_str() );

	glUniform1f( location, value );
}

void ShaderOpenGL::SetParameter( const std::string& name, const types::vec2& value )
{
	const int location = GetParameterLocation( name.c_str() );

	glUniform2f( location, value.x, value.y );
}

void ShaderOpenGL::SetParameter( const std::string& name, const types::vec3& value )
{
	const int location = GetParameterLocation( name.c_str() );

	glUniform3f( location, value.x, value.y, value.z );
}

void ShaderOpenGL::SetParameter( const std::string& name, const ITexture* texture )
{
	const int location = GetParameterLocation( name.c_str() );

	glEnable( GL_TEXTURE_2D );
	glUniform1i( location, lastAllocatedTextureUnit );
	glActiveTexture( GL_TEXTURE0 + lastAllocatedTextureUnit );
	lastAllocatedTextureUnit ++;

	TextureOpenGL* texture_gl = (TextureOpenGL*)texture;
	glBindTexture( GL_TEXTURE_2D, texture_gl->mTexture );
	glActiveTexture( GL_TEXTURE0 );
}

void ShaderOpenGL::SetParameter( const std::string& name, const ITexture3d* texture )
{
	const int location = GetParameterLocation( name.c_str() );

	glEnable( GL_TEXTURE_3D );
	glUniform1i( location, lastAllocatedTextureUnit );
	glActiveTexture( GL_TEXTURE0 + lastAllocatedTextureUnit );
	lastAllocatedTextureUnit ++;
	
	Texture3dOpenGL* texture_gl = (Texture3dOpenGL*)texture;
	glBindTexture( GL_TEXTURE_3D, texture_gl->mTexture );
	glActiveTexture( GL_TEXTURE0 );
}

bool ShaderOpenGL::GetIsCompiledAndLinked() const
{
	return isCompiledAndLinked;
}

//===================================================================================

int ShaderOpenGL::LoadShader( const std::string& filename, bool is_vertex_shader )
{
	CharBufferAutoFree text;
	StreamStatus::Enum read_status = Poro()->GetFileSystem()->ReadWholeFile( filename, text.memory, &text.size_bytes );
	if ( read_status != StreamStatus::NoError )
	{
		isCompiledAndLinked = false;
		return 0;
	}

	int shader_handle = glCreateShader( is_vertex_shader ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER );
	glShaderSource( shader_handle, 1, (const char**)&text.memory, (int*)&text.size_bytes );
	glCompileShader( shader_handle );

	//DEBUG
	GLint status;
    glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader_handle, infoLogLength, NULL, strInfoLog);
        
        
        fprintf(stderr, "GLSL compile failure:\n%s\n", strInfoLog);
        delete[] strInfoLog;

		isCompiledAndLinked = false;
    }

	return shader_handle;
}

int ShaderOpenGL::LoadShaderFromString( const std::string& source, bool is_vertex_shader )
{
    const char* sources[1];
    sources[0] = source.c_str();

    int lengths[1];
    lengths[0] = source.size();

    int shader_handle = glCreateShader( is_vertex_shader ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER );
    glShaderSource( shader_handle, 1, sources, lengths );
    glCompileShader( shader_handle );

    //DEBUG
    GLint status;
    glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader_handle, infoLogLength, NULL, strInfoLog);


        fprintf(stderr, "GLSL compile failure:\n%s\n", strInfoLog);
        delete[] strInfoLog;

        isCompiledAndLinked = false;
    }

    return shader_handle;
}

void ShaderOpenGL::Init()
{
    program = glCreateProgram();
    glAttachShader( program, vertexShader );
    glAttachShader( program, fragmentShader );
    glLinkProgram( program);

    //DEBUG
    GLint status;
    glGetProgramiv (program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "GLSL link failure: %s\n", strInfoLog);
        delete[] strInfoLog;

        Release();
    }
}

int ShaderOpenGL::GetParameterLocation( const std::string& name )
{
    int result;

    auto it = parameterLocationCache.find( name );
    if ( it == parameterLocationCache.end() )
    {
        result = glGetUniformLocation( program, name.c_str() );
        parameterLocationCache.insert( std::make_pair( name, result ) );
    }
    else
    {
        result = it->second;
    }

    return result;
}

}
