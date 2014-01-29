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

#include "../libraries.h"
#include "../iplatform.h"
#include "graphics_buffer_opengl.h"
#include "texture_opengl.h"

namespace poro {
namespace {
Uint32 GetNextPowerOfTwo(Uint32 input)
{
	--input;
	input |= input >> 16;
	input |= input >> 8;
	input |= input >> 4;
	input |= input >> 2;
	input |= input >> 1;
	return input + 1;
}
} // end of anonymous namespace

    public class OpenGL_GLSL_LinkedShader : ILinkedShader
    {
        const int MAX_PARAMETER_COUNT = 100;


        bool compiledSuccesfully = true;
        bool isInitialized;

        int programHandle;

        string name;
        internal int vertShader;
        internal int fragShader;

        int lastSetParameterId = -1;
        int[] parameterHandles = new int[MAX_PARAMETER_COUNT];
        int[] bindedTextures = new int[MAX_PARAMETER_COUNT];
        float[] bindedFloats = new float[MAX_PARAMETER_COUNT];
        Vector2D[] bindedVectors2D = new Vector2D[MAX_PARAMETER_COUNT];
        Vector3D[] bindedVectors3D = new Vector3D[MAX_PARAMETER_COUNT];
        ColorF[] bindedColors = new ColorF[MAX_PARAMETER_COUNT];

        Dictionary<int, int> allocatedTextureUnits;
        int nextAllocatedTextureUnit;


        internal OpenGL_GLSL_LinkedShader(OpenGL_GLSL_VertexShader vertShader, OpenGL_GLSL_FragmentShader fragShader) : this(vertShader, fragShader, null)
        {

        }

        internal OpenGL_GLSL_LinkedShader(OpenGL_GLSL_VertexShader vertShader, OpenGL_GLSL_FragmentShader fragShader, string metadata)
        {
            programHandle = GL.CreateProgram();

            this.vertShader = vertShader.ShaderHandle;
            this.fragShader = fragShader.ShaderHandle;

            GL.AttachShader(programHandle, this.vertShader);
            GL.AttachShader(programHandle, this.fragShader);

            GL.LinkProgram(programHandle);

            name = vertShader.Name + " - " + fragShader.Name + " - " + Matf.Random.Next();
            Metadata = metadata;

            for (int i = 0; i < MAX_PARAMETER_COUNT; i++)
                parameterHandles[i] = -1;

            //DEBUG
            int debugLinkStatus;
            GL.GetProgram(programHandle, ProgramParameter.LinkStatus, out debugLinkStatus);

            string infoLog = GL.GetProgramInfoLog(programHandle);
            if (debugLinkStatus < 1)
                Log.Write("Error linking shader '" + name + "':\n" + infoLog, DebugLevel.Warning);
        }


        public string Metadata;


        /// <summary>
        /// Finds out if a shader does have a certain parameter available.
        /// </summary>
        public bool HasParameter(string parameter)
        {
            int handle = GL.GetUniformLocation(programHandle, parameter);
            return handle != 0;
        }

        /// <summary>
        /// Gets pointer to a shader parameter and stores it to internal array at index.
        /// Index must be in the range 0 to 99.
        /// </summary>
        public void StoreParameterHandle(uint index, string parameter)
        {
            int handle = GL.GetUniformLocation(programHandle, parameter);

            parameterHandles[index] = handle;
            if (handle < 0)
                Log.Write("Shader " + name + " doesn't have parameter " + parameter + ".", DebugLevel.Warning);
            else
                Log.Write("Shader " + name + " has parameter '" + parameter + "' at " + handle + " - Stored to index " + index);

            ShaderManager.Core.CheckForErrors(name, "StoreParameterHandle " + index.ToString());
            lastSetParameterId = (int)index;
        }

        /// <summary>
        /// Sets internal parameter (which is stored to index) to texture.
        /// Index must be in the range 0 to 99.
        /// </summary>
        public void SetParameter(int index, ITexture texture)
        {
            int parameterHandle = parameterHandles[index];
            if (parameterHandle < 0)
                return;


            if (allocatedTextureUnits == null)
                allocatedTextureUnits = new Dictionary<int, int>();

            int allocatedTextureUnit;
            bool found = allocatedTextureUnits.TryGetValue(parameterHandle, out allocatedTextureUnit);

            if (!found)
            {
                //ALLOCATE A TEXTURE UNIT FOR THIS PARAMETER
                allocatedTextureUnit = nextAllocatedTextureUnit;
                nextAllocatedTextureUnit++;
                allocatedTextureUnits.Add(parameterHandle, allocatedTextureUnit);
            }

            GL_StateMachine.Texture2DEnabled = true;
            GL.Uniform1(parameterHandle, allocatedTextureUnit);
#if DEBUGSHADERS && DEBUG
            ShaderManager.Core.CheckForErrors(name, "SetParameter(ITexture) GL.Uniform1 " + index.ToString());
#endif

            var texUnit = (TextureUnit)(allocatedTextureUnit + 33984);
            GL.ActiveTexture(texUnit);
#if DEBUGSHADERS && DEBUG
            ShaderManager.Core.CheckForErrors(name, "SetParameter(ITexture) GL.ActiveTexture " + index.ToString());
#endif

            GL.BindTexture(TextureTarget.Texture2D, texture.GLTextureHandle);
            GL_StateMachine._texture = texture.GLTextureHandle;
#if DEBUGSHADERS && DEBUG
            ShaderManager.Core.CheckForErrors(name, "SetParameter(ITexture) GL_StateMachine.BindTexture " + index.ToString());
#endif
            GL.ActiveTexture(TextureUnit.Texture0);
        }

        /// <summary>
        /// Sets internal parameter (which is stored to index) to texture.
        /// Index must be in the range 0 to 99.
        /// </summary>
        public void SetParameter(int index, ITextureDepth texture)
        {
            int parameterHandle = parameterHandles[index];
            if (parameterHandle < 0)
                return;

            int textureHandle = texture.GLTextureHandle;

            if (allocatedTextureUnits == null)
                allocatedTextureUnits = new Dictionary<int, int>();

            int allocatedTextureUnit;
            bool found = allocatedTextureUnits.TryGetValue(parameterHandle, out allocatedTextureUnit);

            if (!found)
            {
                //ALLOCATE A TEXTURE UNIT FOR THIS PARAMETER
                allocatedTextureUnit = nextAllocatedTextureUnit;
                nextAllocatedTextureUnit++;
                allocatedTextureUnits.Add(parameterHandle, allocatedTextureUnit);
            }
            
            GL_StateMachine.Texture2DEnabled = true;
            GL.Uniform1(parameterHandle, allocatedTextureUnit);
#if DEBUGSHADERS && DEBUG
            ShaderManager.Core.CheckForErrors(name, "SetParameter(ITexture) GL.Uniform1 " + index.ToString());
#endif

            var texUnit = (TextureUnit)(allocatedTextureUnit + 33984);
            GL.ActiveTexture(texUnit);
#if DEBUGSHADERS && DEBUG
            ShaderManager.Core.CheckForErrors(name, "SetParameter(ITexture) GL.ActiveTexture " + index.ToString());
#endif

            GL_StateMachine.BindTexture(textureHandle);
#if DEBUGSHADERS && DEBUG
            ShaderManager.Core.CheckForErrors(name, "SetParameter(ITexture) GL_StateMachine.BindTexture " + index.ToString());
#endif
            GL.ActiveTexture(TextureUnit.Texture0);
        }

        /// <summary>
        /// Sets internal parameter (which is stored to index) to Color.
        /// Index must be in the range 0 to 99.
        /// </summary>
        public void SetParameter(int index, ref ColorF color)
        {
            var parameterHandle = parameterHandles[index];
            if (parameterHandle < 0)
                return;

            if (!bindedColors[index].Equals(color))
            {
                GL.Uniform4(parameterHandle, color.R, color.G, color.B, color.A);
                bindedColors[index] = color;
            }
#if DEBUGSHADERS && DEBUG
            ShaderManager.Core.CheckForErrors(name, "SetParameter " + index.ToString());
#endif
        }

        /// <summary>
        /// Sets internal parameter (for which a handle is stored to index) to a value.
        /// Index must be in the range 0 to 99.
        /// </summary>
        public void SetParameter(int index, float value)
        {
            var parameterHandle = parameterHandles[index];
            if (parameterHandle < 0)
                return;

            if (bindedFloats[index] != value)
            {
                GL.Uniform1(parameterHandle, value);
                bindedFloats[index] = value;
            }
#if DEBUGSHADERS && DEBUG
            ShaderManager.Core.CheckForErrors(name, "SetParameter " + index.ToString());
#endif
        }

        /// <summary>
        /// Sets internal parameter (for which a handle is stored to index) to a value.
        /// Index must be in the range 0 to 99.
        /// </summary>
        public void SetParameter(int index, ref Vector2D value)
        {
            var parameterHandle = parameterHandles[index];
            if (parameterHandle < 0)
                return;

            if (bindedVectors2D[index] != value)
            {
                GL.Uniform2(parameterHandle, value.X, value.Y);
                bindedVectors2D[index] = value;
            }
#if DEBUGSHADERS && DEBUG
            ShaderManager.Core.CheckForErrors(name, "SetParameter " + index.ToString());
#endif
        }

        /// <summary>
        /// Sets internal parameter (for which a handle is stored to index) to a value.
        /// Index must be in the range 0 to 99.
        /// </summary>
        public void SetParameter(int index, float x, float y)
        {
            var parameterHandle = parameterHandles[index];
            if (parameterHandle < 0)
                return;

            var value = new Vector2D(x, y);
            if (bindedVectors2D[index] != value)
            {
                GL.Uniform2(parameterHandle, x, y);
                bindedVectors2D[index] = value;
            }
#if DEBUGSHADERS && DEBUG
            ShaderManager.Core.CheckForErrors(name, "SetParameter " + index.ToString());
#endif
        }

        /// <summary>
        /// Sets internal parameter (for which a handle is stored to index) to a value.
        /// Index must be in the range 0 to 99.
        /// </summary>
        public void SetParameter(int index, ref Vector3D value)
        {
            var parameterHandle = parameterHandles[index];
            if (parameterHandle < 0)
                return;

            if (bindedVectors3D[index] != value)
            {
                GL.Uniform3(parameterHandle, value.X, value.Y, value.Z);
                bindedVectors3D[index] = value;
            }
#if DEBUGSHADERS && DEBUG
            ShaderManager.Core.CheckForErrors(name, "SetParameter " + index.ToString());
#endif
        }

        /// <summary>
        /// Sets internal parameter (for which a handle is stored to index) to a value.
        /// Index must be in the range 0 to 99.
        /// </summary>
        public void SetParameter(int index, float x, float y, float z)
        {
            var parameterHandle = parameterHandles[index];
            if (parameterHandle < 0)
                return;

            var value = new Vector3D(x, y, z);
            if (bindedVectors3D[index] != value)
            {
                GL.Uniform3(parameterHandle, x, y, z);
                bindedVectors3D[index] = value;
            }
#if DEBUGSHADERS && DEBUG
            ShaderManager.Core.CheckForErrors(name, "SetParameter " + index.ToString());
#endif
        }

        /// <summary>
        /// Sets internal parameter (for which a handle is stored to index) to a value.
        /// Index must be in the range 0 to 99.
        /// </summary>
        public void SetParameter(int index, ref Vector4D value)
        {
            var parameterHandle = parameterHandles[index];
            if (parameterHandle < 0)
                return;

            GL.Uniform4(parameterHandle, value.X, value.Y, value.Z, value.W);
#if DEBUGSHADERS && DEBUG
            ShaderManager.Core.CheckForErrors(name, "SetParameter " + index.ToString());
#endif
        }

        /// <summary>
        /// Sets internal parameter (for which a handle is stored to index) to a value.
        /// Index must be in the range 0 to 99.
        /// </summary>
        public void SetParameter(int index, float x, float y, float z, float w)
        {
            var parameterHandle = parameterHandles[index];
            if (parameterHandle < 0)
                return;

            GL.Uniform4(parameterHandle, x, y, z, w);
#if DEBUGSHADERS && DEBUG
            ShaderManager.Core.CheckForErrors(name, "SetParameter " + index.ToString());
#endif
        }

        /// <summary>
        /// Deletes the shader object from the memory.
        /// </summary>
        public void Dispose()
        {
            if (programHandle != 0)
                GL.DeleteProgram(programHandle);

            programHandle = 0;
            GC.SuppressFinalize(this);
        }


        public override string ToString()
        {
            return name;
        }


        #region ILinkedShader Members

        public void Enable()
        {
            GL.UseProgram(programHandle);
        }

        public void Disable()
        {
            GL.UseProgram(0);
            GL.ActiveTexture(TextureUnit.Texture0);
        }

        #endregion
    }

}
