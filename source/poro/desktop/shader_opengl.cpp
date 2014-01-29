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

#include <fstream>
#include <string>

#include "../libraries.h"
#include "../iplatform.h"
#include "shader_opengl.h"
#include "../itexture.h"
#include "../../utils/filesystem/filesystem.h"
//#include "../../utils/safearray/csafearray.h"

namespace poro {

void ShaderOpenGL::Init( const std::string& vertex_source_filename, const std::string& fragment_source_filename )
{
	Release();

    program = glCreateProgram();

	vertexShader = LoadShader( vertex_source_filename, true );
    fragmentShader = LoadShader( fragment_source_filename, false );

	if (vertexShader == 0 || fragmentShader == 0)
	{
		Release();
		return;
	}

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
        fprintf(stderr, "GLSL Linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;

		Release();
    }
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
}

void ShaderOpenGL::Enable()
{
    glUseProgram( program );
	glActiveTexture( GL_TEXTURE0 );
	glEnable( GL_TEXTURE_2D );
}

void ShaderOpenGL::Disable()
{
    glUseProgram( 0 );
	glActiveTexture(GL_TEXTURE0);
}

bool ShaderOpenGL::HasParameter(const std::string& name)
{
	const char *c_str = name.c_str();
    int handle = glGetUniformLocation( program, name.c_str() );
    return handle > -1;
}

void ShaderOpenGL::SetParameter( const std::string& name, float value )
{
	// TODO: cache parameter locations!
	const char *c_str = name.c_str();
	int location = glGetUniformLocation( program, name.c_str() );

	glUniform1f( location, value );
}

void ShaderOpenGL::SetParameter( const std::string& name, ITexture* texture )
{
	// TODO: cache parameter locations!
	const char *c_str = name.c_str();
	int location = glGetUniformLocation( program, name.c_str() );

	glUniform1i( location, 0 );
	glActiveTexture( GL_TEXTURE0 );
	
	TextureOpenGL* texture_gl = (TextureOpenGL*)texture;
	glBindTexture( GL_TEXTURE_2D, texture_gl->mTexture );
}
bool ShaderOpenGL::GetIsCompiledAndLinked()
{
	return isCompiledAndLinked;
}

//===================================================================================

int ShaderOpenGL::LoadShader( const std::string& filename, bool is_vertex_shader )
{
	//ceng::CSafeArray< char, long > source_code;
	//ceng::ReadFileToBuffer( filename, source_code); // end of string contains garbage for some reason if these two lines are used.

	std::ifstream ifile(filename.c_str());
    std::string filetext;

    while( ifile.good() ) {
        std::string line;
        std::getline(ifile, line);
        filetext.append(line + "\n");
    }

	const char* filetext_ptr = filetext.c_str();

	int shader_handle = glCreateShader( is_vertex_shader ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER );
	glShaderSource( shader_handle, 1, &filetext_ptr, NULL );
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

}
