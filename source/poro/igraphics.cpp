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
unsigned char *stbi_write_png_to_mem( unsigned char *pixels, int stride_bytes, int x, int y, int n, int *out_len ); // not public unless defining stb implementation

namespace ceng
{
	std::string GetFilename( const std::string& filename );
};

namespace poro
{
	// 2024/1/31 - added these to support procedural generation and editing of textures that are normally loaded from the filesystem, without changing the code that uses them.
	// MemoryImage_MakeReadWrite() can also be used to create procedural textures that look like file-based images to the game, but don't have a corresponding real file.

	const uint32 MEMORY_IMAGE_SLOTS = 8;
	const Image* read_image_slots[MEMORY_IMAGE_SLOTS] = { 0 };
	Image* write_image_slots[MEMORY_IMAGE_SLOTS] = { 0 };

	std::unordered_map<std::string, Image*> filename_to_memory_image;


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


	bool IGraphics::ImageExistAsFileOrMemoryImage( char const* filename )
	{
		if ( filename == NULL )
			return NULL;

		const Image* image = IGraphics::MemoryImage_Get( filename );
		return ( image && image->data ) ||
			Poro()->GetFileSystem()->DoesExist( FileLocation::WorkingDirectory, filename );
	}


	const Image* IGraphics::MemoryImage_MakeReadWrite( char const* filename, uint32 slot, int width, int height, int last_editor_id )
	{
		if ( slot >= MEMORY_IMAGE_SLOTS )
			return NULL;

		if ( filename == NULL )
			return NULL;

		poro_assert( filename );
		Image** image_ref = &filename_to_memory_image[filename];
		Image* image = NULL;
		if ( *image_ref )
		{
			image = *image_ref;
		}
		else
		{
			int comp;
			int w;
			int h;
			unsigned char* data = IMPL_ImageLoad( filename, &w, &h, &comp, 4, true, true );

			if ( data == NULL && w > 0 && h > 0 ) // if file load failed, allocate an empty bitmap in memory
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
				filename_to_memory_image.erase( filename_to_memory_image.find( filename ) );
				return NULL; // <--------------------------------- returns
			}

			image = new Image();
			image->width = w;
			image->height = h;
			image->data = (uint32*)data;

			*image_ref = image; // store to map
		}

		poro_assert( image );
		poro_assert( image->data );
		poro_assert( image->width > 0 );
		poro_assert( image->height > 0 );

		image->last_editor_id = last_editor_id;

		read_image_slots[slot] = image;
		write_image_slots[slot] = image;

		return image;
	}

	const Image* IGraphics::MemoryImage_Read( char const* filename, uint32 slot )
	{
		if ( slot >= MEMORY_IMAGE_SLOTS )
			return NULL;

		const Image* image = MemoryImage_Get( filename );
		if ( image )
		{
			cassert( image->data );
			read_image_slots[slot] = image;
			write_image_slots[slot] = NULL;
			return image;
		}

		return NULL;
	}

	void IGraphics::MemoryImage_ClearAccess()
	{
		for ( int i = 0; i < MEMORY_IMAGE_SLOTS; i++ )
		{
			read_image_slots[i] = NULL;
			write_image_slots[i] = NULL;
		}
	}

	// ideally we would just return a pointer to the data, but the lifetimes of various images have no consistent logic, 
	// so it's best we just return a new allocation, imitating the use of the stbi API, and clear the memory image map on game reset.
	unsigned char* IGraphics::MemoryImage_GetCopyOfBitmap( const char* filename, int *x, int *y, int *comp, int req_comp )
	{
		auto it = filename_to_memory_image.find( filename );

		if ( it != filename_to_memory_image.end() )
		{
			poro::Image* image = it->second;

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
			
			Poro()->GetGraphics()->ImageSave( ("debug/" + ceng::GetFilename( filename ) ).c_str(), image->width, image->height, req_comp, image_data, image->width * req_comp);

			*x = image->width;
			*y = image->height;
			if ( comp )
				*comp = req_comp; // is always 4
			return image_data;
		}

		return NULL;
	}


	const Image* IGraphics::MemoryImage_Get( const char* filename )
	{
		auto it = filename_to_memory_image.find( filename );
		return it != filename_to_memory_image.end() ? it->second : NULL;
	}


	void IGraphics::MemoryImage_FreeAll()
	{
		MemoryImage_ClearAccess();

		for ( auto& it : filename_to_memory_image )
		{
			if ( it.second )
			{
				if ( it.second->data )
				{
					poro_assert( it.second->data );
					free( it.second->data );
				}
				delete it.second;
			}
		}

		filename_to_memory_image.clear();
	}

	unsigned int IGraphics::MemoryImage_GetPixel( uint32 slot, uint32 x, uint32 y )
	{
		if ( slot >= MEMORY_IMAGE_SLOTS )
			return 0;

		const Image* image = read_image_slots[slot];
		if ( image && x < (uint32)image->width && y < (uint32)image->height )
		{
			poro_assert( image->data );
			return image->data[x + y * image->width];
		}

		return 0;
	}

	void IGraphics::MemoryImage_SetPixel( uint32 slot, uint32 x, uint32 y, uint32 color )
	{
		if ( slot >= MEMORY_IMAGE_SLOTS )
			return;

		const Image* image = read_image_slots[slot];
		if ( image && x < (uint32)image->width && y < (uint32)image->height )
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
