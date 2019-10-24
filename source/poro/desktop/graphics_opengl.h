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

#ifndef INC_GRAPHICS_OPENGL_H
#define INC_GRAPHICS_OPENGL_H

#include "../poro_types.h"
#include "../igraphics.h"


struct SDL_Window;

namespace poro {
//-----------------------------------------------------------------------------

class ITexture;
class ITexture3d;
class TextureOpenGL;

//-----------------------------------------------------------------------------

class GraphicsOpenGL : public IGraphics
{
public:
	GraphicsOpenGL();
	virtual ~GraphicsOpenGL() override;

	//-------------------------------------------------------------------------

	virtual bool				Init( int width, int height, int internal_width, int internal_height, int fullscreen, const types::string& caption ) PORO_OVERRIDE;
	virtual void				SetCaption( const types::string& caption ) PORO_OVERRIDE;
	virtual void				SetIcon( const std::string& icon_bmp_file );

	virtual void				SetInternalSize( types::Float32 width, types::Float32 height ) PORO_OVERRIDE;
	virtual void				SetInternalSizeAdvanced( types::Float32 left, types::Float32 right, types::Float32 bottom, types::Float32 top ) PORO_OVERRIDE;
	virtual poro::types::vec2	GetInternalSize() const PORO_OVERRIDE;
	virtual void				SetWindowSize(int width, int height) PORO_OVERRIDE;
	virtual poro::types::vec2	GetWindowSize() const PORO_OVERRIDE;
	virtual void				SetWindowPosition( int x, int y ) PORO_OVERRIDE;
	virtual void				SetWindowPositionCentered() PORO_OVERRIDE;
	virtual poro::types::vec2	GetWindowPosition() const PORO_OVERRIDE;
	virtual DisplayMode			GetCurrentDisplayMode() PORO_OVERRIDE;
	virtual void				GetDisplayModes( std::vector<DisplayMode>* out_modes ) PORO_OVERRIDE;
	virtual void				SetFullscreen(int fullscreen) PORO_OVERRIDE;
	virtual int					GetFullscreen() PORO_OVERRIDE;
	virtual void				SetVsync( VSYNC_MODE::Enum vsync ) PORO_OVERRIDE;
	virtual VSYNC_MODE::Enum	GetVsync() PORO_OVERRIDE;
	virtual bool				GetVsyncCurrentlyEnabled() PORO_OVERRIDE;
	virtual poro::types::vec2	GetViewPortSize() PORO_OVERRIDE { return mViewportSize; }
	virtual std::string			GetGraphicsHardwareInfo() PORO_OVERRIDE;

	//-------------------------------------------------------------------------

	virtual void				SetSettings( const GraphicsSettings& settings ) PORO_OVERRIDE;
	virtual GraphicsSettings	GetSettings() const PORO_OVERRIDE;
	
	//-------------------------------------------------------------------------

	virtual ITexture*	CreateTexture( int width, int height, TEXTURE_FORMAT::Enum format = TEXTURE_FORMAT::RGBA ) PORO_OVERRIDE;
	virtual void		DestroyTexture( ITexture* texture ) PORO_OVERRIDE;
	virtual ITexture*	CloneTexture( ITexture* other ) PORO_OVERRIDE;
	virtual void		SetTextureData(ITexture* texture, void* data ) PORO_OVERRIDE;
	virtual void		SetTextureData(ITexture* texture, void* data, int x, int y, int w, int h ) PORO_OVERRIDE;
	virtual ITexture*	LoadTexture( const types::string& filename ) PORO_OVERRIDE;
	virtual ITexture*	LoadTexture( const types::string& filename, bool store_raw_pixel_data ) PORO_OVERRIDE;
	virtual void		SetTextureFilteringMode( ITexture* itexture, TEXTURE_FILTERING_MODE::Enum mode ) const PORO_OVERRIDE;
	virtual void		SetTextureWrappingMode( ITexture* itexture, TEXTURE_WRAPPING_MODE::Enum  mode ) const PORO_OVERRIDE;
	virtual void		BeginScissorRect( const poro::types::ivec2& pos_min, const poro::types::ivec2& pos_max ) const PORO_OVERRIDE;
	virtual void		EndScissorRect() const PORO_OVERRIDE;

	//-------------------------------------------------------------------------

	virtual ITexture3d*	LoadTexture3d( const types::string& filename ) PORO_OVERRIDE;
	virtual void		DestroyTexture3d( ITexture3d* texture ) PORO_OVERRIDE;

	//-------------------------------------------------------------------------

	virtual IRenderTexture* RenderTexture_Create( int width, int height, TEXTURE_FILTERING_MODE::Enum filtering_mode ) PORO_OVERRIDE;
	virtual void RenderTexture_Destroy( IRenderTexture* texture ) PORO_OVERRIDE;
	virtual void RenderTexture_BeginRendering( IRenderTexture* texture, bool clear_color = false, bool clear_depth = false, float clear_r = 0.f, float clear_g = 0.f, float clear_b = 0.f, float clear_a = 0.f ) const PORO_OVERRIDE;
	virtual void RenderTexture_EndRendering( IRenderTexture* texture ) const PORO_OVERRIDE;
	virtual void RenderTexture_ReadTextureDataFromGPU( IRenderTexture* texture, uint8* out_pixels ) const PORO_OVERRIDE;
	virtual void RenderTexture_AsyncReadTextureDataFromGPUBegin( IRenderTexture* texture ) const PORO_OVERRIDE;
	virtual void RenderTexture_AsyncReadTextureDataFromGPUFinish( IRenderTexture* texture, uint8* out_pixels ) const PORO_OVERRIDE;
	virtual void RenderTexture_Blit( IRenderTexture* from, IRenderTexture* to, TEXTURE_FILTERING_MODE::Enum filtering_mode ) const PORO_OVERRIDE;

	//-------------------------------------------------------------------------

	virtual IShader* Shader_Create() PORO_OVERRIDE;
	virtual void Shader_Destroy( IShader* shader) PORO_OVERRIDE;
	virtual void Shader_Init( IShader* shader, const std::string& vertex_source_filename, const std::string& fragment_source_filename ) const PORO_OVERRIDE;
	virtual void Shader_InitFromString( IShader* shader, const std::string& vertex_source, const std::string& fragment_source ) const PORO_OVERRIDE;
	virtual void Shader_Release( IShader* shader ) const PORO_OVERRIDE;
	virtual void Shader_Enable( IShader* shader ) const PORO_OVERRIDE;
	virtual void Shader_Disable( IShader* shader ) const PORO_OVERRIDE;
	virtual bool Shader_HasParameter( IShader* shader, const std::string& name ) const PORO_OVERRIDE;
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, float value ) const PORO_OVERRIDE;
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, const types::vec2& value ) const PORO_OVERRIDE;
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, const types::vec3& value ) const PORO_OVERRIDE;
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, const types::vec2& value_xy, types::vec2& value_zw ) const PORO_OVERRIDE;
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, float x, float y, float z, float w ) const PORO_OVERRIDE;
	virtual void Shader_SetParameter( IShader* ishader, const std::string& name, const float* value_4_floats ) const PORO_OVERRIDE;
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, ITexture* texture, TEXTURE_FILTERING_MODE::Enum mode ) PORO_OVERRIDE;
	virtual void Shader_SetParameter( IShader* shader, const std::string& name, const ITexture3d* texture ) const PORO_OVERRIDE;
	virtual bool Shader_GetIsCompiledAndLinked( IShader* shader ) const PORO_OVERRIDE;

	virtual void SetShader( IShader* shader );

	//-------------------------------------------------------------------------

	virtual IVertexBuffer* VertexBuffer_Create( VERTEX_FORMAT::Enum vertex_format ) PORO_OVERRIDE;
	virtual void VertexBuffer_Destroy( IVertexBuffer* buffer ) PORO_OVERRIDE;
	virtual void VertexBuffer_SetData( IVertexBuffer* buffer, void* vertices, uint32 num_vertices ) const PORO_OVERRIDE;

	//-------------------------------------------------------------------------

	virtual void BeginRendering() PORO_OVERRIDE;
	virtual void IMPL_EndRendering() PORO_OVERRIDE;
	virtual void IMPL_SetVSyncEnabled( bool enabled ) PORO_OVERRIDE;
	
	//-------------------------------------------------------------------------

	virtual void LegacyBindTexture( const ITexture* texture ) PORO_OVERRIDE;
	virtual void DrawVertices( const types::Vertex_PosFloat2_ColorUint32* vertices, uint32 num_vertices, const poro::GraphicsState& state ) PORO_OVERRIDE;
	virtual void DrawVertices( const types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, const poro::GraphicsState& state ) PORO_OVERRIDE;
	virtual void DrawVertices( const types::Vertex_PosFloat2_2xTexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, const poro::GraphicsState& state ) PORO_OVERRIDE;
	virtual void DrawVertices( const types::Vertex_PosFloat2_3xTexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, const poro::GraphicsState& state ) PORO_OVERRIDE;
	virtual void DrawVertices( const IVertexBuffer* buffer, uint32 start_vertice, uint32 num_vertices, const poro::GraphicsState& state ) PORO_OVERRIDE;
	
	//-------------------------------------------------------------------------

	types::vec2 ConvertToInternalPos( int x, int y );
	void ResetWindow();
	void LoadOpenGL();

	//-------------------------------------------------------------------------

	void SaveScreenshot( const std::string& filename );
	void SaveScreenshot( const std::string& filename, int x, int y, int w, int h );
	int CaptureScreenshot( unsigned char* data, int max_size );

	//-------------------------------------------------------------------------

	virtual void SetMultithreadLock( bool multithreaded_lock ) PORO_OVERRIDE { mBufferLoadTextures = multithreaded_lock; }
	virtual bool GetMultithreadLock() const PORO_OVERRIDE { return mBufferLoadTextures; }

	virtual bool UpdateBufferedMultithreaded() PORO_OVERRIDE;

	void IMPL_AddTextureToBuffer( TextureOpenGL* buffer_me );
	TextureOpenGL* IMPL_LoadTexture( const types::string& filename, bool store_raw_pixel_data );
	TextureOpenGL* IMPL_CreateImage( uint8* pixels, int w, int h, TEXTURE_FORMAT::Enum format, bool store_raw_pixel_data );

private:
	SDL_Window* mSDLWindow;
	int		mWindowWidth;
	int		mWindowHeight;
	types::vec2 mViewportOffset;
	types::vec2 mViewportSize;

	int mDesktopWidth;
	int mDesktopHeight;
	
	bool mGlContextInitialized;

	bool mVsyncEnabled = false;

	// for multithreaded loading of things
	bool mBufferLoadTextures;
	std::vector< TextureOpenGL* > mBufferedLoadTextures;
};

types::Uint32 GetNextPowerOfTwo(types::Uint32 input);

//-----------------------------------------------------------------------------

// ImageLoad is just a wrapper for stbi_load and stbi_write_png
// unsigned char*	ImageLoad( char const *filename, int *x, int *y, int *comp, int req_comp );
// int				ImageSave( char const *filename, int x, int y, int comp, const void *data, int stride_bytes );

//-----------------------------------------------------------------------------

} // end o namespace poro
#endif