#include "imagetoarray.h"

#include <poro/iplatform.h>
#include <poro/igraphics.h>
#include <utils/color/ccolor.h>
#include <utils/debug.h>
#include <poro/external/stb_image_write.h>


//-----------------------------------------------------------------------------
void LoadImage( const std::string& filename, ceng::CArray2D< poro::types::Uint32 >& out_array2d, bool include_alpha )
{
	using namespace imagetoarray;

	// Cacheing this is a bit silly, use LoadImageConst for cached version
	TempTexture* surface = GetTexture(filename);
	if( surface == NULL || surface->data == NULL )
	{
		if( surface ) delete surface;

		logger_error << "LoadImage() - Failed to load image: " << filename << "\n";
		return;		
	}
	
	out_array2d.Resize( surface->w, surface->h );

	if( surface )
	{
		int i = 0;
		const int bpp = 4;
		auto data = surface->data;
		uint32 color = 0;
		if (include_alpha)
		{
			for (int y = 0; y < surface->h; ++y)
			{
				for (int x = 0; x < surface->w; ++x)
				{
					/*int co = (y * surface->width) * bpp + x * bpp;
					cassert(co == i);*/

					color = data[i+0] << 16 | data[i+1] << 8 | data[i+2] << 0 | data[i+3] << 24;
					i += 4;
					out_array2d.Rand(x, y) = color;
					// cassert( color == surface->GetPixel( x, y, include_alpha ) );
				}
			}
		}
		else
		{
			for (int y = 0; y < surface->h; ++y)
			{
				for (int x = 0; x < surface->w; ++x)
				{
					color = data[i+0] << 16 | data[i+1] << 8 | data[i+2] << 0;
					i += 4;
					out_array2d.Rand(x, y) = color;
				}
			}
		}
	}

	delete surface;
}

//-----------------------------------------------------------------------------
void SaveImage( const std::string& filename, const ceng::CArray2D< poro::types::Uint32 >& image_data )
{
	using namespace imagetoarray;

	// do the file and save it
	const int w = image_data.GetWidth();
	const int h = image_data.GetHeight();
	
	unsigned char* pixels = NULL;
	pixels = new unsigned char[ 4 * w * h ];	

	ceng::CColorUint8 c;
	for( int y = 0; y < h; ++y )
	{
		for( int x = 0; x < w; ++x )
		{
			c.Set32( image_data.Rand( x, y ) );

			int p = ( 4 * x + 4 * w * y);
			pixels[ p + 0 ] = c.GetR();
			pixels[ p + 1 ] = c.GetG();
			pixels[ p + 2 ] = c.GetB();
			pixels[ p + 3 ] = c.GetA();
		}
	}

	if( Poro() == NULL || Poro()->GetGraphics() == NULL )
	{
		stbi_write_png( filename.c_str(), w, h, 4, pixels, w * 4 );
	}
	else
	{
		Poro()->GetGraphics()->ImageSave( filename.c_str(), w, h, 4, pixels, w * 4 );
	}

	delete [] pixels;
	pixels = NULL;
}


namespace imagetoarray {

TempTexture::~TempTexture()
{
	Poro()->GetGraphics()->ImageFree( data );
	data = NULL;
}

TempTexture* GetTexture( const std::string& filename )
{
	TempTexture* result = new TempTexture;
	result->data = Poro()->GetGraphics()->ImageLoad( filename.c_str(), &result->width, &result->height, &result->bpp, 4 );
	if( result->data == NULL ) 
		logger_error << "GetTexture - Couldn't load file: " << filename << "\n";
	result->filename = filename;
	return result;
}

uint32 GetPixel(TempTexture* surface, int x, int y, bool include_alpha )
{
	if( surface == NULL ) return 0;
	return surface->GetPixel( x, y, include_alpha );
}


} // End of namespace imagetoarray