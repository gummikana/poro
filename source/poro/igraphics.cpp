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

#include <unordered_map>
#include "external/stb_image.h"
#include "external/stb_image_write.h"
#include "igraphics.h"
#include "itexture.h"
#include "utils/color/ccolor.h"
#include "poro.h"
#include "utils/string/string.h"
unsigned char *stbi_write_png_to_mem( unsigned char *pixels, int stride_bytes, int x, int y, int n, int *out_len ); // not public unless defining stb implementation

namespace ceng
{
	std::string GetFilename( const std::string& filename );
};

namespace poro
{
	// 2024/1/31 - added the MemoryImage system to support procedural generation and editing of textures that are normally loaded from the filesystem, without changing the code that uses them.
	// MemoryImage_MakeReadWrite() can also be used to create procedural textures that look like file-based images to the game, but don't have a corresponding real file.
	std::unordered_map<std::string, uint32> filename_to_memory_image_id;
	std::vector<MemoryImage> id_to_memory_image;


	// 2019-04-11 CLEANUP NOTE: For now we'll keep DrawTexture implementations as they were in GraphicsOpenGL. They'd sure use a cleanup but the main reason for that would be aesthetic.

	types::vec2 IMPL_Vec2Rotate( const types::vec2& point, const types::vec2& center, float angle )
	{
		types::vec2 D;
		D.x = point.x - center.x;
		D.y = point.y - center.y;

		// D.Rotate( angle );
		float tx = D.x;
		D.x = (float)D.x * (float)cos( angle ) - D.y * (float)sin( angle );
		D.y = (float)tx * (float)sin( angle ) + D.y * (float)cos( angle );

		// D += centre;
		D.x += center.x;
		D.y += center.y;

		return D;
	}

	void IMPL_DrawTexture( IGraphics* graphics, const ITexture* texture, const types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, BLEND_MODE::Enum blend_mode, DRAW_PRIMITIVE_MODE::Enum primitive_mode )
	{
		poro_assert( graphics );
		poro_assert( texture );

		GraphicsState state;
		state.blend_mode = blend_mode;
		state.primitive_mode = primitive_mode;

		graphics->LegacyBindTexture( texture );
		graphics->DrawVertices( vertices, num_vertices, state );
		graphics->LegacyBindTexture( 0 );
	}

	unsigned char* IMPL_ImageLoad( char const* filename, int* x, int* y, int* comp, int req_comp, bool return_without_rb_swap = false, bool loading_for_memory_image = false )
	{
		// comp might be NULL
		poro_assert( x );
		poro_assert( y );
		poro_assert( filename );

		unsigned char* result = NULL;
		
		if ( loading_for_memory_image == false )
			result = poro::IGraphics::MemoryImage_GetCopyOfBitmap( filename, x, y, comp, req_comp );

		if ( result == NULL )
		{
			FileDataTemp file;
			Poro()->GetFileSystem()->ReadWholeFileTemp( filename, &file );
			if ( file.IsValid() == false )
				return NULL;

			result = stbi_load_from_memory( (stbi_uc*)file.data, file.data_size_bytes, x, y, comp, req_comp );
		}

		if ( return_without_rb_swap )
			return result;

		// error reading the file, probably not image file
		if ( result == NULL )
			return NULL;

#define PORO_SWAP_RED_AND_BLUE
#ifdef PORO_SWAP_RED_AND_BLUE
		if ( comp && *comp == 4 )
		{
			int width = *x;
			int height = *y;
			int bpp = *comp;

			for ( int ix = 0; ix < width; ++ix )
			{
				for ( int iy = 0; iy < height; ++iy )
				{
					int i = ( iy * width ) * bpp + ix * bpp;
					// color = ((color & 0x000000FF) << 16) | ((color & 0x00FF0000) >> 16) | (color & 0xFF00FF00);
					/*data[ co ] << 16 |
					data[ co+1 ] << 8 |
					data[ co+2 ] << 0; // |*/

					unsigned char temp = result[i + 2];
					result[i + 2] = result[i];
					result[i] = temp;
				}
			}
		}
#endif

		return result;
	}

	// ---

	void IGraphics::DrawRect( const poro::types::vec2& position, const poro::types::vec2& size, const types::vec2* tex_coords )
	{
		const uint32 NUM_VERTICES = 4;

		poro_assert( tex_coords );

		types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32 vertices[NUM_VERTICES];
		vertices[0].x = (float)position.x;
		vertices[0].y = (float)position.y;
		vertices[1].x = (float)position.x;
		vertices[1].y = (float)( position.y + size.y );
		vertices[2].x = (float)( position.x + size.x );
		vertices[2].y = (float)position.y;
		vertices[3].x = (float)( position.x + size.x );
		vertices[3].y = (float)( position.y + size.y );

		for ( int i = 0; i < NUM_VERTICES; i++ )
		{
			vertices[i].u = tex_coords[i].x;
			vertices[i].v = tex_coords[i].y;
			vertices[i].color = 0xFFffFFff;
		}

		GraphicsState state;
		state.blend_mode = mBlendMode;
		state.primitive_mode = DRAW_PRIMITIVE_MODE::TRIANGLE_STRIP;

		DrawVertices( vertices, NUM_VERTICES, state );
	}

	void IGraphics::DrawRect( const poro::types::vec2& position, const poro::types::vec2& size, const types::vec2* tex_coords, const types::vec2* tex_coords2 )
	{
		const uint32 NUM_VERTICES = 4;

		poro_assert( tex_coords );
		poro_assert( tex_coords2 );

		types::Vertex_PosFloat2_2xTexCoordFloat2_ColorUint32 vertices[NUM_VERTICES];
		vertices[0].x = (float)position.x;
		vertices[0].y = (float)position.y;
		vertices[1].x = (float)position.x;
		vertices[1].y = (float)( position.y + size.y );
		vertices[2].x = (float)( position.x + size.x );
		vertices[2].y = (float)position.y;
		vertices[3].x = (float)( position.x + size.x );
		vertices[3].y = (float)( position.y + size.y );

		for ( int i = 0; i < NUM_VERTICES; i++ )
		{
			vertices[i].u = tex_coords[i].x;
			vertices[i].v = tex_coords[i].y;
			vertices[i].u2 = tex_coords2[i].x;
			vertices[i].v2 = tex_coords2[i].y;
			vertices[i].color = 0xFFffFFff;
		}

		GraphicsState state;
		state.blend_mode = mBlendMode;
		state.primitive_mode = DRAW_PRIMITIVE_MODE::TRIANGLE_STRIP;

		DrawVertices( vertices, NUM_VERTICES, state );
	}

	void IGraphics::DrawRect( const poro::types::vec2& position, const poro::types::vec2& size, const types::vec2* tex_coords, const types::vec2* tex_coords2, const types::vec2* tex_coords3 )
	{
		const uint32 NUM_VERTICES = 4;

		poro_assert( tex_coords );
		poro_assert( tex_coords2 );
		poro_assert( tex_coords3 );

		types::Vertex_PosFloat2_3xTexCoordFloat2_ColorUint32 vertices[NUM_VERTICES];
		vertices[0].x = (float)position.x;
		vertices[0].y = (float)position.y;
		vertices[1].x = (float)position.x;
		vertices[1].y = (float)( position.y + size.y );
		vertices[2].x = (float)( position.x + size.x );
		vertices[2].y = (float)position.y;
		vertices[3].x = (float)( position.x + size.x );
		vertices[3].y = (float)( position.y + size.y );

		for ( int i = 0; i < NUM_VERTICES; i++ )
		{
			vertices[i].u = tex_coords[i].x;
			vertices[i].v = tex_coords[i].y;
			vertices[i].u2 = tex_coords2[i].x;
			vertices[i].v2 = tex_coords2[i].y;
			vertices[i].u3 = tex_coords3[i].x;
			vertices[i].v3 = tex_coords3[i].y;
			vertices[i].color = 0xFFffFFff;
		}

		GraphicsState state;
		state.blend_mode = mBlendMode;
		state.primitive_mode = DRAW_PRIMITIVE_MODE::TRIANGLE_STRIP;

		DrawVertices( vertices, NUM_VERTICES, state );
	}

	void IGraphics::DrawTexture( const ITexture* itexture, float x, float y, float w, float h, const types::fcolor& color, float rotation )
	{
		poro_assert( itexture );

		if ( itexture == NULL )
			return;

		if ( color[3] <= 0 )
			return;

		types::vec2 temp_verts[4];
		types::vec2 tex_coords[4];

		temp_verts[0].x = (float)x;
		temp_verts[0].y = (float)y;
		temp_verts[1].x = (float)x;
		temp_verts[1].y = (float)( y + h );
		temp_verts[2].x = (float)( x + w );
		temp_verts[2].y = (float)y;
		temp_verts[3].x = (float)( x + w );
		temp_verts[3].y = (float)( y + h );

		if ( rotation != 0 )
		{
			types::vec2 center_p;
			center_p.x = temp_verts[0].x + ( ( temp_verts[3].x - temp_verts[0].x ) * 0.5f );
			center_p.y = temp_verts[0].y + ( ( temp_verts[3].y - temp_verts[0].y ) * 0.5f );

			for ( int i = 0; i < 4; ++i )
			{
				temp_verts[i] = IMPL_Vec2Rotate( temp_verts[i], center_p, rotation );
			}
		}

		float tx1 = 0;
		float ty1 = 0;
		float tx2 = (float)itexture->GetWidth();
		float ty2 = (float)itexture->GetHeight();

		tex_coords[0].x = tx1;
		tex_coords[0].y = ty1;

		tex_coords[1].x = tx1;
		tex_coords[1].y = ty2;

		tex_coords[2].x = tx2;
		tex_coords[2].y = ty1;

		tex_coords[3].x = tx2;
		tex_coords[3].y = ty2;

		DrawTexture( itexture, temp_verts, tex_coords, 4, color, DRAW_PRIMITIVE_MODE::TRIANGLE_STRIP );
	}

	void IGraphics::DrawTexture( const ITexture* itexture, types::vec2* vertices, types::vec2* tex_coords, uint32 num_vertices, const types::fcolor& color, DRAW_PRIMITIVE_MODE::Enum primitive_mode )
	{
		poro_assert( itexture );
		poro_assert( num_vertices <= 8 );
		poro_assert( num_vertices <= 4 );

		if ( itexture == NULL )
			return;

		if ( color[3] <= 0 )
			return;

		// do some transformations to the texture coordinates
		const float external_size_x = itexture->GetExternalSizeX();
		const float external_size_y = itexture->GetExternalSizeY();

		for ( uint32 i = 0; i < num_vertices; ++i )
		{
			tex_coords[i].x *= external_size_x;
			tex_coords[i].y *= external_size_y;
		}

		types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32 vert[8];

		const float x_text_conv = ( 1.f / itexture->GetDataWidth() ) * ( itexture->GetUV( 2 ) - itexture->GetUV( 0 ) );
		const float y_text_conv = ( 1.f / itexture->GetDataHeight() ) * ( itexture->GetUV( 3 ) - itexture->GetUV( 1 ) );
		const uint32 color32 = ::types::fcolor( color ).Get32();
		for ( uint32 i = 0; i < num_vertices; ++i )
		{
			vert[i].x = vertices[i].x;
			vert[i].y = vertices[i].y;
			vert[i].u = itexture->GetUV( 0 ) + ( tex_coords[i].x * x_text_conv );
			vert[i].v = itexture->GetUV( 1 ) + ( tex_coords[i].y * y_text_conv );
			vert[i].color = color32;
		}

		IMPL_DrawTexture( this, itexture, vert, num_vertices, mBlendMode, primitive_mode );
	}

	void IGraphics::DrawTexturedRect( const ITexture* itexture, const poro::types::vec2& position, const poro::types::vec2& size, const types::vec2* tex_coords, const poro::types::fcolor& color )
	{
		const uint32 NUM_VERTICES = 4;

		poro_assert( itexture );
		poro_assert( tex_coords );

		types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32 vertices[NUM_VERTICES];
		vertices[0].x = (float)position.x;
		vertices[0].y = (float)position.y;
		vertices[1].x = (float)position.x;
		vertices[1].y = (float)( position.y + size.y );
		vertices[2].x = (float)( position.x + size.x );
		vertices[2].y = (float)position.y;
		vertices[3].x = (float)( position.x + size.x );
		vertices[3].y = (float)( position.y + size.y );

		const uint32 color32 = ::types::fcolor( color ).Get32();
		for ( int i = 0; i < NUM_VERTICES; i++ )
		{
			vertices[i].u = tex_coords[i].x / itexture->GetWidth();
			vertices[i].v = tex_coords[i].y / itexture->GetHeight();
			vertices[i].color = color32;
		}

		IMPL_DrawTexture( this, itexture, vertices, NUM_VERTICES, mBlendMode, DRAW_PRIMITIVE_MODE::TRIANGLE_STRIP );
	}


	void IGraphics::DrawLines( const types::Vertex_PosFloat2_ColorUint32* vertices, uint32 num_vertices, bool smooth, float width, bool loop ) 
	{
		GraphicsState state;
		state.blend_mode = mBlendMode;
		state.line_width = width;
		if ( smooth )
			state.flags = DRAW_FLAGS::LINE_SMOOTH;
		state.primitive_mode = loop ? DRAW_PRIMITIVE_MODE::LINE_LOOP : DRAW_PRIMITIVE_MODE::LINE_STRIP;

		DrawVertices( vertices, num_vertices, state );
	}

	void IGraphics::DrawLines( const std::vector< types::Vertex_PosFloat2_ColorUint32 >& vertices, bool smooth, float width, bool loop )
	{ 
		poro_assert( vertices.empty() == false);
		DrawLines( &vertices[0], vertices.size(), smooth, width, loop ); 
	}

	void IGraphics::DrawLines( const std::vector< types::vec2 >& vertices, const poro::types::fcolor& color, bool smooth, float width, bool loop )
	{
		static std::vector< types::Vertex_PosFloat2_ColorUint32 > pvertices;
		pvertices.clear();

		const uint32 color32 = ::types::fcolor( poro::GetFColor( color[0], color[1], color[2], color[3] ) ).Get32();

		for ( uint32 i = 0; i < vertices.size(); i++ )
			pvertices.push_back( types::Vertex_PosFloat2_ColorUint32{ vertices[i].x, vertices[i].y, color32 } );

		DrawLines( pvertices, smooth, width, loop );
	}

	void IGraphics::DrawQuads( const types::Vertex_PosFloat2_ColorUint32* vertices, uint32 num_vertices )
	{
		GraphicsState state;
		state.blend_mode = mBlendMode;
		state.primitive_mode = DRAW_PRIMITIVE_MODE::QUADS;

		DrawVertices( vertices, num_vertices, state );
	}

	void IGraphics::DrawQuads( const types::Vertex_PosFloat2_TexCoordFloat2_ColorUint32* vertices, uint32 num_vertices, const ITexture* texture )
	{
		poro_assert( texture );

		IMPL_DrawTexture( this, texture, vertices, num_vertices, mBlendMode, DRAW_PRIMITIVE_MODE::QUADS );
	}

	void IGraphics::DrawVertices( const std::vector< types::Vertex_PosFloat2_ColorUint32 >& vertices, const poro::GraphicsState& state )
	{
		poro_assert( vertices.empty() == false );
		DrawVertices( &vertices[0], vertices.size(), state );
	}


	void IGraphics::EndRendering()
	{
		const auto app_config = Poro()->GetApplicationConfig();
		poro_assert( app_config );

		// adaptive vsync
		if ( app_config->graphics_settings.vsync == VSYNC_MODE::ADAPTIVE )
		{
			const float vsync_off_threshold = ADAPTIVE_VSYNC_OFF_THRESHOLD * app_config->framerate;
			const float vsync_on_threshold = (float)app_config->framerate - 1.0f;

			const auto graphics = Poro()->GetGraphics();

			float framerate = ceng::math::Min( (float)app_config->framerate, (float)(1.0 / Poro()->GetLastFrameExecutionTime()) );
			if ( mAdaptiveVsyncSamplingEnabled && Poro()->GetFrameNum() > 30 )
			{
				mAdaptiveVsyncSamples[mAdaptiveVsyncSampleIndex] = framerate;
				mAdaptiveVsyncSampleIndex++;
				if ( mAdaptiveVsyncSampleIndex >= NUM_ADAPTIVE_VSYNC_SAMPLES )
					mAdaptiveVsyncSampleIndex = 0;
			}

			float fps_average = 0;
			for ( auto sample : mAdaptiveVsyncSamples )
				fps_average += sample;

			fps_average /= NUM_ADAPTIVE_VSYNC_SAMPLES;

			const bool vsync_enabled = graphics->GetVsyncCurrentlyEnabled();

			if ( fps_average <= vsync_off_threshold && vsync_enabled )
				IMPL_SetVSyncEnabled( false );
			else if ( fps_average >= vsync_on_threshold && ( vsync_enabled == false ) )
				IMPL_SetVSyncEnabled( true );
		}

		// ---
		if ( mCurrentShader )
			Shader_Disable( mCurrentShader );
		mCurrentShader = NULL;

		// ---
		IMPL_EndRendering();
	}

	void IGraphics::GraphicsFreeLeakedResources()
	{
		std::vector<ITexture*> _textures;
		std::vector<ITexture3d*> _texture3Ds;
		std::vector<IRenderTexture*> _rendertextures;
		std::vector<IShader*> _shaders;
		std::vector<IVertexBuffer*> _vertexbuffers;

		for ( auto res : textures )
			_textures.push_back( res );
		for ( auto res : texture3Ds )
			_texture3Ds.push_back( res );
		for ( auto res : rendertextures )
			_rendertextures.push_back( res );
		for ( auto res : shaders )
			_shaders.push_back( res );
		for ( auto res : vertexbuffers )
			_vertexbuffers.push_back( res );

		for ( auto res : _textures )
			DestroyTexture( res );
		for ( auto res : _texture3Ds )
			DestroyTexture3d( res );
		for ( auto res : _rendertextures )
			RenderTexture_Destroy( res );
		for ( auto res : _shaders )
			delete res; // man, this API
		for ( auto res : _vertexbuffers )
			VertexBuffer_Destroy( res );

		textures.clear();
		texture3Ds.clear();
		rendertextures.clear();
		shaders.clear();
		vertexbuffers.clear();
	}


	unsigned char* IGraphics::ImageLoadTemp( char const *filename, int *x, int *y, int *comp, int req_comp )
	{
		return IMPL_ImageLoad( filename, x, y, comp, req_comp );
	}

	unsigned char* IGraphics::ImageLoad( char const *filename, int *x, int *y, int *comp, int req_comp )
	{
		return IMPL_ImageLoad(  filename, x, y, comp, req_comp );
	}

	void IGraphics::ImageFree( unsigned char* image )
	{
		free( image );
	}

	int	IGraphics::ImageSave( char const *filename, int x, int y, int comp, const void *data, int stride_bytes )
	{
		int png_size_bytes;
		unsigned char* png_memory = stbi_write_png_to_mem( (unsigned char*)data, stride_bytes, x, y, comp, &png_size_bytes );
		if ( png_memory != 0 && png_size_bytes > 0 )
		{
			StreamStatus::Enum write_status = Poro()->GetFileSystem()->WriteWholeFile( filename, (char*)png_memory, (unsigned int)png_size_bytes, StreamWriteMode::Recreate, FileLocation::WorkingDirectory );
			if ( write_status != StreamStatus::NoError )
				poro_logger << "Error ImageSave() - couldn't write to file: " << filename << "\n";
		}
		else
		{
			return 0;
		}
		free( png_memory );

		return 1;
	}


	bool IGraphics::ImageExistsAsFileOrMemoryImage( char const* filename )
	{
		// NOTE: this should not have any side effects that endanger thread safety
		if ( filename == NULL )
			return NULL;

		const MemoryImageHandle handle = IGraphics::MemoryImage_GetFromFilename( filename );
		return ( handle.id != MEMORYIMAGE_NULL_ID ) ||
			Poro()->GetFileSystem()->DoesExist( filename );
	}




	// copied here so we can use this without linking to a certain codebase
	bool MemoryImage_StringStartsWith( const std::string& in, const std::string& what )
	{
		if ( in.size() < what.size() )
			return false;
		if ( in.empty() )
			return false;

		for ( size_t i = 0; i < what.size(); i++ )
		{
			if ( in[i] != what[i] )
				return false;
		}

		return true;
	}

	const MemoryImageHandle IGraphics::MemoryImage_MakeReadWrite( char const* filename, int width, int height, int last_editor_id )
	{
		if ( filename == NULL )
			return MemoryImageHandle();
		
		poro_assert( filename );

		MemoryImageHandle handle = MemoryImage_GetFromFilename( filename );

		if ( handle.id == MEMORYIMAGE_NULL_ID )
		{
			// create new memory image
			int comp;
			int w;
			int h;
			unsigned char* data = NULL;
			if ( MemoryImage_StringStartsWith( filename, "data/" ) || MemoryImage_StringStartsWith( filename, "mods/" ) ) // hax for certain codebase, because otherwise it reports an error
				data = IMPL_ImageLoad( filename, &w, &h, &comp, 4, true, true );

			if ( data == NULL && width > 0 && height > 0 ) // if file load failed, allocate an empty bitmap in memory
			{
				// ensure size doesn't overflow int. though at this size it's possible the allocation fails
				w = ceng::math::Clamp( width, 0, 1024 * 16 );
				h = ceng::math::Clamp( height, 0, 1024 * 16 );

				const int data_size_bytes = w * h * sizeof( unsigned int );
				data = (unsigned char*)malloc( data_size_bytes ); // allocate an in-memory-image. malloc, because ImageFree could touch it and uses free()
				if ( data )
					memset( data, 0, data_size_bytes );
			}

			if ( data == NULL ) // if couldn't create image at all
			{
				return handle; // <--------------------------------- returns
			}

			poro_assert( data );
			poro_assert( w > 0 );
			poro_assert( h > 0 );

			const uint32 id = id_to_memory_image.size();

			MemoryImage image;
			image.width = w;
			image.height = h;
			image.data = (uint32*)data;
			image.last_editor_id = last_editor_id;

			id_to_memory_image.push_back( image );
			filename_to_memory_image_id[filename] = id;

			handle.id = id;
			handle.image = &id_to_memory_image.back();
		}
		else
		{
			id_to_memory_image[handle.id].last_editor_id = last_editor_id;
		}

		return handle;
	}

	// ideally we would just return a pointer to the data, but the lifetimes of various images have no consistent logic, 
	// so it's best we just return a new allocation, imitating the use of the stbi API, and clear the memory image map on game reset.
	unsigned char* IGraphics::MemoryImage_GetCopyOfBitmap( const char* filename, int *x, int *y, int *comp, int req_comp )
	{
		poro_assert( filename );
		poro_assert( x );
		poro_assert( y );

		// NOTE: this should not have any side effects that endanger thread safety
		const MemoryImageHandle img = MemoryImage_GetFromFilename( filename );
		if ( img.id != MEMORYIMAGE_NULL_ID )
		{
			poro_assert( img.image );

			const MemoryImage* image = img.image;

			poro_assert( image );
			poro_assert( image->data );
			poro_assert( req_comp == 3 || req_comp == 4 );

			const int image_src_size_bytes = image->width * image->height * 4;
			const int image_size_bytes = image->width * image->height * req_comp;
			unsigned char* image_data = (unsigned char*)malloc( image_size_bytes );

			if ( req_comp == 4 )
			{
				memcpy( image_data, image->data, image_size_bytes );
			}
			else
			{
				const unsigned char* src = (unsigned char*)image->data;
				int write_index = 0;
				for ( int i = 0; i < image_src_size_bytes; i += 4 )
				{
					// ABGR
					image_data[write_index++] = src[i+0];
					image_data[write_index++] = src[i+1];
					image_data[write_index++] = src[i+2];
				}
			}

			//Poro()->GetGraphics()->ImageSave( ("temptemp/" + ceng::GetFilename( filename ) ).c_str(), image->width, image->height, req_comp, image_data, image->width * req_comp);

			*x = image->width;
			*y = image->height;
			if ( comp )
				*comp = req_comp; // is always 4
			return image_data;
		}

		return NULL;
	}


	const MemoryImageHandle IGraphics::MemoryImage_GetFromFilename( const char* filename )
	{
		// NOTE: this should not have any side effects that endanger thread safety
		MemoryImageHandle handle;

		if ( filename )
		{
			handle.id = MEMORYIMAGE_NULL_ID;

			auto it = filename_to_memory_image_id.find( filename );
			if ( it != filename_to_memory_image_id.end() )
			{
				poro_assert( it->second < id_to_memory_image.size() );

				const MemoryImage* image = &id_to_memory_image[it->second];
				handle.id = it->second;
				handle.image = image;
			}
		}

		return handle;
	}


	void IGraphics::MemoryImage_InitAndFreeExisting()
	{
		for ( MemoryImage& image : id_to_memory_image )
		{
			if ( image.data )
				free( image.data );
		}

		id_to_memory_image.clear();
		filename_to_memory_image_id.clear();

		// allocate an non-image in the first slot, for id 0. 
		// the image will have 0,0 as its size so getpixel and setpixel calls will fail without accessing any data, and we don't need a separate check for MEMORYIMAGE_NULL_ID
		id_to_memory_image.push_back( MemoryImage() );

		poro_assert( MEMORYIMAGE_NULL_ID == 0 );
		poro_assert( id_to_memory_image[MEMORYIMAGE_NULL_ID].width == 0 );
		poro_assert( id_to_memory_image[MEMORYIMAGE_NULL_ID].height == 0 );
	}

	unsigned int IGraphics::MemoryImage_GetPixel( uint32 id, uint32 x, uint32 y )
	{
		// NOTE: this should not have any side effects that endanger thread safety
		if ( id >= id_to_memory_image.size() )
			return 0;

		const MemoryImage* image = &id_to_memory_image[id];
		poro_assert( image );
		if ( x < (uint32)image->width && y < (uint32)image->height )
		{
			poro_assert( image->data );
			return image->data[x + y * image->width];
		}

		return 0;
	}

	void IGraphics::MemoryImage_SetPixel( uint32 id, uint32 x, uint32 y, uint32 color )
	{
		// NOTE: this should not have any side effects that endanger thread safety
		if ( id >= id_to_memory_image.size() )
			return;

		const MemoryImage* image = &id_to_memory_image[id];
		poro_assert( image );
		if ( x < (uint32)image->width && y < (uint32)image->height )
		{
			poro_assert( image->data );
			image->data[x + y * image->width] = color;
		}
	}



	void IGraphics::IMPL_InitAdaptiveVSync()
	{
		const auto app_config = Poro()->GetApplicationConfig();
		poro_assert( app_config );
		const float initial_sample = (float)app_config->framerate;// *( ADAPTIVE_VSYNC_OFF_THRESHOLD + 1.0f ) * 0.5f;
		for ( float& sample : mAdaptiveVsyncSamples )
		{
			sample = initial_sample;
		}
	}

}
