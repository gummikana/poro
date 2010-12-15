#ifndef INC_GRAPHICS_WIN_H
#define INC_GRAPHICS_WIN_H

#include "poro_types.h"
#include "igraphics.h"


namespace poro {

class ITexture;
class IGraphicsBuffer;

class GraphicsWin : public IGraphics
{
public:

	virtual bool		Init( int width, int height, bool fullscreen, const types::string& caption );
	virtual void		SetInternalSize( types::Float32 width, types::Float32 height );

	virtual ITexture*	CreateTexture( int width, int height );
	virtual void		SetTextureData(ITexture* texture, void* data );
	virtual ITexture*	LoadTexture( const types::string& filename );
	virtual void		ReleaseTexture( ITexture* texture );

	void				DrawTexture( ITexture* texture, 
									types::Float32 x, 
									types::Float32 y, 
									types::Float32 w, 
									types::Float32 h, 
									const types::fcolor& color, 
									types::Float32 rotation = 0.0f );

	void				DrawTexture( ITexture* texture, 
									types::vec2 vertices[ 4 ], 
									types::vec2 tex_coords[ 4 ], 
									const types::fcolor& color );

	virtual void		DrawTextureWithAlpha( 
		ITexture* texture, 
		types::vec2* vertices, 
		types::vec2* tex_coords, 
		int count, 
		const types::fcolor& color,
		ITexture* alpha_texture, 
		types::vec2* alpha_vertices, 
		types::vec2* alpha_tex_coords, 
		const types::fcolor& alpha_color );


	virtual void		DrawTriangle( ITexture* texture, types::vec2 vertices[ 3 ], types::vec2 tex_coords[ 3 ], const types::fcolor& color );

	virtual void		BeginRendering();
	virtual void		EndRendering();

	virtual void		DrawLines( const std::vector< poro::types::vec2 >& vertices, const types::fcolor& color );
	
	virtual IGraphicsBuffer* CreateGraphicsBuffer(int width, int height);
	virtual void DestroyGraphicsBuffer(IGraphicsBuffer* buffer);
	
	
};

} // end o namespace poro
#endif