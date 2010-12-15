#ifndef INC_IGRAPHICS_H
#define INC_IGRAPHICS_H

#include "poro_types.h"
#include "poro_macros.h"
#include "itexture.h"
//#include "igraphics_buffer.h"

#include <vector>

namespace poro {

	class IGraphicsBuffer;
	
class IGraphics 
{
public:
	virtual ~IGraphics() { }
	//-------------------------------------------------------------------------

	virtual bool		Init( int width, int height, bool fullscreen, const types::string& caption ) = 0;
	virtual void		SetInternalSize( types::Float32 width, types::Float32 height ) { poro_assert( false ); /* You have to implement this */ }

	virtual ITexture*	CreateTexture( int width, int height );
	void				SetTextureData(ITexture* texture, unsigned char* data ){ SetTextureData(texture, (void*)data ); }
	void				SetTextureData(ITexture* texture, float* data ){ SetTextureData(texture, (void*)data ); }
	virtual void		SetTextureData(ITexture* texture, void* data );
		
	virtual ITexture*	LoadTexture( const types::string& filename ) = 0;
	virtual void		ReleaseTexture( ITexture* texture )  = 0;
	
	virtual void		DrawTexture(	ITexture* texture, 
										types::Float32 x, types::Float32 y, types::Float32 w, types::Float32 h, 
										const types::fcolor& color, types::Float32 rotation )  = 0;
	virtual void		DrawTexture( ITexture* texture, types::vec2 vertices[ 4 ], types::vec2 tex_coords[ 4 ], const types::fcolor& color ) = 0;

	// haxored for alpha masking
	virtual void		DrawTextureWithAlpha( ITexture* texture, types::vec2* vertices, types::vec2* tex_coords, int count, const types::fcolor& color,
		ITexture* alpha_texture, types::vec2* alpha_vertices, types::vec2* alpha_tex_coords, const types::fcolor& alpha_color ) { poro_assert( false && "Needs to be implemented" ); }

	// haxored this for tesselation - Petri
	virtual void		DrawTriangle( ITexture* texture, types::vec2 vertices[ 3 ], types::vec2 tex_coords[ 3 ], const types::fcolor& color ) { }


	virtual void		BeginRendering() = 0;
	virtual void		EndRendering() = 0;

	//-------------------------------------------------------------------------

	virtual void		DrawLines( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color ) { }
	
	virtual IGraphicsBuffer* CreateGraphicsBuffer(int width, int height);
	virtual void DestroyGraphicsBuffer(IGraphicsBuffer* buffer);
};
	
	inline ITexture* IGraphics::CreateTexture(int width, int height) { 
		// If this fails, it means you have not implemented create texture
		poro_assert( false );
		return NULL;
	}
	
	inline void IGraphics::SetTextureData(ITexture* texture, void* data ) {
		// If this fails, it means you have not implemented set texture data
		poro_assert( false );
	}
	
	inline IGraphicsBuffer* IGraphics::CreateGraphicsBuffer(int width, int height) { 
		// If this fails, it means you should implement grafics buffer on your end of things
		poro_assert( false );
		return NULL;
	}
	inline void IGraphics::DestroyGraphicsBuffer(IGraphicsBuffer* buffer){
		// If this fails, it means you should implement grafics buffer on your end of things
		poro_assert( false );
	}
	

} // end o namespace poro

#endif